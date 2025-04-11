#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void printD(std::vector<std::vector<int>>& D, const std::string& strA, const std::string& strB) {
    std::cout << "# ";
    for (int x = 0; x < strB.size() + 1; x++) {
        char b = x > 0 ? strB[x - 1] : '~';
        std::cout << b << ' ';
    }
    std::cout << '\n';

    for (int y = 0; y < strA.size() + 1; y++) {
        char a = y > 0 ? strA[y - 1] : '~';
        std::cout << a << ' ';
        for (int x = 0; x < strB.size() + 1; x++) {
            std::cout << D[y][x] << ' ';
        }
        std::cout << '\n';
    }
}

int calculateEditDistance(
    const std::string& str1, const std::string& str2,
    int insertCost, int deleteCost, int replaceCost,
    char specialInsertChar, int specialInsertCost,
    char specialReplaceChar, int specialReplaceCost
) {
    std::cout << "===========================================\n";
    std::cout << "\tНачало работы алгоритма\n";
    std::cout << "Строка A: " << str1 << '\n';
    std::cout << "Строка B: " << str2 << '\n';
    std::cout << "Стандартные стоимости: вставка = " << insertCost << ", удаление = " << deleteCost << ", замена = " << replaceCost << '\n';
    std::cout << "Спецсимволы: вставка '" << specialInsertChar << "' (стоимость " << specialInsertCost << "), замена на '" << specialReplaceChar << "' (стоимость " << specialReplaceCost << ")\n";

    int len1 = str1.length();
    int len2 = str2.length();
    std::vector<std::vector<int>> D(len1 + 1, std::vector<int>(len2 + 1, 0));

    std::cout << "\nМатрица сразу после создания:\n";
    printD(D, str1, str2);
    std::cout << '\n';

    std::cout << "Заполняем первую строку (вставка символов во вторую строку):\n";
    for (int j = 1; j <= len2; ++j) {
        int cost = (str2[j - 1] == specialInsertChar) ? specialInsertCost : insertCost;
        std::cout << "   Вставляем '" << str2[j - 1] << "': ";
        if (str2[j - 1] == specialInsertChar)
            std::cout << "спецсимвол, берём спецстоимость " << specialInsertCost;
        else
            std::cout << "обычный символ, берём стандартную стоимость " << insertCost;

        D[0][j] = D[0][j - 1] + cost;
        std::cout << " → " << D[0][j - 1] << '+' << cost << '=' << D[0][j]
                  << " → в ячейку (" << j << ", 0)\n";
    }

    std::cout << "\nЗаполняем первый столбец (удаление символов из первой строки):\n";
    for (int i = 1; i <= len1; ++i) {
        D[i][0] = D[i - 1][0] + deleteCost;
        std::cout << "   Удаляем '" << str1[i - 1] << "': стандартная стоимость "
                  << deleteCost << " → " << D[i - 1][0] << '+' << deleteCost << '=' << D[i][0]
                  << " → в ячейку (0, " << i << ")\n";
    }

    std::cout << "\nМатрица после заполнения первой строки и первого столбца:\n";
    printD(D, str1, str2);
    std::cout << '\n';

    for (int i = 1; i <= len1; ++i) {
        std::cout << "Заполняем " << i << "-ю строку:\n";
        for (int j = 1; j <= len2; ++j) {
            int costInsert = (str2[j - 1] == specialInsertChar) ? specialInsertCost : insertCost;

            int costReplace;
            if (str1[i - 1] == str2[j - 1]) {
                std::cout << "   Символы равны: " << str1[i - 1] << " == " << str2[j - 1] << '\n';
                costReplace = 0;
                D[i][j] = D[i - 1][j - 1];
                std::cout << "\tПереписываем " << D[i][j] << " из (" << j - 1 << ", " << i - 1 << ") → (" << j << ", " << i << ")\n";
            } else {
                costReplace = (str1[i - 1] == specialReplaceChar) ? specialReplaceCost : replaceCost;
                std::cout << "   Символы не равны: '" << str1[i - 1] << "' != '" << str2[j - 1] << "'\n";
                std::cout << "\tУдаление: " << D[i - 1][j] << " + " << deleteCost << " = " << (D[i - 1][j] + deleteCost) << '\n';
                std::cout << "\tВставка '" << str2[j - 1] << "': ";
                if (str2[j - 1] == specialInsertChar)
                    std::cout << "спецстоимость = " << specialInsertCost;
                else
                    std::cout << "стандартная = " << insertCost;
                std::cout << " → " << D[i][j - 1] + costInsert << '\n';

                std::cout << "\tЗамена '" << str1[i - 1] << "' → '" << str2[j - 1] << "': ";
                if (str1[i - 1] == specialReplaceChar)
                    std::cout << "спецстоимость = " << specialReplaceCost;
                else
                    std::cout << "стандартная = " << replaceCost;
                std::cout << " → " << D[i - 1][j - 1] + costReplace << '\n';

                D[i][j] = std::min({
                    D[i - 1][j] + deleteCost,
                    D[i][j - 1] + costInsert,
                    D[i - 1][j - 1] + costReplace
                });

                std::cout << "\t→ Минимум из трёх: записываем " << D[i][j] << " в (" << j << ", " << i << ")\n";
            }
        }
        std::cout << "\nМатрица после строки " << i << ":\n";
        printD(D, str1, str2);
        std::cout << '\n';
    }

    std::cout << "Финальная матрица:\n";
    printD(D, str1, str2);
    std::cout << "\tКонец работы алгоритма\n";
    std::cout << "===========================================\n";

    return D[len1][len2];
}

int main() {
    int insertCost, deleteCost, replaceCost;
    int specialInsertCost, specialReplaceCost;
    char specialInsertChar, specialReplaceChar;

    std::cout << "Введите спецсимвол для ВСТАВКИ и ЗАМЕНЫ (через пробел): ";
    std::cin >> specialInsertChar >> specialReplaceChar;
    std::cout << "Введите соответствующие спецстоимости вставки и замены: ";
    std::cin >> specialInsertCost >> specialReplaceCost;
    std::cout << "Введите стандартные стоимости замены, вставки и удаления: ";
    std::cin >> replaceCost >> insertCost >> deleteCost;

    std::string str1, str2;
    std::cout << "Введите строку A: ";
    std::cin >> str1;
    std::cout << "Введите строку B: ";
    std::cin >> str2;

    int result = calculateEditDistance(
        str1, str2,
        insertCost, deleteCost, replaceCost,
        specialInsertChar, specialInsertCost,
        specialReplaceChar, specialReplaceCost
    );

    std::cout << "Расстояние Левенштейна = " << result << '\n';
    return 0;
}
