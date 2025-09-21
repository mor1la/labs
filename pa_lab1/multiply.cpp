#include "multiply.h"

// Однопоточное базовое умножение матриц
Matrix basicMultiply(const Matrix& A, const Matrix& B) {
    std::size_t n = A.size();
    std::vector<double> result(n * n, 0.0);

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (std::size_t k = 0; k < n; ++k) {
                sum += A(i, k) * B(k, j);
            }
            result[i * n + j] = sum;
        }
    }

    return Matrix(result, n);
}

// Вспомогательная функция: блочное умножение
void multiplyBlock(const Matrix& A, const Matrix& B, Matrix& C,
                       std::size_t bi, std::size_t bj,
                       std::size_t blocks_i, std::size_t blocks_j) 
{
    std::size_t n = A.size();
    std::size_t block_size_i = (n + blocks_i - 1) / blocks_i;
    std::size_t block_size_j = (n + blocks_j - 1) / blocks_j;

    std::size_t i0 = bi * block_size_i;
    std::size_t j0 = bj * block_size_j;
    std::size_t i1 = std::min(i0 + block_size_i, n);
    std::size_t j1 = std::min(j0 + block_size_j, n);

    if (i0 >= n || j0 >= n) return;

    for (std::size_t i = i0; i < i1; ++i) {
        for (std::size_t j = j0; j < j1; ++j) {
            double sum = 0.0;
            for (std::size_t k = 0; k < n; ++k)
                sum += A(i, k) * B(k, j);
            C(i, j) = sum;
        }
    }
}

// Многопоточное умножение через std::thread
Matrix multiplyThreaded(const Matrix &A, const Matrix &B, unsigned numThreads) {
    std::size_t n = A.size();
    Matrix C(std::vector<double>(n * n, 0.0), n);

    numThreads = std::min<unsigned>(numThreads, n);

    std::size_t blocks_i = numThreads;
    std::size_t blocks_j = 1;
    for (std::size_t i = static_cast<std::size_t>(std::sqrt(numThreads)); i >= 1; --i) {
        if (numThreads % i == 0) {
            blocks_i = i;
            blocks_j = numThreads / i;
            break;
        }
    }

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (std::size_t bi = 0; bi < blocks_i; ++bi) {
        for (std::size_t bj = 0; bj < blocks_j; ++bj) {
            threads.emplace_back([&A, &B, &C, bi, bj, blocks_i, blocks_j]() {
                multiplyBlock(A, B, C, bi, bj, blocks_i, blocks_j);
            });
        }
    }

    for (auto &th : threads)
        if (th.joinable()) th.join();

    return C;
}

// Многопоточное умножение через std::async
Matrix multiplyAsync(const Matrix &A, const Matrix &B, unsigned numThreads) {
    std::size_t n = A.size();
    Matrix C(std::vector<double>(n * n, 0.0), n);

    numThreads = std::min<unsigned>(numThreads, n);

    std::size_t blocks_i = numThreads;
    std::size_t blocks_j = 1;
    for (std::size_t i = static_cast<std::size_t>(std::sqrt(numThreads)); i >= 1; --i) {
        if (numThreads % i == 0) {
            blocks_i = i;
            blocks_j = numThreads / i;
            break;
        }
    }

    std::vector<std::future<void>> futures;
    futures.reserve(numThreads);

    for (std::size_t bi = 0; bi < blocks_i; ++bi) {
        for (std::size_t bj = 0; bj < blocks_j; ++bj) {
            futures.push_back(std::async(std::launch::async, [&A, &B, &C, bi, bj, blocks_i, blocks_j]() {
                multiplyBlock(A, B, C, bi, bj, blocks_i, blocks_j);
            }));
        }
    }

    for (auto &f : futures) f.get();
    return C;
}
