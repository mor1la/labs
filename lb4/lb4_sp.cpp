
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    int insertCost = 1;
    int deleteCost = 1;
    int replaceCost = 1;
    int specialInsertCost = 1;
    int specialReplaceCost = 1;
    char specialInsertChar, specialReplaceChar;
    std::cin >> specialInsertChar >> specialReplaceChar;
    std::cin >> specialInsertCost >> specialReplaceCost;
    std::cin >> replaceCost >> insertCost >> deleteCost;
    std::string str1, str2;
    std::cin >> str1 >> str2;
    int len1 = str1.length();
    int len2 = str2.length();
    std::vector<std::vector<int>> D(len1 + 1, std::vector<int>(len2 + 1, 0));

    D[0][0] = 0;  
    for (int j = 1; j <= len2; ++j) {
        D[0][j] = D[0][j - 1] + ((str2[j - 1] == specialInsertChar) ? specialInsertCost : insertCost);
    }

    for (int i = 1; i <= len1; ++i) {
        D[i][0] = D[i - 1][0] + deleteCost;
        for (int j = 1; j <= len2; ++j) {
            int costInsert = (str2[j - 1] == specialInsertChar) ? specialInsertCost : insertCost;
            int costReplace;
            if (str1[i - 1] == str2[j - 1]) {
                costReplace = 0;
            } else {
                costReplace = (str1[i - 1] == specialReplaceChar) ? specialReplaceCost : replaceCost;
            }

            D[i][j] = std::min({
                D[i - 1][j] + deleteCost,       // удаление
                D[i][j - 1] + costInsert,       // вставка
                D[i - 1][j - 1] + costReplace   // замена или совпадение
            });
        }
    }

    std::cout << D[len1][len2] << "\n";
}