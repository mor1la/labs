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
 
const int INF = std::numeric_limits<int>::max(); 
 
class Matrix 
{ 
public: 
    Matrix(int size) 
    { 
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

    void setCell(int row, int col, int val) 
    { 
        _matrix[row][col] = val; 
    } 
 
    int getCell(int row, int col) 
    { 
        return _matrix[row][col]; 
    } 
 
    int getSize() 
    { 
        return _size; 
    } 
 
    int getRelevantSize() 
    { 
        return _rows.size(); 
    } 
 
    bool isCellAvailable(int row, int col) 
    { 
        bool has_in_row = (std::find(_rows.begin(), _rows.end(), row) != _rows.end()); 
 
        bool has_in_col = (std::find(_cols.begin(), _cols.end(), col) != _cols.end()); 
        return has_in_row && has_in_col; 
    } 
 
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
        _rows.erase(std::remove(_rows.begin(), _rows.end(), row), 
_rows.end()); 
        _cols.erase(std::remove(_cols.begin(), _cols.end(), col), 
_cols.end()); 
    } 
 
    int getMinInRow(int row) 
    { 
        int min = INF; 
        for (int j = 0; j < _size; j++) 
        { 
            if (isCellAvailable(row, j)) 
            { 
                int curr = _matrix[row][j]; 
                if (curr < min) 
                { 
                    min = curr; 
                } 
            } 
        } 
        return min; 
    } 
 
    int getMinInCol(int col) 
    { 
        int min = INF; 
        for (int i = 0; i < _size; i++) 
        { 
            if (isCellAvailable(i, col)) 
            { 
                int curr = _matrix[i][col]; 
                if (curr < min) 
                { 
                    min = curr; 
                } 
            } 
        } 
        return min; 
    } 

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
 
    void print() 
    { 
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
 
private: 
    std::vector<std::vector<int>> _matrix; 
    std::vector<int> _rows, _cols; 
    int _size; 
}; 
 
class TSP 
{ 
public: 
    std::pair<int, std::vector<int>> BnB(Matrix& matrix, int 
start_vertex) 
    { 
        Matrix matr = matrix; 
        int best_cost = INF; 
        std::vector<std::pair<int, int>> path, best_path = {}; 
        calcBnB(matr, 0, path, best_cost, best_path); 
        std::vector<int> way = formWay(best_path, start_vertex); 
        return {best_cost, way}; 
    } 

    std::pair<int, std::vector<int>> NNA(Matrix& matrix, int 
start_vertex) 
    { 
        int size = matrix.getSize(); 
        int cost = 0; 
        std::vector<bool> visited(size, false); 
        std::vector<int> path; 
        int curr = start_vertex; 
        visited[curr] = true; 
        path.push_back(curr); 
        for (int i = 1; i < size; i++) 
        { 
            int next = -1; 
            int min = INF; 
            for (int j = 0; j < size; j++) 
            { 
                int val = matrix.getCell(curr, j); 
                if (!visited[j] && val < min && val < INF) 
                { 
                    min = val; 
                    next = j; 
                } 
            } 
            path.push_back(next); 
            visited[next] = true; 
            cost += min; 
            curr = next; 
        } 
        cost += matrix.getCell(curr, start_vertex); 
        path.push_back(start_vertex); 
        return {cost, path}; 
    } 
 
private: 
    int reduce(Matrix& matrix) 
    { 
        int bound = 0; 
        int size = matrix.getSize(); 
        for (int i = 0; i < size; i++) 
        { 
            int min = matrix.getMinInRow(i); 
            if (min > 0 && min < INF) 
            { 
                for (int j = 0; j < size; j++) 
                { 
                    int val = matrix.getCell(i, j); 
                    if (matrix.isCellAvailable(i, j) && val < INF) 
                    { 
                        matrix.setCell(i, j, val - min); 
                    } 
                } 
                bound += min; 
            } 
        } 
        for (int j = 0; j < size; j++) 
        { 
            int min = matrix.getMinInCol(j); 
            if (min > 0 && min < INF) 
            { 
                for (int i = 0; i < size; i++) 
                { 
                    int val = matrix.getCell(i, j); 
                    if (matrix.isCellAvailable(i, j) && val < INF) 
                    { 
                        matrix.setCell(i, j, val - min); 
                    } 
                } 
                bound += min; 
            } 
        } 
        return bound; 
    } 
 
    int calcMST(Matrix& matrix) 
    { 
        int cost = 0; 
        int size = matrix.getSize(); 
        int relevant_size = matrix.getRelevantSize(); 
        std::vector<bool> selected(relevant_size, false); 
        selected[0] = true; 
        int no_edge = 0; 
        while (no_edge < relevant_size - 1) 
        { 
            int min = INF; 
            int x = 0, y = 0; 
            for (int i = 0; i < size; i++) 
            { 
                if (selected[i] && matrix.isCellAvailable(i, 0)) 
                { 
                    for (int j = 0; j < size; j++) 
                    { 
                        if (matrix.isCellAvailable(i, j)) 
                        { 
                            int val = matrix.getCell(i, j); 
                            if (!selected[j] && val < INF) 
                            { 
                                if (min > val) 
                                { 
                                    min = val; 
                                    x = i, y = j; 
                                } 
                            } 
                        } 
                    } 
                } 
            } 
            cost += matrix.getCell(x, y); 
            selected[y] = true; 
            no_edge++; 
        } 
        return cost; 
    } 

    std::pair<int, int> calcBestArc(Matrix& matrix) 
    { 
        std::pair<int, int> best_arc = {-1, -1}; 
        int best_delta = 0; 
        int size = matrix.getSize(); 
        for (int i = 0; i < size; i++) 
        { 
            for (int j = 0; j < size; j++) 
            { 
                if (matrix.isCellAvailable(i, j)) 
                { 
                    if (matrix.getCell(i, j) == 0) 
                    { 
                        matrix.setCell(i, j, INF); 
                        int min_in_row = matrix.getMinInRow(i); 
                        int min_in_col = matrix.getMinInCol(j); 
                        if (min_in_row < INF && min_in_col < INF) 
                        { 
                            int delta = min_in_row + min_in_col; 
                            if (delta > best_delta) 
                            { 
                                best_delta = delta; 
                                best_arc = {i, j}; 
                            } 
                        } 
                        matrix.setCell(i, j, 0); 
                    } 
                } 
            } 
        } 
        return best_arc; 
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
 
    void calcBnB(Matrix& matrix, int cost, std::vector<std::pair<int, int>>& path, int& best_cost, std::vector<std::pair<int, int>>& best_path) 
    { 
        cost += reduce(matrix); 
        if (cost + calcMST(matrix) > best_cost) 
        { 
            return; 
        } 
        if (matrix.getRelevantSize() == 2) 
        { 
            if (cost < best_cost) 
            { 
                best_cost = cost; 
                best_path = path; 
            } 
            return; 
        } 
        std::pair<int, int> arc = calcBestArc(matrix); 
        int i = arc.first, j = arc.second; 
        if (i != -1) 
        { 
            path.push_back(arc); 
            Matrix new_matrix = matrix; 
            new_matrix.deleteRowCol(i, j); 
            std::pair<int, int> forb_arc = calcForbiddenArc(new_matrix, path, i, j); 
            new_matrix.setCell(forb_arc.first, forb_arc.second, INF); 
            calcBnB(new_matrix, cost, path, best_cost, best_path); 
 
            path.pop_back(); 
            matrix.setCell(i, j, INF); 
            calcBnB(matrix, cost, path, best_cost, best_path); 
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
    TSP tsp;
    Matrix matrix(data);
    // matrix.print();
    std::pair<int, std::vector<int>> res = tsp.BnB(matrix, 0);
    int best_cost = res.first;
    std::vector<int> path = res.second;
    std::cout << "";
    for (int i = 0; i < path.size() - 2; i++)
    {
        std::cout << path[i] << " ";
    }
    std::cout << path[path.size() - 2] << "\n";
    std::cout << std::fixed << std::setprecision(1) << (float)best_cost;
    return 0;
    // std::cout << bestCost << "\n";
    // for (auto a: path) {
    //     std::cout << a.first << "  " << a.second << "\n";
    // }
    // matrix.print();
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