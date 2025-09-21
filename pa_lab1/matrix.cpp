#include "matrix.h"


// Конструктор: из вектора и размера (квадратная матрица n x n)
Matrix::Matrix(const std::vector<double>& vals, std::size_t n) : n_(n), data_(n * n) {
    if (vals.size() != n * n) {
        throw std::invalid_argument("Размер вектора не соответствует n*n");
    }
    data_ = vals;
}

// Конструктор: создаёт случайную матрицу размера n x n
// Вызов Matrix(n) создаёт матрицу с элементами в диапазоне [-RANGE, RANGE]
Matrix::Matrix(std::size_t n) : n_(n), data_(n * n) {
    if (n == 0) throw std::invalid_argument("Размер матрицы должен быть > 0");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-RANGE, RANGE);
    for (auto &x : data_) x = dis(gen);
}

// Доступ к элементам (i,j) — нумерация с 0
double&  Matrix::operator()(std::size_t i, std::size_t j) {
    checkIndices(i, j);
    return data_[i * n_ + j];
}

double Matrix::operator()(std::size_t i, std::size_t j) const {
    checkIndices(i, j);
    return data_[i * n_ + j];
}

// Вывод в консоль
void Matrix::print(std::ostream &os, int width, int precision) const {
    os << std::fixed << std::setprecision(precision);
    for (std::size_t i = 0; i < n_; ++i) {
        for (std::size_t j = 0; j < n_; ++j) {
            os << std::setw(width) << operator()(i, j);
        }
        os << '\n';
    }
}

void Matrix::checkIndices(std::size_t i, std::size_t j) const {
    if (i >= n_ || j >= n_) throw std::out_of_range("Index out of bounds");
}

bool Matrix::equals(const Matrix& other) const {
    if (n_ != other.n_) return false;
    for (int i = 0; i < n_*n_; ++i) {
        if (data_[i] != other.data_[i]){
            return false;
        }
    }
    return true;
}