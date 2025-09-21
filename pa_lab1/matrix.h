#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <stdexcept>
#define RANGE 100.0

class Matrix {
public:
    Matrix(const std::vector<double>& vals, std::size_t n);

    Matrix(std::size_t n);

    std::size_t size() const noexcept { return n_; }

    double& operator()(std::size_t i, std::size_t j);

    double operator()(std::size_t i, std::size_t j) const; 

    void print(std::ostream &os = std::cout, int width = 10, int precision = 3) const;

    bool equals(const Matrix& other) const;

private:
    std::size_t n_;
    std::vector<double> data_;

    void checkIndices(std::size_t i, std::size_t j) const;
};


