

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    int insertCost = 1;
    int deleteCost = 1;
    int replaceCost = 1;
    std::cin >> replaceCost >> insertCost >> deleteCost;
    std::string str1, str2;
    std::cin >> str1 >> str2;
    int len1 = str1.length();
    int len2 = str2.length();
    std::vector<std::vector<int>> D(len1 + 1, std::vector<int>(len2 + 1, 0));

    D[0][0] = 0;  

    for (int j = 1; j <= len2; ++j) {
        D[0][j] = D[0][j - 1] + insertCost;
    }

    for (int i = 1; i <= len1; ++i) {
        D[i][0] = D[i - 1][0] + deleteCost;
        for (int j = 1; j <= len2; ++j) {
            if (str1[i - 1] != str2[j - 1]) {
                D[i][j] = std::min({D[i - 1][j] + deleteCost, 
                                   D[i][j - 1] + insertCost, 
                                   D[i - 1][j - 1] + replaceCost});
            } else {
                D[i][j] = D[i - 1][j - 1];
            }
        }
    }
    
    int i = str1.size();
    int j = str2.size();
    std::vector<char> operations;

    while (i > 0 || j > 0) {
        if (i > 0 && D[i][j] == D[i - 1][j] + deleteCost) {
            operations.push_back('D');
            i--;
        } else if (j > 0 && D[i][j] == D[i][j - 1] + insertCost) {
            operations.push_back('I');
            j--;
        } else if (i > 0 && j > 0 && D[i][j] == D[i - 1][j - 1] + replaceCost && str1[i - 1] != str2[j - 1]) {
            operations.push_back('R');
            i--; j--;
        } else { // Совпадение
            operations.push_back('M');
            i--; j--;
        }
    }

    std::reverse(operations.begin(), operations.end());
    for (char op : operations) std::cout << op;
    std::cout << "\n";

}