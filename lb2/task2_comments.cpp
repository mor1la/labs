#include <iostream>
#include <vector>

// Функция для вычисления массива LPS (Longest Prefix Suffix)
std::vector<int> computeLPS(const std::string &pattern) {
    int m = pattern.size(); // Длина шаблона
    std::vector<int> lps(m, 0); // Вектор для хранения значений LPS
    int i = 1, j = 0; // i - текущий индекс в шаблоне, j - длина предыдущего наибольшего префикса
    
    std::cout << "\n=== Вычисление LPS массива для шаблона: " << pattern << " ===\n";
    std::cout << "lps[0] = 0\n";
    while (i < m) {
        if (pattern[i] == pattern[j]) { // Если символы совпадают
            j++; // Увеличиваем длину префикса
            lps[i] = j; // Записываем значение в LPS-массив
            std::cout << "Совпадение: pattern[" << i << "] == pattern[" << j-1 << "] (" << pattern[i] << ") -> lps[" << i << "] = " << j << "\n";
            i++; // Переходим к следующему символу
        } else {
            if (j > 0) {
                std::cout << "Несовпадение: pattern[" << i << "] != pattern[" << j << "] (" << pattern[i] << " != " << pattern[j] << ") -> j = lps[" << j-1 << "] = " << lps[j-1] << "\n";
                j = lps[j - 1]; // Перемещаем j по LPS-массиву
            } else {
                lps[i] = 0; // Если нет совпадений, записываем 0
                std::cout << "Несовпадение: pattern[" << i << "] != pattern[" << j << "] (" << pattern[i] << " != " << pattern[j] << ") -> lps[" << i << "] = 0\n";
                i++; // Двигаем i дальше
            }
        }
    }
    
    std::cout << "LPS массив: ";
    for (int val : lps) std::cout << val << " ";
    std::cout << "\n";
    
    return lps;
}

// Функция для поиска подстроки (pattern) в строке (text) с использованием алгоритма searchKMP
int searchKMP(const std::string& text, const std::string& pattern) {
    std::vector<int> lps = computeLPS(pattern);  // Получаем массив LPS для паттерна
    int n = text.size(), m = pattern.size();  // Длины строк
    int i = 0, j = 0;  // Индексы для прохода по тексту и паттерну

    std::cout << "\n=== Начало поиска (searchKMP) ===\n";

    // Проходим по тексту
    while (i < n) {
        // Если символы совпадают, увеличиваем индексы i и j
        if (text[i] == pattern[j]) {
            i++, j++;
            std::cout << "i = " << i << ", j = " << j 
                      << ", text[0:" << i << "] = \"" << text.substr(0, i) 
                      << "\", pattern[0:" << j << "] = \"" << pattern.substr(0, j) << "\"\n";
            if (j == m) return i - m;  // Если все символы совпали, возвращаем индекс начала совпадения
        } else {
            if (j > 0) {
                std::cout << "Несовпадение: text[" << i << "] = " << text[i] 
                          << " и pattern[" << j << "] = " << pattern[j] << "\n";
                std::cout << "Откат к lps[j" << "-1] = lps["<< j << "-1] = "<< "Теперь j = " << lps[j - 1] << "\n";
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return -1;  // Если совпадения не найдено, возвращаем -1
}

// Функция для проверки, является ли строка str1 вращением строки str2
int check(const std::string& str1, const std::string& str2) {
    // Если строки имеют разные длины или одна из строк пуста, сразу возвращаем -1
    if (str1.size() != str2.size() || str1.empty()) {
        std::cout << "\n=== Проверка на вращение ===\n";
        std::cout << "Строки не могут быть вращением: различие в длине или одна из строк пуста.\n";
        return -1;
    }

    // Строка str2 удваивается, чтобы покрыть все возможные вращения
    std::string str = str2 + str2;
    std::cout << "\n=== Проверка на вращение ===\n";
    std::cout << "Удвоенная строка str2: " << str << "\n";
    
    // Используем алгоритм searchKMP для поиска str1 в удвоенной строке str2
    int result = searchKMP(str, str1);

    // Выводим результат проверки вращения
    if (result == -1) {
        std::cout << "Шаблон (str1) не найден в удвоенной строке (str2 + str2). Это означает, что str1 не является вращением str2.\n";
    } else {
        std::cout << "Шаблон найден на позиции: " << result << " в удвоенной строке. Это означает, что str1 является вращением str2.\n";
    }
    
    return result;
}

int main() {
    std::string str1, str2;
    std::cin >> str1 >> str2;  // Ввод двух строк

    std::cout << "Проверка на вращение: ";
    std::cout << "str1 = " << str1 << ", str2 = " << str2 << "\n";

    int result = check(str2, str1);  // Проверка, является ли str1 вращением str2
    std::cout << "\n=== Результат проверки вращения: ===\n";
    
    // Выводим результат
    if (result == -1) {
        std::cout << "Шаблон не найден (-1)\n";
    } else {
        std::cout << "Шаблон найден на позиции: " << result << "\n";
    }

    return 0;
}

