# DC Lab 1 - Parallel Matrix-Vector Multiplication

**Лабораторна робота з паралельних обчислень**  
*Розробка послідовного та паралельного алгоритмів множення матриці на вектор*

## Мета роботи

- Реалізація послідовного алгоритму множення матриці на вектор
- Розробка паралельної версії з використанням MPI
- Порівняння продуктивності послідовного та паралельного підходів
- Дослідження масштабованості паралельного алгоритму

## Технології

- **Мова програмування**: C++
- **Паралельні обчислення**: MPI (Message Passing Interface)
- **Компілятори**: g++, mpic++
- **ОС**: Alpine Linux / WSL
- 
# Послідовна версія

for i = 0 to Size-1:
    result[i] = 0
    for j = 0 to Size-1:
        result[i] += matrix[i][j] * vector[j]
# Паралельна версія
Розподіл рядків матриці між процесами
Локальне обчислення частин результату
Збір результатів з усіх процесів

### Компіляція і запуск:
## Послідовна версія
#g++ -o SerialMV SerialMV.cpp
#./SerialMV

## Паралельна версія
# Компіляція
mpic++ -o ParallelMV ParallelMV.cpp
# Запуск
mpiexec --allow-run-as-root -n 2 ./ParallelMV
mpiexec --allow-run-as-root --oversubscribe -n
 8 ./ParallelMV запуск на моєму компютері якщо 4 8 процесів бо не виитягує

### Корисні команди
# Компіляція послідовної програми
g++ -o SerialMV SerialMV.cpp

# Запуск послідовної програми
./SerialMV

# Компіляція паралельної програми
mpic++ -o ParallelMV ParallelMV.cpp

# Запуск паралельної програми на 4 процесах
mpiexec --allow-run-as-root --oversubscribe -n 4 ./ParallelMV

# Запуск на 2 процесах
mpiexec --allow-run-as-root --oversubscribe -n 2 ./ParallelMV

# Перехід в директорію
cd ~/lab_DC

# Перегляд вмісту директорії
ls
