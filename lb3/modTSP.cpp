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

class Matrix
{
private:
    std::vector<std::vector<double>> _matrix; // Двумерный вектор для хранения матрицы стоимостей
    int _size;                             // Размер матрицы (число городов)

public:
    // Конструктор для создания квадратной матрицы заданного размера
    Matrix(int size)
    {
        _size = size;
        _matrix = std::vector<std::vector<double>>(_size, std::vector<double>(_size, 0));
    }

    // Конструктор для инициализации матрицы готовыми значениями
    Matrix(std::vector<std::vector<double>> matrix)
    {
        _size = matrix.size();
        _matrix = matrix;
    }

    // Установка значения в ячейку матрицы
    void setCell(int row, int col, int val)
    {
        _matrix[row][col] = val;
    }

    // Получение значения из ячейки матрицы
    double getCell(int row, int col)
    {
        return _matrix[row][col];
    }

    // Получение размера матрицы
    int getSize()
    {
        return _size;
    }

    // Удаление строки и столбца из матрицы (заменяя их на INF, чтобы они не учитывались)
    void deleteRowCol(int row, int col)
    {
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
    int getMinInRow(int row)
    {
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
    int getMinInCol(int col)
    {
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
    void print()
    {
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
    void generate(bool is_symmetric = false, int min = 1, int max = 100)
    {
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dist(min, max);

        for (int i = 0; i < _size; i++)
        {
            for (int j = 0; j < _size; j++)
            {
                if (i == j)
                {
                    _matrix[i][j] = INF;
                }
                else
                {
                    if (is_symmetric)
                    {
                        if (i < j)
                        {
                            _matrix[i][j] = dist(gen);
                        }
                        else
                        {
                            _matrix[i][j] = _matrix[j][i];
                        }
                    }
                    else
                    {
                        _matrix[i][j] = dist(gen);
                    }
                }
            }
        }
    }
};

// Функция редукции матрицы. Суть редукции:
// - Находим минимальные элементы в строках и вычитаем их из всех элементов строки
// - Аналогично делаем с минимальными элементами столбцов
// - Суммируем все вычтенные минимумы и возвращаем как нижнюю границу (bound)
int reduce(Matrix &matrix)
{
    int bound = 0;

    std::cout << "Исходная матрица перед редукцией:\n";
    matrix.print();

    // Проход по строкам
    for (int i = 0; i < matrix.getSize(); i++)
    {
        int min = matrix.getMinInRow(i);
        if (min == INF)
            continue; // Если строка вся INF, пропускаем её

        std::cout << "Минимальное значение в строке " << i << ": " << min << '\n';

        for (int j = 0; j < matrix.getSize(); j++)
        {
            if (matrix.getCell(i, j) != INF)
            {
                matrix.setCell(i, j, matrix.getCell(i, j) - min);
            }
        }
        bound += min;
    }

    // Проход по столбцам
    for (int j = 0; j < matrix.getSize(); j++)
    {
        int min = matrix.getMinInCol(j);
        if (min == INF)
            continue; // Если столбец весь INF, пропускаем его

        std::cout << "Минимальное значение в столбце " << j << ": " << min << '\n';

        for (int i = 0; i < matrix.getSize(); i++)
        {
            if (matrix.getCell(i, j) != INF)
            {
                matrix.setCell(i, j, matrix.getCell(i, j) - min);
            }
        }
        bound += min;
    }

    std::cout << "Матрица после редукции:\n";
    matrix.print();
    std::cout << "Нижняя граница: " << bound << "\n\n";

    return bound;
}

// Выбор наилучшего ребра (нулевого элемента) на основе штрафов
std::pair<int, int> calcBestArc(Matrix &matrix)
{
    std::vector<std::pair<int, int>> coords;

    for (int i = 0; i < matrix.getSize(); i++)
    {
        for (int j = 0; j < matrix.getSize(); j++)
        {
            if (matrix.getCell(i, j) == 0)
            {
                coords.push_back({i, j});
            }
        }
    }

    int maxdelta = -1;
    std::pair<int, int> bestCoord = {-1, -1};

    for (auto &coord : coords)
    {
        matrix.setCell(coord.first, coord.second, INF);
        int minInRow = matrix.getMinInRow(coord.first);
        int minInCol = matrix.getMinInCol(coord.second);
        if (minInRow == INF)
            minInRow = 0;
        if (minInCol == INF)
            minInCol = 0;
        int delta = minInRow + minInCol;
        matrix.setCell(coord.first, coord.second, 0);
        std::cout << "Для ребра: (" << coord.first << " -> " << coord.second << ") штраф " << delta << "\n";
        if (maxdelta < delta)
        {
            maxdelta = delta;
            bestCoord = {coord.first, coord.second};
        }
    }

    std::cout << "Лучшее ребро: (" << bestCoord.first << " -> " << bestCoord.second << ") с ценой " << maxdelta << "\n";
    return bestCoord;
}

std::pair<int, int> calcForbiddenArc(Matrix &matrix, std::vector<std::pair<int, int>> &path, int from, int to)
{
    int X = from, Y = to;

    // 1. Найти все рёбра, которые ведут к X
    bool cond = true;
    int incoming_to = X;
    while (cond)
    {
        cond = false;
        for (const auto &[u, v] : path)
        {
            if (v == incoming_to)
            { // Если из вершины v есть ребро к X
                cond = true;
                incoming_to = u; // Вершина, откуда приходит ребро к X (W → X)
                break;
            }
        }
    }

    // 2. Найти все рёбра, которые выходят из Y
    cond = true;
    int outgoing_from = Y;
    while (cond)
    {
        cond = false;
        for (const auto &[u, v] : path)
        {
            if (u == outgoing_from)
            { // Если из вершины Y выходит ребро к v
                cond = true;
                outgoing_from = v; // Вершина, в которую ведёт ребро от Y (Y → Z)
                break;
            }
        }
    }

    // Если есть вход в X (W → X) и выход из Y (Y → Z), то запрещаем ребро Z → W
    if (incoming_to != X || outgoing_from != Y)
    {
        return {outgoing_from, incoming_to};
    }

    // Если других связей нет, то просто запрещаем Y → X
    return {Y, X};
}

void calcBnB(Matrix &matrix, int cost, std::vector<std::pair<int, int>> &path,
             std::vector<std::pair<int, int>> &bestPath, int &bestCost)
{
    // Вывод текущего пути
    std::cout << "Текущий путь: ";
    for (const auto &[u, v] : path)
    {
        std::cout << "(" << u << " -> " << v << ") ";
    }
    std::cout << "\nТекущая стоимость: " << cost << "\n";

    // Если найден путь, содержащий (N-1) рёбер (N - размер матрицы), проверяем его стоимость
    if (path.size() == matrix.getSize() - 1)
    {
        std::cout << "Найден возможный путь, проверка стоимости...\n";
        if (cost < bestCost)
        {
            bestCost = cost;
            bestPath = path;
            std::cout << "Новый лучший путь найден! Стоимость: " << bestCost << "\n";
        }
        return;
    }

    // Если текущая стоимость уже больше лучшей найденной, отбрасываем ветку
    if (cost >= bestCost)
    {
        std::cout << "Отбрасываем ветку, так как cost (" << cost << ") >= bestCost (" << bestCost << ")\n";
        return;
    }

    // Применяем редукцию матрицы и добавляем её стоимость к текущей границе
    cost += reduce(matrix);
    std::cout << "После редукции, текущая граница: " << cost << "\n";

    // Выбираем наилучшее ребро для включения в путь
    std::pair<int, int> arc = calcBestArc(matrix);
    std::cout << "Выбрано ребро (" << arc.first << " -> " << arc.second << ")\n";

    int i = arc.first, j = arc.second;
    if (i == -1)
    {
        std::cout << "Не удалось найти подходящее ребро\n";
        return;
    }
    // Клонируем матрицу для левой ветки (принимаем ребро в путь)
    Matrix newMatrix = matrix;
    std::pair<int, int> forb_arc = calcForbiddenArc(newMatrix, path, i, j);
    std::cout << "Запрещаем обратное ребро (" << forb_arc.first << " -> " << forb_arc.second << ")\n";

    // Добавляем выбранное ребро в путь
    path.push_back(arc);
    newMatrix.setCell(forb_arc.first, forb_arc.second, INF);
    newMatrix.deleteRowCol(i, j);
    std::cout << "Переход в левую ветку (включаем ребро " << i << " -> " << j << ")\n";

    // Рекурсивно вызываем BnB для левой ветки
    calcBnB(newMatrix, cost, path, bestPath, bestCost);

    // Возвращаемся, убираем добавленное ребро и переходим в правую ветку
    path.pop_back();
    matrix.setCell(i, j, INF);
    std::cout << "Переход в правую ветку (исключаем ребро " << i << " -> " << j << ")\n";

    // Рекурсивно вызываем BnB для правой ветки
    calcBnB(matrix, cost, path, bestPath, bestCost);
}

// Восстановление пути из списка рёбер
std::vector<int> formWay(std::vector<std::pair<int, int>> &arcs, int start_vertex)
{
    std::vector<int> resR, resL;
    int vertex = start_vertex;
    resR.push_back(vertex);

    // Двигаемся вперёд по рёбрам
    bool cond = true;
    while (cond)
    {
        cond = false;
        for (const auto &[u, v] : arcs)
        {
            if (u == vertex)
            {
                cond = true;
                vertex = v;
                resR.push_back(vertex);
                break;
            }
        }
    }

    // Двигаемся назад по рёбрам
    vertex = start_vertex;
    cond = true;
    while (cond)
    {
        cond = false;
        for (const auto &[u, v] : arcs)
        {
            if (v == vertex)
            {
                cond = true;
                vertex = u;
                resL.insert(resL.begin(), vertex);
                break;
            }
        }
    }

    // Объединяем левую и правую части пути
    std::vector<int> res;
    res.insert(res.end(), resL.begin(), resL.end());
    res.insert(res.end(), resR.begin(), resR.end());

    return res;
}

std::vector<std::pair<int, int>> primMST(Matrix& matrix) {
    int n = matrix.getSize();
    std::vector<double> minWeight(n, INF);  // Массив для минимальных весов рёбер
    std::vector<int> parent(n, -1);  // Массив для родителей вершин в дереве
    std::vector<bool> inMST(n, false);  // Массив для проверки, в MST ли вершина

    minWeight[0] = 0;  // Начинаем с первой вершины
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;  // Мин-куча (вес, вершина)
    pq.push({0.0, 0});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;

        // Проходим по всем соседям вершины u
        for (int v = 0; v < n; ++v) {
            double weight = matrix.getCell(u, v);
            if (weight != INF && !inMST[v] && weight < minWeight[v]) {
                minWeight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }

    std::vector<std::pair<int, int>> mstEdges;
    for (int v = 1; v < n; ++v) {
        if (parent[v] != -1) {
            mstEdges.push_back({parent[v], v});
        }
    }

    return mstEdges;
}


std::vector<int> dfsMST(const std::vector<std::vector<std::pair<int, double>>>& mstAdj, int start_vertex) {
    std::vector<int> path;
    std::vector<bool> visited(mstAdj.size(), false);
    
    // Стек для обхода в глубину
    std::vector<int> stack;
    stack.push_back(start_vertex);

    while (!stack.empty()) {
        int node = stack.back();
        stack.pop_back();

        if (visited[node]) continue;

        visited[node] = true;
        path.push_back(node);
        //std::cout << node << " |";

        // Сортируем соседей по весу рёбер (это важно для TSP)
        std::vector<std::pair<int, double>> neighbors = mstAdj[node];
        std::sort(neighbors.begin(), neighbors.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second >= b.second;  // Сортируем по весу рёбер
        });


        // Добавляем соседей в стек
        for (const auto& [v, _] : neighbors) {
            if (!visited[v]) {
                stack.push_back(v);
            }
        }
    }

    return path;
}


std::pair<double, std::vector<int>> approximateTSP(Matrix& matrix, int start_vertex) {
    int n = matrix.getSize();

    // 1. Строим минимальное остовное дерево (MST)
    std::vector<std::pair<int, int>> mstEdges = primMST(matrix);
    std::vector<std::vector<std::pair<int, double>>> mstAdj(n); // <сосед, вес>

    // 2. Создаём список смежности для MST
    for (const auto& [u, v] : mstEdges) {
        double weight = matrix.getCell(u, v);
        mstAdj[u].push_back({v, weight});
        mstAdj[v].push_back({u, weight});  // Граф неориентированный
    }

    // for (auto &i: mstAdj){
    //     for (auto &[a, b]: i) {
    //         std::cout << a << " weight: " << b << '\n';
    //     }
    //     std::cout << '\n';
    // }

    // 3. Обходим MST с использованием DFS
    std::vector<int> path = dfsMST(mstAdj, start_vertex);

    // 4. Замыкаем путь
    double total_cost = 0;
    if (!path.empty()) {
        path.push_back(start_vertex);  // Замкнуть цикл
    }

    // 5. Подсчёт стоимости маршрута
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i];
        int v = path[i + 1];
        double edge_cost = matrix.getCell(u, v);
        // std::cout << "COST " << edge_cost << "\n";
        
        // Проверяем, существует ли ребро (если нет, путь невозможен)
        if (edge_cost == INF) {
            return {INF, {}};  // Путь невозможен
        }

        total_cost += edge_cost;
    }

    return {total_cost, path};
}


int main() {
    int start_vertex;
    std::cin >> start_vertex; // Считываем стартовую вершину

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

    std::vector<std::pair<int, int>> path = {};
    std::vector<std::pair<int, int>> bestPath = {};
    int bestCost = INF;
    Matrix matrix(data);
    //calcBnB(matrix, 0, path, bestPath, bestCost);
    auto result = approximateTSP(matrix, start_vertex);
    
    if (result.first == INF) {
        //std::cout << "No solution\n";
    } else {
        //std::cout << "Cost: " << result.first << "\nPath: ";
        std::cout << (double)result.first <<"\n";
        for (int v : result.second) {
            std::cout << v << " ";
        }
    }
    
    return 0;
    
}
