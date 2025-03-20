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


const int INF = std::numeric_limits<int>::max(); 

class Matrix {
private:
    std::vector<std::vector<int>> _matrix;
    std::vector<int> _rows, _cols; 
    int _size;

public:
Matrix(int size) {
    _size = size;
    _matrix = std::vector<std::vector<int>>(_size, std::vector<int>(_size, 0));
    for (int i = 0; i < _size; i++) 
    { 
        _rows.push_back(i); 
        _cols.push_back(i); 
    } 

}

Matrix(std::vector<std::vector<int>> matrix) {
    _size = matrix.size();  
    _matrix = matrix; 
    for (int i = 0; i < _size; i++) { 
        _rows.push_back(i); 
        _cols.push_back(i); 
    } 

}

void setCell(int row, int col, int val) {
    _matrix[row][col] = val;
}

int getCell(int row, int col) {
    return _matrix[row][col];
}

int getSize() {
    return _size;
}

int getRelevantSize() {
    return _rows.size();
}

bool isCellAvailable(int row, int col) 
{ 
    bool has_in_row = (std::find(_rows.begin(), _rows.end(), row) != _rows.end()); 

    bool has_in_col = (std::find(_cols.begin(), _cols.end(), col) != _cols.end()); 
    return has_in_row && has_in_col; 
} 

void deleteRowCol(int row, int col) { 
    for (int i = 0; i < _size; i++) { 
        for (int j = 0; j < _size; j++) { 
            if (i == row || j == col) { 
                setCell(i, j, INF); 
            } 
        } 
    } 
    _rows.erase(std::remove(_rows.begin(), _rows.end(), row), _rows.end()); 
    _cols.erase(std::remove(_cols.begin(), _cols.end(), col), _cols.end()); 
} 

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

std::vector<std::vector<int>> getMatrix(){
    return _matrix;
} 

void print() { 
    std::cout << std::string(50, '-') << '\n'; 
    for (int i = 0; i < _size; i++) 
    { 
        for (int j = 0; j < _size; j++) 
        { 
            _matrix[i][j] == INF ? std::cout << "INF" : std::cout << _matrix[i][j]; 
            j == _size - 1 ? std::cout << '\n' : std::cout << ' '; 
        } 
    } 
    std::cout << std::string(50, '-') << '\n'; 
} 


};



int reduce(Matrix& matrix) {
    int bound = 0;

    // Обход строк
    for (int i = 0; i < matrix.getSize(); i++) {
        int min = matrix.getMinInRow(i);
        if (min == INF) continue;  // Пропускаем, если вся строка INF

        for (int j = 0; j < matrix.getSize(); j++) {
            if (matrix.isCellAvailable(i, j) && matrix.getCell(i, j) != INF) {
                matrix.setCell(i, j, matrix.getCell(i, j) - min);
            }
        }
        bound += min;
    }

    // Обход столбцов
    for (int j = 0; j < matrix.getSize(); j++) {
        int min = matrix.getMinInCol(j);
        if (min == INF) continue;  // Пропускаем, если весь столбец INF

        for (int i = 0; i < matrix.getSize(); i++) {
            if (matrix.isCellAvailable(i, j) && matrix.getCell(i, j) != INF) {
                matrix.setCell(i, j, matrix.getCell(i, j) - min);
            }
        }
        bound += min;
    }

    return bound;
}

std::pair<int, int> calcBestArc(Matrix& matrix) {
    std::vector<std::pair<int, int>> coords;
    for (int i = 0; i < matrix.getSize(); i++) {
        for (int j = 0; j < matrix.getSize(); j++) {
            if (matrix.isCellAvailable(i, j)) {
                if (matrix.getCell(i, j) == 0){
                    coords.push_back({i, j});
                }
            }
        }
    }
    int maxdelta = 0;
    std::pair<int, int> bestCoord = {-1, -1};
    for (auto &coord: coords) {
        matrix.setCell(coord.first, coord.second, INF); 
        int delta = matrix.getMinInRow(coord.first) + matrix.getMinInCol(coord.second);
        matrix.setCell(coord.first, coord.second, 0); 
        if (maxdelta < delta) {
            maxdelta = delta;
            bestCoord = {coord.first, coord.second};
        }
    }
    return bestCoord;
}

std::pair<int, int> calcForbiddenArc(Matrix& matrix, std::vector<std::pair<int, int>>& path, int from, int to) {
    int X = from, Y = to;
    
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
        return {outgoing_from, incoming_to};
    }
    
    // Если других связей нет, то просто запрещаем Y → X
    return {Y, X};
}


void calcBnB(Matrix& matrix, int cost, std::vector<std::pair<int, int>>& path, std::vector<std::pair<int, int>>& bestPath, int& bestCost) {
    cost += reduce(matrix);

    if (matrix.getRelevantSize() == 2) {
        if (cost < bestCost) {
            bestCost = cost;
            bestPath = path;
            // for (int i = 0; i < matrix.getSize(); i++) {
            //     for (int j = 0; j < matrix.getSize(); j++) {
            //         if (matrix.isCellAvailable(i, j) && matrix.getCell(i, j) != INF) {
            //             bestPath.push_back({i, j});
            //         }
            //     }
            // //Исправить
        }
        return;
    }

    std::pair<int, int> arc = calcBestArc(matrix);
    int i = arc.first, j = arc.second;
    if (i != -1) 
    { 
        path.push_back(arc); 
        Matrix newMatrix = matrix; 
        newMatrix.deleteRowCol(i, j); 
        std::pair<int, int> forb_arc = calcForbiddenArc(newMatrix, path, i, j); 
        newMatrix.setCell(forb_arc.first, forb_arc.second, INF); 
        calcBnB(newMatrix, cost, path, bestPath, bestCost); 

        path.pop_back(); 
        matrix.setCell(i, j, INF); 
        calcBnB(matrix, cost, path, bestPath, bestCost); 
    } 

}

std::vector<int> formWay(std::vector<std::pair<int, int>>& arcs, int start_vertex) 
{ 
    int size = arcs.size(); 
    std::vector<int> starts, ends; 
    for (int i = 0; i < size; i++) 
    { 
        bool is_start = true; 
        for (int j = 0; j < size; j++) 
        { 
            if (i == j) 
            { 
                continue; 
            } 
            if (arcs[i].first == arcs[j].second) 
            { 
                is_start = false; 
                break; 
            } 
        } 
        if (is_start) 
        { 
            starts.push_back(arcs[i].first); 
        } 
    } 
    std::unordered_map<int, int> arcs_map; std::set<int> vertices; 
    for (auto& arc : arcs) 
    { 
        arcs_map[arc.first] = arc.second; 
    } 
    for (int i = 0; i < starts.size(); i++) 
    { 
        int start = starts[i]; 
        while (arcs_map.find(start) != arcs_map.end()) 
        { 
            start = arcs_map[start]; 
            vertices.insert(start); 
        } 
        ends.push_back(start); 
        vertices.insert(start); 
    } 
    if (starts.size() == 1) 
    { 
        int vertex; 
        for (int i = 0; i < arcs.size() + 2; i++) 
        { 
            if (!vertices.count(i) && i != starts[0]) 
            { 
                vertex = i; 
                break; 
            } 
        } 
        starts.push_back(vertex); 
        ends.push_back(vertex); 
    } 
    arcs_map[ends[0]] = starts[1]; 
    arcs_map[ends[1]] = starts[0]; 
    std::vector<int> way = {start_vertex}; 
    int vertex = start_vertex; 
    while (arcs_map[vertex] != start_vertex) 
    { 
        vertex = arcs_map[vertex]; 
        way.push_back(vertex); 
    } 
    way.push_back(start_vertex); 
    return way; 
} 



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
    calcBnB(matrix, 0, path, bestPath, bestCost);
    std::vector<int> res = formWay(bestPath, 0);
    for (int i = 0;i < res.size() - 2; i++) {
        std::cout << res[i] << " ";
    }
    std::cout << res[res.size() - 2];
    std::cout << "\n";
    float bestCost1 = static_cast<float>(bestCost);
    std::cout << std::fixed << std::setprecision(1) << bestCost1 << "\n";
    // int bound = reduce(matrix);
    // int bound = reduce(matrix);
    // std::cout << bound << "\n";
    // matrix.deleteRowCol(1, 0);
    // matrix.print();
    // bound = reduce(matrix);
    // std::cout << bound << "\n";
    // matrix.print();

    // std::vector<std::vector<int>> data = {
    //     {INF, INF, INF, INF, INF},
    //     {INF, INF, INF, INF, INF},
    //     {INF, 11, INF, INF, INF},
    //     {INF, INF, INF, INF, INF},
    //     {INF, INF, 0, INF, INF}
    // };
    // Matrix matrix(data);
    // matrix.deleteRowCol(1, 0);
    // matrix.deleteRowCol(3, 4);
    // matrix.deleteRowCol(0, 3);
    // std::vector<std::pair<int, int>> meow = {{1,0}, {3, 4}};
    // auto ne = calcForbiddenArc(matrix, meow, 0, 3);

    // std::cout << ne.first << " " << ne.second;


    // std::vector<std::vector<int>> data = {
    //     {INF, INF, 13, 1, 0},
    //     {INF, INF, INF, INF, INF},
    //     {INF, 13, INF, 2, 0},
    //     {INF, 43, 18, INF, 0},
    //     {INF, 0, 0, 0, INF}
    // };
    // Matrix matrix(data);
    // matrix.deleteRowCol(1, 0);
    // //matrix.deleteRowCol(3, 4);

    // std::vector<std::pair<int, int>> meow = {{1,0}};
    // auto ne = calcForbiddenArc(matrix, meow, 3, 4);

    // std::cout << ne.first << " " << ne.second;
}