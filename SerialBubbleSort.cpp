#include <cstdlib>      // Для rand(), srand(), exit()
#include <cstdio>       // Для printf(), scanf()
#include <cstring>      // Для роботи з рядками (хоча в цьому коді не використовується)
#include <ctime>        // Для time(), clock()
#include <algorithm>    // Для std::sort()

using namespace std;

// --- Глобальні константи ---
const double RandomDataMultiplier = 1000.0;

// --- Прототипи функцій ---
void ProcessInitialization(double *&pData, int& DataSize);
void ProcessTermination(double *pData);
void DummyDataInitialization(double*& pData, int& DataSize);
void RandomDataInitialization(double *&pData, int& DataSize);
void SerialBubble(double *pData, int DataSize);
void SerialStdSort(double *pData, int DataSize);
void PrintData(double *pData, int DataSize);

// --- Головна функція ---
int main(int argc, char *argv[]) {
    double *pData = 0;
    int DataSize = 0;
    time_t start, finish;
    double duration = 0.0;

    printf("Serial bubble sort program\n");

    // Ініціалізація процесу
    ProcessInitialization(pData, DataSize);
    
    // printf("Data before sorting\n");
    // PrintData(pData, DataSize);

    start = clock();
    
    // Послідовне сортування бульбашкою
    // SerialBubble(pData, DataSize);
    
    // Сортування стандартним алгоритмом
    SerialStdSort(pData, DataSize);
    
    finish = clock();

    // printf("Data after sorting\n");
    // PrintData(pData, DataSize);

    duration = (finish - start) / double(CLOCKS_PER_SEC);
    printf("Time of execution: %f\n", duration);

    // Завершення процесу
    ProcessTermination(pData);

    return 0;
}

// --- Реалізації функцій ---

/**
 * @brief Виділення пам'яті та ініціалізація даних.
 */
void ProcessInitialization(double *&pData, int& DataSize) {
    do {
        printf("Enter the size of data to be sorted: ");
        scanf("%d", &DataSize);

        if(DataSize <= 0)
            printf("Data size should be greater than zero\n");
    }
    while(DataSize <= 0);
        printf("Sorting %d data items\n", DataSize);

        pData = new double[DataSize];

        // Просте заповнення даних
        // DummyDataInitialization(pData, DataSize);
        
        // Заповнення даних випадковими числами
        RandomDataInitialization(pData, DataSize);
}

/**
 * @brief Звільнення виділеної пам'яті.
 */
void ProcessTermination(double *pData) {
    delete []pData;
}

/**
 * @brief Заповнення масиву тестовими даними (у зворотному порядку).
 */
void DummyDataInitialization(double*& pData, int& DataSize) {
    for(int i = 0; i < DataSize; i++)
        pData[i] = DataSize - i;
}

/**
 * @brief Заповнення масиву випадковими даними.
 */
void RandomDataInitialization(double *&pData, int& DataSize) {
    srand( (unsigned)time(0) );

    for(int i = 0; i < DataSize; i++)
        pData[i] = double(rand()) / RAND_MAX * RandomDataMultiplier;
}

/**
 * @brief Реалізація послідовного сортування бульбашкою.
 */
void SerialBubble(double *pData, int DataSize) {
    double Tmp;

    for(int i = 1; i < DataSize; i++)
        for(int j = 0; j < DataSize - i; j++)
            if(pData[j] > pData[j + 1]) {
                Tmp = pData[j];
                pData[j] = pData[j + 1];
                pData[j + 1] = Tmp;
            }
}

/**
 * @brief Сортування за допомогою std::sort зі стандартної бібліотеки.
 */
void SerialStdSort(double *pData, int DataSize) {
    sort(pData, pData + DataSize);
}

/**
 * @brief Форматований вивід масиву даних.
 */
void PrintData(double *pData, int DataSize) {
    for(int i = 0; i < DataSize; i++)
        printf("%7.4f ", pData[i]);
    printf("\n");
}
