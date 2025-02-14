#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <limits>
#include <tuple>
#include <chrono>

// Структура для хранения состояния алгоритма
struct State {
    std::vector<std::vector<int>> matrix; // Текущая матрица разбиения
    int count; // Количество использованных квадратов
    std::vector<std::tuple<int, int, int>> squares; // Список размещенных квадратов (x, y, size)
};

// ------------------------------------------------------------------
// 1. Вспомогательные функции
// ------------------------------------------------------------------

// Функция поиска первой свободной ячейки в матрице
std::pair<int, int> findStart(const std::vector<std::vector<int>>& matrix) {
    int n = matrix.size();
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            if (matrix[y][x] == 0) {
                return {y, x}; // Возвращаем координаты первой найденной пустой ячейки
            }
        }
    }
    return {-1, -1}; // Если пустых ячеек нет
}

// Определяет максимальный размер квадрата, который можно разместить в (x, y)
int findLargestPossibleSquare(const std::vector<std::vector<int>>& matrix, int x, int y) {
    int n = matrix.size();
    int size = 1;
    while (x + size < n && y + size < n && matrix[y][x + size] == 0 && matrix[y + size][x] == 0) {
        ++size;
    }
    return size;
}

// Проверка числа на простоту
bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

// Находит первый простой делитель числа
int firstPrimeDivisor(int num) {
    int primes[] = {3, 5, 7, 11, 13, 17, 19, 23};
    for (int p : primes) {
        if (num % p == 0) return p;
    }
    return num;
}

// ------------------------------------------------------------------
// 2. Разбиение доски
// ------------------------------------------------------------------

// Генерирует начальное разбиение доски
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

// Основной алгоритм поиска минимального разбиения на квадраты
std::vector<std::tuple<int, int, int>> findMinSquarePartition(int n) {
    // Минимальное количество квадратов, найденное на данный момент (изначально максимально возможное)
    int minSquares = std::numeric_limits<int>::max();
    std::vector<std::tuple<int, int, int>> bestSolution; // Лучшее разбиение квадрата
    int newSquareID;

    std::stack<State> stack; // Стек для хранения состояний при переборе вариантов
    std::vector<std::vector<int>> initialMatrix(n, std::vector<int>(n, 0)); // Исходная пустая матрица
    std::vector<std::tuple<int, int, int>> initialPartition = startSquares(n); // Начальное разбиение квадрата
    int squareID = 1; // ID первого квадрата

    // Заполняем матрицу начальными квадратами
    for (const auto& partition : initialPartition) {
        int x, y, size;
        std::tie(x, y, size) = partition;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                initialMatrix[y + i][x + j] = squareID; // Заполняем клетки значением squareID
        squareID++; // Увеличиваем ID для следующего квадрата
    }

    // Добавляем начальное состояние в стек
    stack.push({initialMatrix, (int)initialPartition.size(), initialPartition});

    // Основной цикл перебора возможных разбиений
    int iteration = 0;
    while (!stack.empty()) {
        ++iteration;
        State state = stack.top(); // Получаем текущее состояние
        stack.pop(); // Убираем его из стека

        // Вывод текущего состояния
        std::cout << "\nИтерация " << iteration << ":\n";
        std::cout << "Текущее количество квадратов: " << state.count << "\n";
        std::cout << "Лучший результат: " << (minSquares != std::numeric_limits<int>::max() ? std::to_string(minSquares) : "не найден") << "\n";

        // Если текущее разбиение уже хуже найденного лучшего, пропускаем его
        if (state.count >= minSquares) {
            std::cout << "Отбрасываем ветку - результат хуже текущего лучшего\n";
            continue;
        }

        // Ищем первую свободную клетку в текущей матрице
        auto [y, x] = findStart(state.matrix);
        if (y == -1 && x == -1) { // Если пустых клеток нет, значит, матрица полностью заполнена
            std::cout << "!!! Найдено новое решение !!!\n";
            std::cout << "Количество квадратов: " << state.count << " (предыдущий минимум: " << minSquares << ")\n";
            std::cout << "Визуализация заполнения:\n";
            for (const auto& row : state.matrix) {
                for (int cell : row) {
                    std::cout << cell << "\t";
                }
                std::cout << "\n";
            }
            minSquares = state.count; // Обновляем минимальное количество квадратов
            bestSolution = state.squares; // Запоминаем разбиение
            continue;
        }

        // Определяем максимальный возможный размер квадрата, который можно разместить в (x, y)
        int max_size = findLargestPossibleSquare(state.matrix, x, y);
        std::cout << "Пустая клетка: (" << x + 1 << ", " << y + 1 << "), максимальный размер: " << max_size << "\n";

        // Перебираем все возможные размеры квадратов от 1 до max_size
        for (int size = 1; size <= max_size; ++size) {
            std::vector<std::vector<int>> newMatrix = state.matrix; // Копируем текущее состояние матрицы
            newSquareID = state.squares.size() + 1; // ID нового квадрата
            for (int i = 0; i < size; ++i)
                for (int j = 0; j < size; ++j)
                    newMatrix[y + i][x + j] = newSquareID; // Заполняем клетки нового квадрата

            std::vector<std::tuple<int, int, int>> newSquares = state.squares; // Копируем список квадратов
            newSquares.emplace_back(x, y, size); // Добавляем новый квадрат

            // Добавляем новое состояние в стек для дальнейшего рассмотрения
            stack.push({newMatrix, state.count + 1, newSquares});
            std::cout << "Добавлено состояние в стек. Новое количество: " << state.count + 1 << "\n";
        }
    }

    // Вывод итогового результата
    std::cout << "\n=== Итоговый результат ===\n";
    std::cout << "Минимальное количество квадратов: " << minSquares << "\n";
    std::cout << "Расположение квадратов (x, y, size):\n";
    for (const auto& square : bestSolution) {
        std::cout << std::get<0>(square) + 1 << " " << std::get<1>(square) + 1 << " " << std::get<2>(square) << "\n";
    }

    // Визуализация матрицы
    std::cout << "\nВизуализация заполнения:\n";
    for (const auto& row : bestSolution) {
        std::cout << std::get<0>(row) << "\t" << std::get<1>(row) << "\t" << std::get<2>(row) << "\n";
    }
}

int main() {
    int n;
    std::cin >> n;
    findMinSquarePartition(n);
    return 0;
}
