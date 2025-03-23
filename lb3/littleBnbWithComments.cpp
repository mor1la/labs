#include <limits> 
#include <vector> 
#include <chrono> 
#include <random> 
#include <fstream> 
#include <sstream> 
#include <iostream> 
#include <algorithm> 
#include <unordered_map> 
#include <set> 
#include <iomanip>
#include <unordered_set>
#include <map>


const int INF = std::numeric_limits<int>::max(); 

class Matrix {
public:
    std::vector<std::vector<int>> _matrix; // Двумерный вектор для хранения матрицы
    int _size; // Размер матрицы (количество строк и столбцов)

public:
    // Конструктор, инициализирующий матрицу нулями заданного размера
    Matrix(int size) {
        _size = size;
        _matrix = std::vector<std::vector<int>>(_size, std::vector<int>(_size, 0));
    }

    // Конструктор, инициализирующий матрицу заданными данными
    Matrix(std::vector<std::vector<int>> matrix) {
        _size = matrix.size();  
        _matrix = matrix; 
    }

    Matrix() {};

    // Устанавливает значение val в ячейку (row, col)
    void setCell(int row, int col, int val) {
        _matrix[row][col] = val;
    }

    // Возвращает значение в ячейке (row, col)
    int getCell(int row, int col) {
        return _matrix[row][col];
    }

    // Возвращает размер матрицы
    int getSize() {
        return _size;
    }

    // Удаляет строку и столбец, заменяя их значениями INF
    void deleteRowCol(int row, int col) { 
        for (int i = 0; i < _size; i++) { 
            for (int j = 0; j < _size; j++) { 
                if (i == row || j == col) { 
                    setCell(i, j, INF); 
                } 
            } 
        } 
    } 

    // Возвращает минимальное значение в строке row
    int getMinInRow(int row) {
        int min = INF;
        for (int j = 0; j < _size; j++) {
            int curr = _matrix[row][j];
            if (curr < min) {
                min = curr;
            }
        }
        return min;
    }

    // Возвращает минимальное значение в столбце col
    int getMinInCol(int col) {
        int min = INF;
        for (int i = 0; i < _size; i++) {
            int curr = _matrix[i][col];
            if (curr < min) {
                min = curr;
            }
        }
        return min;
    }

    // Возвращает всю матрицу в виде вектора
    std::vector<std::vector<int>> getMatrix(){
        return _matrix;
    } 

    // Выводит матрицу в консоль
    void print() { 
        std::cout << std::string(50, '-') << '\n'; 
        for (int i = 0; i < _size; i++) { 
            for (int j = 0; j < _size; j++) { 
                // Если значение INF, выводим -1
                _matrix[i][j] == INF ? std::cout << "-1" : std::cout << _matrix[i][j]; 
                j == _size - 1 ? std::cout << '\n' : std::cout << ' '; 
            } 
        } 
        std::cout << std::string(50, '-') << '\n'; 
    } 

    // Генерирует случайную матрицу, может быть симметричной
    void generate(bool is_symmetric = false, int min = 1, int max = 100) { 
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count(); 
        std::mt19937 gen(seed); // Генератор случайных чисел
        std::uniform_int_distribution<> dist(min, max); // Распределение чисел в заданном диапазоне

        for (int i = 0; i < _size; i++) { 
            for (int j = 0; j < _size; j++) { 
                if (i == j) { 
                    _matrix[i][j] = INF; // Заполняем диагональ значением INF
                } else { 
                    if (is_symmetric) { 
                        if (i < j) { 
                            _matrix[i][j] = dist(gen); // Заполняем только верхний треугольник
                        } else { 
                            _matrix[i][j] = _matrix[j][i]; // Делаем матрицу симметричной
                        } 
                    } else { 
                        _matrix[i][j] = dist(gen); // Заполняем случайными числами
                    } 
                } 
            } 
        } 
    }
    
    //сохранить в файл
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (int i = 0; i < _size; i++) {
                for (int j = 0; j < _size; j++) {
                    file << _matrix[i][j] << " ";
                }
                file << '\n';
            }
        }
        file.close();
    }

    //загрузить матрицу из файла
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        std::vector<int> row;
        _matrix.clear();
        int val;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            row.clear();
            while (iss >> val) {
                row.push_back(val);
            }
            _matrix.push_back(row);
        }
        _size = row.size();
    }

    void readMatrixFromConsole() {
        int n;
        std::cin >> n;
        std::vector<std::vector<int>> data(n, std::vector<int>(n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cin >> data[i][j];
                if (i == j){
                    data[i][j] = INF;
                }
            }
        }
        _matrix = data;
        _size = data.size();
    }
};

class BranchAndBound {
public:
int reduce(Matrix& matrix) {
    int bound = 0;
    int n = matrix.getSize();
    
    std::cout << "\nРедукция матрицы:" << std::endl;
    std::cout << "Матрица до редукции:" << std::endl;
    matrix.print();

    // Вычитание минимального значения из каждой строки
    for (int i = 0; i < n; i++) {
        int min = matrix.getMinInRow(i);
        if (min > 0 && min != INF) {
            for (int j = 0; j < n; j++) {
                if (matrix.getCell(i, j) != INF) {
                    matrix.setCell(i, j, matrix.getCell(i, j) - min);
                }
            }
            std::cout << "Из строки " << i << " вычли " << min << std::endl;
            bound += min;
        } else {
            std::cout << "Из строки " << i << " ничего не вычли" << std::endl;
        }
    }

    // Вычитание минимального значения из каждого столбца
    for (int j = 0; j < n; j++) {
        int min = matrix.getMinInCol(j);
        if (min > 0 && min != INF) {
            for (int i = 0; i < n; i++) {
                if (matrix.getCell(i, j) != INF) {
                    matrix.setCell(i, j, matrix.getCell(i, j) - min);
                }
            }
            std::cout << "Из столбца " << j << " вычли " << min << std::endl;
            bound += min;
        } else {
            std::cout << "Из столбца " << j << " ничего не вычли" << std::endl;
        }
    }

    std::cout << "Матрица после редукции:" << std::endl;
    matrix.print();
    std::cout << "Общее сокращение: " << bound << "\n" << std::endl;
    
    return bound;
}

std::pair<int, int> calcBestArc(Matrix& matrix) {
    std::vector<std::pair<int, int>> coords;
    std::cout << "\nПоиск лучшей дуги (ребра):" << std::endl;

    // Находим все нулевые элементы в матрице (кандидаты на выбор ребра)
    for (int i = 0; i < matrix.getSize(); i++) {
        for (int j = 0; j < matrix.getSize(); j++) {
            if (matrix.getCell(i, j) == 0) {
                coords.push_back({i, j});
            }
        }
    }
    std::cout << "Найденные нулевые элементы (кандидаты на ребро):" << std::endl;
    for (auto& coord : coords) {
        std::cout << "(" << coord.first << ", " << coord.second << ") ";
    }
    std::cout << std::endl;

    int maxdelta = -1;
    std::pair<int, int> bestCoord = {-1, -1};
    
    // Рассчитываем "цену" отказа от каждого нулевого элемента и выбираем наилучший
    for (auto &coord: coords) {
        matrix.setCell(coord.first, coord.second, INF);
        
        int minInRow = matrix.getMinInRow(coord.first);
        int minInCol = matrix.getMinInCol(coord.second);
        
        if (minInRow == INF) {
            minInRow = 0;
        }
        if (minInCol == INF) {
            minInCol = 0;
        }
        
        int delta = minInRow + minInCol;
        matrix.setCell(coord.first, coord.second, 0);

        std::cout << "Анализируем ноль в (" << coord.first << ", " << coord.second << "):" << std::endl;
        std::cout << "Минимум в строке: " << minInRow << ", минимум в столбце: " << minInCol << ", дельта: " << delta << std::endl;

        if (maxdelta < delta) {
            maxdelta = delta;
            bestCoord = {coord.first, coord.second};
        }
    }
    
    std::cout << "Выбрано ребро: (" << bestCoord.first << ", " << bestCoord.second << ") с дельтой " << maxdelta << "\n";
    return bestCoord;
}


std::pair<int, int> calcForbiddenArc(Matrix& matrix, std::vector<std::pair<int, int>>& path, int from, int to) {
    int X = from, Y = to;
    std::cout << "\n____________________________________\n";
    std::cout << "Запрет дуги:";
    // 1. Найти все рёбра, которые ведут к X
    bool cond = true;
    int incoming_to = X;
    while (cond) {
        cond = false;
        for (const auto& [u, v] : path) {
            if (v == incoming_to) { // Если из вершины v есть ребро к X
                cond = true;
                incoming_to = u; // Вершина, откуда приходит ребро к X (W → X)
                break;
            }
        }
    }
    
    // 2. Найти все рёбра, которые выходят из Y
    cond = true;
    int outgoing_from = Y;
    while (cond) {
        cond = false;
        for (const auto& [u, v] : path) {
            if (u == outgoing_from) { // Если из вершины Y выходит ребро к v
                cond = true;
                outgoing_from = v; // Вершина, в которую ведёт ребро от Y (Y → Z)
                break;
            }
        }
    }
    
    // Если есть вход в X (W → X) и выход из Y (Y → Z), то запрещаем ребро Z → W
    if (incoming_to != X || outgoing_from != Y) {
        std::cout << outgoing_from  << " " << incoming_to << "\n";
        return {outgoing_from, incoming_to};
    }
    
    // Если других связей нет, то просто запрещаем Y → X
    std::cout << X << " " << Y << "\n";
    return {Y, X};
}


void calcBnB(Matrix& matrix, int cost, std::vector<std::pair<int, int>>& path, std::vector<std::pair<int, int>>& bestPath, int& bestCost) {
    std::cout << "\n=============================" << std::endl;
    std::cout << "Текущий путь: ";
    for (auto& p : path) {
        std::cout << "(" << p.first << " -> " << p.second << ") ";
    }
    std::cout << "\nТекущая стоимость: " << cost << ", Лучшая известная стоимость: " << bestCost << std::endl;

    // Базовый случай: если путь содержит (n - 1) рёбер, замыкаем цикл
    if (path.size() == matrix._size - 1) { 
        if (cost < bestCost) {
            bestCost = cost;
            bestPath = path;
            std::cout << "Найден новый лучший путь с стоимостью " << bestCost << ":" << std::endl;
            for (auto& p : bestPath) {
                std::cout << "(" << p.first << " -> " << p.second << ") ";
            }
            std::cout << std::endl;

            for (int i = 0; i < matrix.getSize(); i++) {
                for (int j = 0; j < matrix.getSize(); j++) {
                    if (matrix.getCell(i, j) != INF) {
                        bestPath.push_back({i, j});
                    }
                }
            }
        }
        return;
    }

    // Проверяем, не превысила ли текущая стоимость наилучшее найденное решение
    if (cost >= bestCost) {
        std::cout << "Обрезка ветви: текущая стоимость " << cost << " >= " << bestCost << std::endl;
        return;
    }

    // Шаг 1: Редукция матрицы и обновление стоимости
    int reduction = reduce(matrix);
    int totalLowerBound = cost + reduction;
    std::cout << "После редукции: новая стоимость " << totalLowerBound << " (редукция на " << reduction << ")" << std::endl;

    if (totalLowerBound >= bestCost) {
        std::cout << "Обрезка ветви: нижняя граница " << totalLowerBound << " >= " << bestCost << std::endl;
        return;
    }

    // Шаг 2: Выбор наилучшего ребра
    std::pair<int, int> arc = calcBestArc(matrix);
    int i = arc.first, j = arc.second;

    if (i != -1) { 
        std::cout << "Выбрано ребро (" << i << " -> " << j << ")" << std::endl;

        // Ветка 1: Включаем ребро (i -> j)
        Matrix newMatrix1 = Matrix(matrix._matrix);
        std::pair<int, int> forb_arc = calcForbiddenArc(newMatrix1, path, i, j);
        
        path.push_back(arc);
        std::cout << "Добавили ребро (" << i << " -> " << j << ") в путь" << std::endl;

        newMatrix1.setCell(forb_arc.first, forb_arc.second, INF);
        std::cout << "Запрещено обратное ребро (" << forb_arc.first << " -> " << forb_arc.second << ")" << std::endl;

        newMatrix1.deleteRowCol(i, j);
        std::cout << "Удалены строка " << i << " и столбец " << j << " из матрицы" << std::endl;

        std::cout << "Переход в рекурсивный вызов (ветка 1: включение ребра)" << std::endl;
        calcBnB(newMatrix1, totalLowerBound, path, bestPath, bestCost);

        // Ветка 2: Исключаем ребро (i -> j)
        path.pop_back();
        std::cout << "Возвращаемся назад, убираем ребро (" << i << " -> " << j << ") из пути" << std::endl;

        matrix.setCell(i, j, INF);
        std::cout << "Запрещаем ребро (" << i << " -> " << j << "), переход к альтернативной ветви" << std::endl;

        calcBnB(matrix, totalLowerBound, path, bestPath, bestCost);
    }
}


// Восстановление пути из списка рёбер
std::vector<int> formWay(std::vector<std::pair<int, int>> &arcs, int start_vertex) {
    std::vector<int> resR, resL;
    int vertex = start_vertex;
    resR.push_back(vertex);

    // Двигаемся вперёд по рёбрам
    bool cond = true;
    while (cond) {
        cond = false;
        for (const auto &[u, v] : arcs) {
            if (u == vertex) {
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
    while (cond) {
        cond = false;
        for (const auto &[u, v] : arcs) {
            if (v == vertex) {
                cond = true;
                vertex = u;
                resL.insert(resL.begin(), vertex);
                break;
            }
        }
    }

    // Объединяем левую и правую части пути
    std::vector<int> res;
    res.insert(res.end(), resR.begin(), resR.end());
    res.insert(res.end(), resL.begin(), resL.end());

    return res;
}
};


int main() {
    Matrix matrix;
    //matrix.readMatrixFromConsole();
    matrix.loadFromFile("m5.txt");
    matrix.print();
    std::vector<std::pair<int, int>> path = {};
    std::vector<std::pair<int, int>> bestPath = {};
    int bestCost = INF;
    BranchAndBound desicion;
    desicion.calcBnB(matrix, 0, path, bestPath, bestCost);  
    std::vector<int> res =  desicion.formWay(bestPath, 0);
    std::cout << "\n Best Path: ";
    for (int i = 0; i < res.size() - 1; i++) {
        std::cout << res[i] << " ";
    }   
    std::cout << res[res.size() - 1];
    std::cout << "\n Cost: ";
    std::cout << std::fixed << std::setprecision(1) << (float)bestCost << "\n";
}