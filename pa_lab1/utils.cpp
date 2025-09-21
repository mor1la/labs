#include "utils.h"

// Функция для измерения времени любой функции умножения матриц
long long measureTime(std::function<Matrix()> f) {
    auto t0 = std::chrono::high_resolution_clock::now();
    volatile auto M = f(); (void)M;
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

