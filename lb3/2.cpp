#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include <memory>
#include <tuple>

const int INF = std::numeric_limits<int>::max();

class Matrix {
private:
    std::vector<std::vector<int>> _matrix;
    int _size;

public:
    Matrix(int size) : _size(size), _matrix(size, std::vector<int>(size, INF)) {}

    void setCell(int row, int col, int val) {
        _matrix[row][col] = val;
    }

    int getCell(int row, int col) const {
        return _matrix[row][col];
    }

    int getSize() const {
        return _size;
    }

    void print() const {
        std::cout << "Matrix:\n";
        for (const auto& row : _matrix) {
            for (int val : row) {
                if (val == INF) std::cout << "INF ";
                else std::cout << val << " ";
            }
            std::cout << "\n";
        }
    }

    void reduceMatrix(std::vector<std::vector<int>>& matrix, int& reductionCost) {
        int n = matrix.size();
        reductionCost = 0;
        
        for (int i = 0; i < n; i++) {
            int rowMin = INF;
            for (int j = 0; j < n; j++)
                rowMin = std::min(rowMin, matrix[i][j]);
            
            if (rowMin != INF && rowMin > 0) {
                for (int j = 0; j < n; j++) {
                    if (matrix[i][j] != INF) matrix[i][j] -= rowMin;
                }
                reductionCost += rowMin;
            }
        }

        for (int j = 0; j < n; j++) {
            int colMin = INF;
            for (int i = 0; i < n; i++)
                colMin = std::min(colMin, matrix[i][j]);

            if (colMin != INF && colMin > 0) {
                for (int i = 0; i < n; i++) {
                    if (matrix[i][j] != INF) matrix[i][j] -= colMin;
                }
                reductionCost += colMin;
            }
        }
    }
};

class TSPSolver {
private:
    std::unique_ptr<Matrix> _matrix;
    int _n;
    int _bestCost;
    std::vector<int> _bestPath;

public:
    TSPSolver(const std::vector<std::vector<int>>& costMatrix) {
        _n = costMatrix.size();
        _matrix = std::make_unique<Matrix>(_n);
        _bestCost = INF;

        for (int i = 0; i < _n; i++) {
            for (int j = 0; j < _n; j++) {
                _matrix->setCell(i, j, costMatrix[i][j]);
            }
        }
    }

    void solve() {
        std::vector<std::vector<int>> matrix = _matrix->_matrix;
        int reductionCost;
        _matrix->reduceMatrix(matrix, reductionCost);
        branchAndBound({}, 0, 0, reductionCost, matrix);
    }

    void branchAndBound(std::vector<int> path, int currentCity, int pathCost, int lowerBound, std::vector<std::vector<int>> matrix) {
        path.push_back(currentCity);

        if (path.size() == _n) {
            pathCost += _matrix->getCell(currentCity, path[0]);
            if (pathCost < _bestCost) {
                _bestCost = pathCost;
                _bestPath = path;
            }
            return;
        }

        for (int nextCity = 0; nextCity < _n; nextCity++) {
            if (matrix[currentCity][nextCity] != INF) {
                std::vector<std::vector<int>> newMatrix = matrix;
                for (int j = 0; j < _n; j++) newMatrix[currentCity][j] = INF;
                for (int i = 0; i < _n; i++) newMatrix[i][nextCity] = INF;
                newMatrix[nextCity][currentCity] = INF;
                
                int newReductionCost;
                _matrix->reduceMatrix(newMatrix, newReductionCost);
                int newLowerBound = lowerBound + matrix[currentCity][nextCity] + newReductionCost;
                
                if (newLowerBound < _bestCost) {
                    branchAndBound(path, nextCity, pathCost + matrix[currentCity][nextCity], newLowerBound, newMatrix);
                }
            }
        }
    }

    void printBestPath() {
        if (_bestCost == INF) {
            std::cout << "No solution found\n";
            return;
        }
        std::cout << "Best Path: ";
        for (int city : _bestPath) {
            std::cout << city << " -> ";
        }
        std::cout << _bestPath[0] << "\nBest Cost: " << _bestCost << "\n";
    }
};

int main() {}