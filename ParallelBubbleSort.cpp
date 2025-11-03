#include <cstdlib>      // Для rand(), srand()
#include <cstdio>       // Для printf(), scanf(), setvbuf()
#include <cstring>      // Для роботи з рядками
#include <ctime>        // Для time(), clock()
#include <cmath>        // Для математичних функцій
#include <algorithm>    // Для std::sort, std::merge, std::copy, std::equal
#include <mpi.h>        // Для MPI функцій

using namespace std;

// --- Глобальні константи та змінні ---
const double RandomDataMultiplier = 1000.0;
int ProcNum = 0;    // Кількість доступних процесів
int ProcRank = -1;  // Ранг поточного процесу

// Режим розділення даних при злитті
enum split_mode { KeepFirstHalf, KeepSecondHalf };

// --- Прототипи функцій ---
void ProcessInitialization(double *&pData, int& DataSize, double *&pProcData, int& BlockSize);
void ProcessTermination(double *pData, double *pProcData);
void DummyDataInitialization(double*& pData, int& DataSize);
void RandomDataInitialization(double *&pData, int& DataSize);
void DataDistribution(double *pData, int DataSize, double *pProcData, int BlockSize);
void DataCollection(double *pData, int DataSize, double *pProcData, int BlockSize);
void ParallelBubble(double *pProcData, int BlockSize);
void ExchangeData(double *pProcData, int BlockSize, int DualRank, double *pDualData, int DualBlockSize);
void TestDistribution(double *pData, int DataSize, double *pProcData, int BlockSize);
void ParallelPrintData(double *pProcData, int BlockSize);
void TestResult(double *pData, double *pSerialData, int DataSize);
void CopyData(double *pData, int DataSize, double *pDataCopy);
bool CompareData(double *pData1, double *pData2, int DataSize);
void SerialBubbleSort(double *pData, int DataSize);
void SerialStdSort(double *pData, int DataSize);
void PrintData(double *pData, int DataSize);

// --- Головна функція ---
int main(int argc, char *argv[]) {
    double *pData = 0;
    double *pProcData = 0;
    int DataSize = 0;
    int BlockSize = 0;
    double *pSerialData = 0;
    double start, finish;
    double duration = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if(ProcRank == 0)
        printf("Parallel bubble sort program\n");

    // Ініціалізація процесу
    ProcessInitialization(pData, DataSize, pProcData, BlockSize);

    if (ProcRank == 0) {
        pSerialData = new double[DataSize];
        CopyData(pData, DataSize, pSerialData);
    }

    start = MPI_Wtime();

    // Розподіл початкових даних між процесами
    DataDistribution(pData, DataSize, pProcData, BlockSize);

    // Тестування розподілу даних
    // TestDistribution(pData, DataSize, pProcData, BlockSize);

    // Паралельне сортування бульбашкою
    ParallelBubble(pProcData, BlockSize);

    // Друк відсортованих даних (паралельно)
    // ParallelPrintData(pProcData, BlockSize);
    
    // Збір даних
    DataCollection(pData, DataSize, pProcData, BlockSize);
    
    // Тестування результату
    // TestResult(pData, pSerialData, DataSize);

    finish = MPI_Wtime();
    duration = finish - start;

    if(ProcRank == 0)
        printf("Time of execution: %f\n", duration);

    if (ProcRank == 0)
        delete []pSerialData;

    // Завершення процесу
    ProcessTermination(pData, pProcData);
    MPI_Finalize();

    return 0;
}

// --- Реалізації функцій ---

// Функція для виділення пам'яті та встановлення початкових значень
void ProcessInitialization(double *&pData, int& DataSize, double *&pProcData, int& BlockSize) {
    setvbuf(stdout, 0, _IONBF, 0);

    if(ProcRank == 0) {
        do {
            printf("Enter the size of data to be sorted: ");
            scanf("%d", &DataSize);

            if(DataSize < ProcNum)
                printf("Data size should be greater than number of processes\n");
        } while(DataSize < ProcNum);
            printf("Sorting %d data items\n", DataSize);
    }

    // Розсилка розміру даних
    MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int RestData = DataSize;
    for(int i = 0; i < ProcRank; i++)
        RestData -= RestData / (ProcNum - i);
    BlockSize = RestData / (ProcNum - ProcRank);
    
    pProcData = new double[BlockSize];
    
    if(ProcRank == 0) {
        pData = new double[DataSize];
        // Ініціалізація даних
        //RandomDataInitialization(pData, DataSize);
        DummyDataInitialization(pData, DataSize);
    }
}

// Функція для завершення обчислювального процесу
void ProcessTermination(double *pData, double *pProcData) {
    if(ProcRank == 0)
        delete []pData;
    delete []pProcData;
}

// Функція для простого заповнення даних
void DummyDataInitialization(double*& pData, int& DataSize) {
    for(int i = 0; i < DataSize; i++)
        pData[i] = DataSize - i;
}

// Функція для ініціалізації даних генератором випадкових чисел
void RandomDataInitialization(double *&pData, int& DataSize) {
    srand( (unsigned)time(0) );

    for(int i = 0; i < DataSize; i++)
        pData[i] = double(rand()) / RAND_MAX * RandomDataMultiplier;
}

// Розподіл даних між процесами
void DataDistribution(double *pData, int DataSize, double *pProcData, int BlockSize) {
    int *pSendInd = new int[ProcNum];
    int *pSendNum = new int[ProcNum];
    int RestData = DataSize;
    int CurrentSize = DataSize / ProcNum;

    pSendNum[0] = CurrentSize;
    pSendInd[0] = 0;

    for(int i = 1; i < ProcNum; i++) {
        RestData -= CurrentSize;
        CurrentSize = RestData / (ProcNum - i);
        pSendNum[i] = CurrentSize;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }

    MPI_Scatterv(pData, pSendNum, pSendInd, MPI_DOUBLE, pProcData,
    pSendNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    delete [] pSendNum;
    delete [] pSendInd;
}

// Функція для збору даних
void DataCollection(double *pData, int DataSize, double *pProcData, int BlockSize) {
    int *pReceiveNum = new int[ProcNum];
    int *pReceiveInd = new int[ProcNum];
    int RestData = DataSize;

    pReceiveInd[0] = 0;
    pReceiveNum[0] = DataSize / ProcNum;

    for(int i = 1; i < ProcNum; i++) {
        RestData -= pReceiveNum[i - 1];
        pReceiveNum[i] = RestData / (ProcNum - i);
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
    }

    MPI_Gatherv(pProcData, BlockSize, MPI_DOUBLE, pData,
    pReceiveNum, pReceiveInd, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    delete []pReceiveNum;
    delete []pReceiveInd;
}

// Паралельний алгоритм сортування бульбашкою
void ParallelBubble(double *pProcData, int BlockSize) {
    
    // ==========================================================
    // ЗМІНА ТУТ:
    // ==========================================================
    
    // Локальне сортування даних процесу
    SerialBubbleSort(pProcData, BlockSize);
    // SerialStdSort(pProcData, BlockSize); // <--- Тепер це закоментовано

    // ==========================================================

    int Offset;
    split_mode SplitMode;

    for(int i = 0; i < ProcNum; i++) {
        if((i % 2) == 1) { // Непарна ітерація
            if((ProcRank % 2) == 1) { // Непарні процеси
                Offset = 1;
                SplitMode = KeepFirstHalf;
            } else { // Парні процеси
                Offset = -1;
                SplitMode = KeepSecondHalf;
            }
        } else { // Парна ітерація
            if((ProcRank % 2) == 1) { // Непарні процеси
                Offset = -1;
                SplitMode = KeepSecondHalf;
            } else { // Парні процеси
                Offset = 1;
                SplitMode = KeepFirstHalf;
            }
        }
        
        // Перевірка для першого та останнього процесів
        if((ProcRank == ProcNum - 1) && (Offset == 1)) continue;
        if((ProcRank == 0 ) && (Offset == -1)) continue;

        MPI_Status status;
        int DualBlockSize;

        MPI_Sendrecv(&BlockSize, 1, MPI_INT, ProcRank + Offset, 0,
        &DualBlockSize, 1, MPI_INT, ProcRank + Offset, 0,
        MPI_COMM_WORLD, &status);

        double *pDualData = new double[DualBlockSize];
        double *pMergedData = new double[BlockSize + DualBlockSize];

        // Обмін даними
        ExchangeData(pProcData, BlockSize, ProcRank + Offset, pDualData,
        DualBlockSize);

        // Злиття даних
        merge(pProcData, pProcData + BlockSize, pDualData, pDualData +
        DualBlockSize, pMergedData);

        // Розділення даних
        if(SplitMode == KeepFirstHalf)
            copy(pMergedData, pMergedData + BlockSize, pProcData);
        else
            copy(pMergedData + BlockSize, pMergedData + BlockSize +
            DualBlockSize, pProcData);
        
        delete []pDualData;
        delete []pMergedData;
    }
}

// Функція для обміну даними між сусідніми процесами
void ExchangeData(double *pProcData, int BlockSize, int DualRank,
double *pDualData, int DualBlockSize) {
    MPI_Status status;
    MPI_Sendrecv(pProcData, BlockSize, MPI_DOUBLE, DualRank, 0,
    pDualData, DualBlockSize, MPI_DOUBLE, DualRank, 0,
    MPI_COMM_WORLD, &status);
}

// Функція для тестування розподілу даних
void TestDistribution(double *pData, int DataSize, double *pProcData, int BlockSize) {
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (ProcRank == 0) {
        printf("Initial data:\n");
        PrintData(pData, DataSize);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int i = 0; i < ProcNum; i++) {
        if (ProcRank == i) {
            printf("ProcRank = %d\n", ProcRank);
            printf("Block:\n");
            PrintData(pProcData, BlockSize);
        }
    
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Функція для паралельного виводу даних
void ParallelPrintData(double *pProcData, int BlockSize) {
    for(int i = 0; i < ProcNum; i++) {
        if (ProcRank == i) {
            printf("ProcRank = %d\n", ProcRank);
            printf("Proc sorted data: \n");
            PrintData(pProcData, BlockSize);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Функція для тестування результату паралельного сортування
void TestResult(double *pData, double *pSerialData, int DataSize) {
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(ProcRank == 0) {
        SerialBubbleSort(pSerialData, DataSize);
        
        //SerialStdSort(pSerialData, DataSize);
        if(!CompareData(pData, pSerialData, DataSize))
            printf("The results of serial and parallel algorithms are "
            "NOT identical. Check your code\n");
        else
            printf("The results of serial and parallel algorithms are "
            "identical\n");
    }
}

// Функція для копіювання даних
void CopyData(double *pData, int DataSize, double *pDataCopy) {
    copy(pData, pData + DataSize, pDataCopy);
}

// Функція для порівняння даних
bool CompareData(double *pData1, double *pData2, int DataSize) {
    return equal(pData1, pData1 + DataSize, pData2);
}

// Послідовний алгоритм сортування бульбашкою
void SerialBubbleSort(double *pData, int DataSize) {
    double Tmp;
    for(int i = 1; i < DataSize; i++)
        for(int j = 0; j < DataSize - i; j++)
            if(pData[j] > pData[j + 1]) {
                Tmp = pData[j];
                pData[j] = pData[j + 1];
                pData[j + 1] = Tmp;
            }
}

// Сортування за допомогою стандартного алгоритму
void SerialStdSort(double *pData, int DataSize) {
    sort(pData, pData + DataSize);
}

// Функція для форматованого виводу даних
void PrintData(double *pData, int DataSize) {
    for(int i = 0; i < DataSize; i++)
        printf("%7.4f ", pData[i]);
    printf("\n");
}
