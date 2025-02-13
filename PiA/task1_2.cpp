#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <limits>
#include <tuple>
#include <chrono>

struct State {
    std::vector<std::vector<int>> matrix;
    int count;
    std::vector<std::tuple<int, int, int>> squares;
};

std::pair<int, int> findStart(const std::vector<std::vector<int>>& matrix) {
    int n = matrix.size();
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            if (matrix[y][x] == 0) {
                return {y, x};
            }
        }
    }
    return {-1, -1};
}

int findLargestPossibleSquare(const std::vector<std::vector<int>>& matrix, int x, int y) {
    int n = matrix.size();
    int size = 1;
    while (x + size < n && y + size < n && matrix[y][x + size] == 0 && matrix[y + size][x] == 0) {
        ++size;
    }
    return size;
}

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int firstPrimeDivisor(int num) {
    int primes[] = {3, 5, 7, 11, 13, 17, 19, 23};
    for (int p : primes) {
        if (num % p == 0) return p;
    }
    return num;
}

std::vector<std::tuple<int, int, int>> startSquares(int size) {
    std::vector<std::tuple<int, int, int>> partition;
    if (size % 2 == 0) {
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                partition.emplace_back(i * (size / 2), j * (size / 2), size / 2);
    } else if (isPrime(size)) {
        int half = size / 2;
        partition.emplace_back(0, 0, half + 1);
        partition.emplace_back(half + 1, 0, half);
        partition.emplace_back(0, half + 1, half);
    } else {
        int p = firstPrimeDivisor(size);
        partition.emplace_back(0, 0, size / p * 2);
        partition.emplace_back(size / p * 2, size / p, size / p);
        partition.emplace_back(size / p, size / p * 2, size / p);
    }
    return partition;
}

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << "\n";
    }
}

std::vector<std::tuple<int, int, int>> findMinSquarePartition(int n) {
    int minSquares = std::numeric_limits<int>::max();
    std::vector<std::tuple<int, int, int>> bestSolution;

    std::stack<State> stack;
    std::vector<std::vector<int>> initialMatrix(n, std::vector<int>(n, 0));
    std::vector<std::tuple<int, int, int>> initialPartition =  startSquares(n);
    int squareID = 1;

    for (const std::tuple<int, int, int>& partition : initialPartition) {
        int x, y, size;
        std::tie(x, y, size) = partition;  
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                initialMatrix[y + i][x + j] = squareID;
        squareID++;
    }

    stack.push({initialMatrix, (int)initialPartition.size(), initialPartition});

    while (!stack.empty()) {
        State state = stack.top();
        stack.pop();

        if (state.count >= minSquares) continue;

        auto [y, x] = findStart(state.matrix);
        if (y == -1 && x == -1) {
            if (state.count < minSquares) {
                minSquares = state.count;
                bestSolution = state.squares;
                initialMatrix = state.matrix;
            }
            continue;
        }

        int max_size = findLargestPossibleSquare(state.matrix, x, y);

        for (int size = 1; size <= max_size; ++size) {
            std::vector<std::vector<int>> newMatrix = state.matrix;
            int newSquareID = state.squares.size() + 1;
            for (int i = 0; i < size; ++i)
                for (int j = 0; j < size; ++j)
                    newMatrix[y + i][x + j] = newSquareID;
            
            std::vector<std::tuple<int, int, int>> newSquares = state.squares;
            newSquares.emplace_back(x, y, size);
            
            stack.push({newMatrix, state.count + 1, newSquares});
        }
    }

    std::cout << minSquares << "\n";
    for (const std::tuple<int, int, int>& solution : bestSolution) {
        int x, y, size;
        std::tie(x, y, size) = solution; 
        std::cout << x + 1 << " " << y + 1 << " " << size << "\n";
    }
    printMatrix(initialMatrix);
    return bestSolution;
}

// void benchmarkPartition(int maxSize) {
//     std::cout << "Size\tEven (ms)\tOdd (ms)\n";

//     for (int n = 2; n <= maxSize; n += 2) {
//         // Измеряем время для четного n
//         auto start_even = std::chrono::high_resolution_clock::now();
//         findMinSquarePartition(n);
//         auto end_even = std::chrono::high_resolution_clock::now();
//         double time_even = std::chrono::duration<double, std::milli>(end_even - start_even).count();

//         // Измеряем время для нечетного n (n+1)
//         auto start_odd = std::chrono::high_resolution_clock::now();
//         findMinSquarePartition(n + 1);
//         auto end_odd = std::chrono::high_resolution_clock::now();
//         double time_odd = std::chrono::duration<double, std::milli>(end_odd - start_odd).count();

//         std::cout << n << "\t" << time_even << " ms\t" << time_odd << " ms\n";
//     }
// }

int main() {
    int n;
    //int maxSize = 20;
    std::cin >> n;
    findMinSquarePartition(n);
    //benchmarkPartition(maxSize);
    return 0;
}
