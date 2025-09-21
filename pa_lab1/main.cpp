#include "matrix.h"
#include "multiply.h"
#include "utils.h"

// Функция для вычисления среднего времени выполнения
long long calculateAverageTime(const std::vector<long long>& times) {
    long long sum = 0;
    for (auto time : times) {
        sum += time;
    }
    return sum / times.size();
}

int main() {
    std::size_t n = 2000;            // размер матрицы
    unsigned numThreads = 4;      // количество потоков
    const int numRuns = 1;       // количество прогонов для усреднения

    std::cout << "Создаем матрицы " << n << "x" << n << "\n";
    std::cout << "Количество потоков: " << numThreads << "\n";
    std::cout << "Количество прогонов для усреднения: " << numRuns << "\n\n";
    Matrix A(n), B(n);

    // Векторы для хранения времени выполнения
    std::vector<long long> asyncTimes;
    std::vector<long long> threadTimes;

    // Базовое умножение
    long long t_basic = measureTime([&]() { return basicMultiply(A, B); });
    Matrix C_basic = basicMultiply(A, B);
    //C_basic.print();
    std::cout << "basicMultiply время:   " << t_basic/ 1000.0 << " мкс\n\n";

    // Многопоточное через async
    std::cout << "Тестирование multiplyAsync...\n";
    for (int i = 0; i < numRuns; ++i) {
        long long t_async = measureTime([&]() { return multiplyAsync(A, B, numThreads); });
        asyncTimes.push_back(t_async);
    }
    Matrix C_async = multiplyAsync(A, B, numThreads);
    //C_async.print();
    long long avg_async = calculateAverageTime(asyncTimes);
    std::cout << "multiplyAsync среднее время: " << avg_async / 1000.0 << " мкс\n";
    std::cout << "async корректность: "
              << (C_basic.equals(C_async) ? "OK" : "FAIL") << "\n\n";

    // Многопоточное через thread
    std::cout << "Тестирование multiplyThreaded...\n";
    for (int i = 0; i < numRuns; ++i) {
        long long t_thread = measureTime([&]() { return multiplyThreaded(A, B, numThreads); });
        threadTimes.push_back(t_thread);
    }
    Matrix C_thread = multiplyThreaded(A, B, numThreads);
    long long avg_thread = calculateAverageTime(threadTimes);
    std::cout << "multiplyThreaded среднее время: " << avg_thread / 1000.0 << " мкс\n";
    std::cout << "thread корректность: "
              << (C_basic.equals(C_thread) ? "OK" : "FAIL") << "\n";

    return 0;
}
