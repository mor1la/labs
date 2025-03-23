#include <limits>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <climits>
#include <stack>
#include <algorithm>
#include <map>

const int INF = std::numeric_limits<int>::max(); // Константа, обозначающая бесконечность (используется для исключения путей)

class Matrix {
private:
    std::vector<std::vector<double>> _matrix; // Двумерный вектор для хранения матрицы стоимостей
    int _size;                             // Размер матрицы (число городов)

public:
    // Конструктор для создания квадратной матрицы заданного размера
    Matrix(int size) {
        _size = size;
        _matrix = std::vector<std::vector<double>>(_size, std::vector<double>(_size, 0));
    }

    // Конструктор для инициализации матрицы готовыми значениями
    Matrix(std::vector<std::vector<double>> matrix) {
        _size = matrix.size();
        _matrix = matrix;
    }

    Matrix() {};

    // Установка значения в ячейку матрицы
    void setCell(int row, int col, int val) {
        _matrix[row][col] = val;
    }

    // Получение значения из ячейки матрицы
    double getCell(int row, int col) {
        return _matrix[row][col];
    }

    // Получение размера матрицы
    int getSize() {
        return _size;
    }

    // Удаление строки и столбца из матрицы (заменяя их на INF, чтобы они не учитывались)
    void deleteRowCol(int row, int col) {
        for (int i = 0; i < _size; i++)
        {
            for (int j = 0; j < _size; j++)
            {
                if (i == row || j == col)
                {
                    setCell(i, j, INF);
                }
            }
        }
    }

    // Нахождение минимального элемента в строке
    int getMinInRow(int row) {
        int min = INF;
        for (int j = 0; j < _size; j++)
        {
            int curr = _matrix[row][j];
            if (curr < min)
            {
                min = curr;
            }
        }
        return min;
    }

    // Нахождение минимального элемента в столбце
    int getMinInCol(int col) {
        int min = INF;
        for (int i = 0; i < _size; i++)
        {
            int curr = _matrix[i][col];
            if (curr < min)
            {
                min = curr;
            }
        }
        return min;
    }

    // Вывод матрицы в консоль для отладки
    void print() {
        std::cout << std::string(50, '-') << '\n';
        for (int i = 0; i < _size; i++)
        {
            for (int j = 0; j < _size; j++)
            {
                _matrix[i][j] == INF ? std::cout << "-1" : std::cout << _matrix[i][j];
                j == _size - 1 ? std::cout << '\n' : std::cout << ' ';
            }
        }
        std::cout << std::string(50, '-') << '\n';
    }

    // Генерация матрицы
    void generate(bool is_symmetric = false, int min = 1, int max = 100) {
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dist(min, max);

        for (int i = 0; i < _size; i++) {
            for (int j = 0; j < _size; j++) {
                if (i == j) {
                    _matrix[i][j] = INF;
                }
                else {
                    if (is_symmetric) {
                        if (i < j) {
                            _matrix[i][j] = dist(gen);
                        }
                        else {
                            _matrix[i][j] = _matrix[j][i];
                        }
                    }
                    else {
                        _matrix[i][j] = dist(gen);
                    }
                }
            }
        }
    }

    void readMatrixFromConsole() {
        std::vector<std::vector<double>> data; // Матрица весов
        std::string line;
        int n = 0; // Определяем размерность матрицы
    
        // Пропускаем символ новой строки после числа
        std::cin.ignore();
    
        // Читаем строки, пока есть данные
        while (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::vector<double> row;
            double value;
    
            while (ss >> value) {
                row.push_back(value);
            }
    
            if (row.empty()) break; // Если пустая строка, завершаем чтение
    
            data.push_back(row);
        }
    
        n = data.size(); // Определяем размерность матрицы
        
        // Заменяем -1 на INF (обозначение отсутствия пути)
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (data[i][j] == -1) {
                    data[i][j] = INF;
                }
            }
        }
        _matrix = data;
        _size = data.size();
    }

    //сохранить в файл 
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (size_t i = 0; i < _size; i++) {
                for (size_t j = 0; j < _size; j++) {
                    file << _matrix[i][j] << " ";
                }
                file << '\n';
            }
        }
        file.close();
    }

    //считать с файла
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::vector<double> row;
        _matrix.clear();
        std::string line;
        double val;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            row.clear();
            while (iss >> val) {
                row.push_back(val);
            }
            _matrix.push_back(row);
        }
        _size = _matrix.size();
    }
};


class ModTSP {
public:
std::vector<std::pair<int, int>> primMST(Matrix& matrix) {
    int n = matrix.getSize();
    std::vector<double> minWeight(n, INF);  // Минимальные веса рёбер
    std::vector<int> parent(n, -1);  // Родитель каждой вершины в MST
    std::vector<bool> inMST(n, false);  // Включена ли вершина в MST

    minWeight[0] = 0;  // Начинаем с первой вершины
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;
    pq.push({0.0, 0});

    std::cout << "Построение минимального остовного дерева (MST) алгоритмом Прима:\n";

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;
        std::cout << "Добавлена вершина " << u << " в MST.\n";

        // Проход по соседям вершины u
        for (int v = 0; v < n; ++v) {
            double weight = matrix.getCell(u, v);
            if (weight != INF && !inMST[v] && weight < minWeight[v]) {
                minWeight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
                std::cout << "Рассмотрено ребро (" << u << " -> " << v << "), вес: " << weight << "\n";
            }
        }
    }

    std::vector<std::pair<int, int>> mstEdges;
    std::cout << "Готовый список рёбер MST:\n";
    for (int v = 1; v < n; ++v) {
        if (parent[v] != -1) {
            mstEdges.push_back({parent[v], v});
            std::cout << "(" << parent[v] << " -> " << v << ")\n";
        }
    }

    return mstEdges;
}

// Функция выполняет обход минимального остовного дерева (MST) в глубину (DFS)
// и возвращает путь обхода в виде списка вершин.
std::vector<int> dfsMST(const std::vector<std::vector<std::pair<int, double>>>& mstAdj, int start_vertex) {
    std::vector<int> path;               // Хранит порядок посещения вершин
    std::vector<bool> visited(mstAdj.size(), false); // Вектор для отслеживания посещённых вершин
    std::vector<int> stack;              // Имитация стека для DFS (глубинного обхода)

    stack.push_back(start_vertex); // Начинаем обход с заданной вершины
    std::cout << "Обход MST в глубину (DFS), начиная с вершины " << start_vertex << ":\n";

    // Пока стек не пуст, продолжаем обход
    while (!stack.empty()) {
        int node = stack.back(); // Берём верхний элемент стека
        stack.pop_back();        // Удаляем его из стека

        if (visited[node]) continue; // Если вершина уже посещена, пропускаем её

        visited[node] = true;  // Отмечаем вершину как посещённую
        path.push_back(node);  // Добавляем её в путь обхода
        std::cout << "Посещена вершина " << node << "\n";

        // Получаем список соседей текущей вершины
        std::vector<std::pair<int, double>> neighbors = mstAdj[node];

        // Сортируем соседей по весу рёбер (чтобы посещать в порядке увеличения веса)
        std::sort(neighbors.begin(), neighbors.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });

        // Добавляем в стек соседей, которых ещё не посещали
        for (const auto& [v, _] : neighbors) {
            if (!visited[v]) {
                stack.push_back(v);
                std::cout << "Добавлен в стек сосед " << v << "\n";
            }
        }
    }

    return path; // Возвращаем список вершин в порядке их посещения
}


// Функция находит приближённое решение задачи коммивояжёра (TSP) 
// с помощью минимального остовного дерева (MST).
// Используется метод Прима для построения MST, затем выполняется DFS-обход.
std::pair<double, std::vector<int>> approximateTSP(Matrix& matrix, int start_vertex) {
    int n = matrix.getSize(); // Получаем размер матрицы (количество вершин в графе)
    std::cout << "\n==== Запуск приближённого решения TSP через MST ====\n";

    // 1. Строим минимальное остовное дерево (MST) с помощью алгоритма Прима.
    // Возвращается список рёбер остовного дерева.
    std::vector<std::pair<int, int>> mstEdges = primMST(matrix);
    
    // 2. Создаём список смежности для MST
    std::vector<std::vector<std::pair<int, double>>> mstAdj(n);
    for (const auto& [u, v] : mstEdges) {
        double weight = matrix.getCell(u, v); // Получаем вес ребра
        mstAdj[u].push_back({v, weight}); // Добавляем в список смежности
        mstAdj[v].push_back({u, weight}); // Граф неориентированный, добавляем обратное ребро
    }

    // 3. Выполняем обход MST в глубину (DFS), начиная с указанной стартовой вершины.
    std::vector<int> path = dfsMST(mstAdj, start_vertex);

    // 4. Замыкаем цикл, возвращаясь к стартовой вершине
    double total_cost = 0;
    if (!path.empty()) {
        path.push_back(start_vertex); // Добавляем стартовую вершину в конец пути
        std::cout << "Замыкаем путь: возвращаемся к стартовой вершине " << start_vertex << "\n";
    }

    // 5. Вычисляем суммарную стоимость маршрута
    std::cout << "Рассчитанный путь TSP:\n";
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i], v = path[i + 1];  // Берём текущую вершину и следующую в пути
        double edge_cost = matrix.getCell(u, v); // Получаем вес ребра между ними

        // Если отсутствует ребро между вершинами, путь невозможен
        if (edge_cost == INF) {
            std::cout << "Ошибка: отсутствует ребро (" << u << " -> " << v << "), путь невозможен.\n";
            return {INF, {}}; // Возвращаем бесконечность и пустой путь
        }

        total_cost += edge_cost; // Прибавляем стоимость ребра к общей стоимости маршрута
        std::cout << "(" << u << " -> " << v << "), вес: " << edge_cost << "\n";
    }

    std::cout << "Общая стоимость маршрута: " << total_cost << "\n";
    return {total_cost, path}; // Возвращаем суммарную стоимость маршрута и сам путь
}
};


int main() {
    int start_vertex;
    std::cin >> start_vertex; // Считываем стартовую вершину
    Matrix matrix;
    matrix.readMatrixFromConsole();
    std::vector<std::pair<int, int>> path = {};
    std::vector<std::pair<int, int>> bestPath = {};
    int bestCost = INF;
    ModTSP modTsp;
    auto result = modTsp.approximateTSP(matrix, start_vertex);
    if (result.first == INF) {
        std::cout << "No solution\n";
    } else {
        std::cout << "Cost: " << result.first << "\nPath: ";
        std::cout << (double)result.first <<"\n";
        for (int v : result.second) {
            std::cout << v << " ";
        }
    }
    
    return 0;
    
}
