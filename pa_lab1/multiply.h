#include "matrix.h"
#include <chrono>
#include <functional>
#include <future>
#include <thread>
#include <algorithm>

// Однопоточное базовое умножение матриц
Matrix basicMultiply(const Matrix& A, const Matrix& B);
 
// Многопоточное умножение через std::thread
Matrix multiplyThreaded(const Matrix& A, const Matrix& B, unsigned num_threads);

// Многопоточное умножение через std::async
Matrix multiplyAsync(const Matrix& A, const Matrix& B, unsigned num_threads);

// Вспомогательная функция: умножение блока матрицы A (i0..i1, k0..k1)
// и блока матрицы B (k0..k1, j0..j1), результат добавляется в C (i0..i1, j0..j1)
void multiplyBlock(const Matrix& A, const Matrix& B, Matrix& C,
                       std::size_t bi, std::size_t bj,
                       std::size_t blocks_i, std::size_t blocks_j);