#include <iostream>
#include <vector>

// Функция для вычисления массива LPS (Longest Prefix Suffix)
std::vector<int> computeLPS(const std::string &pattern) {
    int m = pattern.size(); // Длина шаблона
    std::vector<int> lps(m, 0); // Вектор для хранения значений LPS
    int i = 1, j = 0; // i - текущий индекс в шаблоне, j - длина предыдущего наибольшего префикса
    
    std::cout << "\n=== Вычисление LPS массива для шаблона: " << pattern << " ===\n";
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

// Функция поиска подстроки в строке с помощью алгоритма Кнута-Морриса-Пратта
std::vector<int> KMP(const std::string &text, const std::string &pattern) {
    std::vector<int> result;
    std::vector<int> lps = computeLPS(pattern);
    int n = text.size(), m = pattern.size();
    int i = 0, j = 0;
    
    std::cout << "\n=== Начало поиска (KMP) ===\n";
    
    while (i < n) {
        if (text[i] == pattern[j]) {
            j++;
            i++;
            std::cout << "i = " << i << ", j = " << j 
                      << ", text[0:" << i << "] = \"" << text.substr(0, i) 
                      << "\", pattern[0:" << j << "] = \"" << pattern.substr(0, j) << "\"\n";
            if (j == m) {
                std::cout << "Найдено совпадение в позиции: " << (i - m) << "\n";
                result.push_back(i - m);
                j = lps[j - 1];
            }
        } else {
            if (j > 0) {
                std::cout << "Несовпадение: text[" << i << "] = " << text[i] 
                          << " и pattern[" << j << "] = " << pattern[j] << "\n";
                j = lps[j - 1];
                std::cout << "Откат к lps[" << j << "]\n";
            } else {
                i++;
            }
        }
    }
    
    if (result.empty()) {
        std::cout << "Совпадений не найдено.\n";
    }
    
    return result;
}

int main() {
    std::string pattern, text;
    std::cout << "Введите шаблон: ";
    std::cin >> pattern;
    std::cout << "Введите текст: ";
    std::cin >> text;
    
    std::vector<int> result = KMP(text, pattern); // Выполняем поиск KMP

    std::cout << "\n=== Результат поиска: ===\n";
    if (result.empty()) {
        std::cout << "Шаблон не найден (-1)\n";
    } else {
        std::cout << "Шаблон найден на позициях: ";
        for (size_t i = 0; i < result.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << result[i];
        }
        std::cout << "\n";
    }
    return 0;
}
