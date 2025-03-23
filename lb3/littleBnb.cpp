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
    void saveToFile(const std::string& filename)
    {
        std::ofstream file(filename);
        if (file.is_open())
        {
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    file << _matrix[i][j] << " ";
                }
                file << '\n';
            }
        }
        file.close();
    }

    //загрузить матрицу из файла
    void loadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        std::vector<int> row;
        _matrix.clear();
        int val;
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            row.clear();
            while (iss >> val)
            {
                row.push_back(val);
            }
            _matrix.push_back(row);
        }
    }
};


class BranchAndBound {
public:
    // Функция выполняет редукцию матрицы (вычитает минимальные значения из строк и столбцов) и вычисляет нижнюю границу стоимости пути
    int reduce(Matrix& matrix) {
        int bound = 0;
    
        // Обход строк
        for (int i = 0; i < matrix.getSize(); i++) {
            int min = matrix.getMinInRow(i);
            if (min == INF) continue;  // Пропускаем, если вся строка INF
    
            // Вычитаем минимальный элемент из всех элементов строки
            for (int j = 0; j < matrix.getSize(); j++) {
                if (matrix.getCell(i, j) != INF) {
                    matrix.setCell(i, j, matrix.getCell(i, j) - min);
                }
            }
            bound += min;
        }
    
        // Обход столбцов
        for (int j = 0; j < matrix.getSize(); j++) {
            int min = matrix.getMinInCol(j);
            if (min == INF) continue;  // Пропускаем, если весь столбец INF
    
            // Вычитаем минимальный элемент из всех элементов столбца
            for (int i = 0; i < matrix.getSize(); i++) {
                if (matrix.getCell(i, j) != INF) {
                    matrix.setCell(i, j, matrix.getCell(i, j) - min);
                }
            }
            bound += min;
        }
    
        return bound;
    }
    
    // Функция вычисляет "лучшее" ребро, которое нужно добавить в маршрут
    std::pair<int, int> calcBestArc(Matrix& matrix) {
        std::vector<std::pair<int, int>> coords;
        
        // Поиск всех нулевых элементов в матрице (потенциальные кандидаты на добавление в маршрут)
        for (int i = 0; i < matrix.getSize(); i++) {
            for (int j = 0; j < matrix.getSize(); j++) {
                if (matrix.getCell(i, j) == 0){
                    coords.push_back({i, j});
                }
            }
        }
    
        int maxdelta = -1;
        std::pair<int, int> bestCoord = {-1, -1};
        
        // Для каждого нулевого элемента вычисляем "стоимость" его выбора (дельта)
        for (auto &coord: coords) {
            matrix.setCell(coord.first, coord.second, INF);
            int minInRow = matrix.getMinInRow(coord.first);
            int minInCol = matrix.getMinInCol(coord.second);
            
            if (minInRow == INF) minInRow = 0;
            if (minInCol == INF) minInCol= 0;
            
            int delta = minInRow + minInCol;
            matrix.setCell(coord.first, coord.second, 0); 
            
            if (maxdelta < delta) {
                maxdelta = delta;
                bestCoord = {coord.first, coord.second};
            }
        }
        return bestCoord;
    }
    
    // Функция определяет запрещённое ребро (чтобы не образовывался цикл)
    std::pair<int, int> calcForbiddenArc(Matrix& matrix, std::vector<std::pair<int, int>>& path, int from, int to) {
        int X = from, Y = to;
    
        // Найти все рёбра, которые ведут к X
        bool cond = true;
        int incoming_to = X;
        while (cond) {
            cond = false;
            for (const auto& [u, v] : path) {
                if (v == incoming_to) {
                    cond = true;
                    incoming_to = u;
                    break;
                }
            }
        }
        
        // Найти все рёбра, которые выходят из Y
        cond = true;
        int outgoing_from = Y;
        while (cond) {
            cond = false;
            for (const auto& [u, v] : path) {
                if (u == outgoing_from) {
                    cond = true;
                    outgoing_from = v;
                    break;
                }
            }
        }
        
        // Если найден вход в X и выход из Y, запрещаем Z → W
        if (incoming_to != X || outgoing_from != Y) {
            return {outgoing_from, incoming_to};
        }
        
        // Иначе запрещаем Y → X
        return {Y, X};
    }
    
    // Функция выполняет основной алгоритм ветвей и границ
    void calcBnB(Matrix& matrix, int cost, std::vector<std::pair<int, int>>& path, std::vector<std::pair<int, int>>& bestPath, int& bestCost) {
        if (path.size() == matrix._size - 1) { 
            if (cost < bestCost) {
                bestCost = cost;
                bestPath = path;
                for (int i = 0; i < matrix.getSize(); i++) {
                    for (int j = 0; j < matrix.getSize(); j++) {
                        if (matrix.getCell(i, j) != INF) {
                            bestPath.push_back({i, j});
                        }
                    }
                }
            return;
            }
        }
    
        if (cost >= bestCost) {
            return;
        }
    
        cost += reduce(matrix);
        std::pair<int, int> arc = calcBestArc(matrix);
        int i = arc.first, j = arc.second;
        if (i != -1) { 
            Matrix newMatrix = Matrix(matrix._matrix); 
            std::pair<int, int> forb_arc = calcForbiddenArc(newMatrix, path, i, j);
            path.push_back(arc);
            newMatrix.setCell(forb_arc.first, forb_arc.second, INF);
            newMatrix.deleteRowCol(i, j);
            calcBnB(newMatrix, cost, path, bestPath, bestCost); 
            path.pop_back();
            matrix.setCell(i, j, INF);
            calcBnB(matrix, cost, path, bestPath, bestCost); 
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
    std::vector<std::pair<int, int>> path = {};
    std::vector<std::pair<int, int>> bestPath = {};
    int bestCost = INF;
    Matrix matrix(data);
    BranchAndBound desicion;
    desicion.calcBnB(matrix, 0, path, bestPath, bestCost);  
    std::vector<int> res =  desicion.formWay(bestPath, 0);
    for (int i = 0; i < res.size() - 1; i++) {
        std::cout << res[i] << " ";
    }   
    std::cout << res[res.size() - 1];
    std::cout << "\n";
    std::cout << std::fixed << std::setprecision(1) << (float)bestCost << "\n";
}