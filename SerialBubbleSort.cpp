#include <cstdlib>      // Для rand(), srand()
#include <cstdio>       // Для printf(), scanf(), setvbuf()
#include <ctime>        // Для time(), clock()
#include <algorithm>    // Для std::sort, std::copy, std::equal

using namespace std;

// --- Глобальні константи ---
const double RandomDataMultiplier = 1000.0;

// --- Прототипи функцій ---
void ProcessInitialization(double *&pData, int& DataSize);
void ProcessTermination(double *pData);
void DummyDataInitialization(double*& pData, int& DataSize);
void RandomDataInitialization(double *&pData, int& DataSize);
void CopyData(double *pData, int DataSize, double *pDataCopy);
void SerialBubbleSort(double *pData, int DataSize);
void SerialStdSort(double *pData, int DataSize);
void PrintData(double *pData, int DataSize);

// --- Головна функція ---
int main(int argc, char *argv[]) {
    double *pData = 0;
    int DataSize = 0;
    clock_t start, finish; // Використовуємо clock_t для послідовного коду
    double duration = 0.0;

    printf("Serial sort timing program\n");

    // Ініціалізація процесу
    ProcessInitialization(pData, DataSize);

    // --- БЛОК ВИМІРЮВАННЯ ПОСЛІДОВНИХ АЛГОРИТМІВ ---
    printf("\n--- Serial Algorithm Timing (on full data) ---\n");
    double *pTempData = new double[DataSize];

    // 1. Вимірювання часу для SerialBubbleSort
    CopyData(pData, DataSize, pTempData); // Свіжа копія оригінальних даних
    start = clock();
    SerialBubbleSort(pTempData, DataSize);
    finish = clock();
    duration = (finish - start) / (double)CLOCKS_PER_SEC;
    printf("Time for Serial Bubble Sort:  %f\n", duration);

    // 2. Вимірювання часу для SerialStdSort
    CopyData(pData, DataSize, pTempData); // Свіжа копія оригінальних даних
    start = clock();
    SerialStdSort(pTempData, DataSize);
    finish = clock();
    duration = (finish - start) / (double)CLOCKS_PER_SEC;
    printf("Time for Serial Standard Sort: %f\n", duration);

    delete [] pTempData;
    printf("--------------------------------------------\n\n");
    // --- КІНЕЦЬ БЛОКУ ВИМІРЮВАННЯ ---

    // Завершення процесу
    ProcessTermination(pData);

    return 0;
}

// --- Реалізації функцій ---

// Функція для виділення пам'яті та встановлення початкових значень
void ProcessInitialization(double *&pData, int& DataSize) {
    setvbuf(stdout, 0, _IONBF, 0);

    do {
        printf("Enter the size of data to be sorted: ");
        scanf("%d", &DataSize);

        if(DataSize <= 0)
            printf("Data size should be greater than zero\n");
    } while(DataSize <= 0);

    printf("Sorting %d data items\n", DataSize);
    
    pData = new double[DataSize];
    
    // Ініціалізація даних
    //RandomDataInitialization(pData, DataSize);
    DummyDataInitialization(pData, DataSize);
}

// Функція для завершення обчислювального процесу
void ProcessTermination(double *pData) {
    if (pData != 0) {
        delete []pData;
    }
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
