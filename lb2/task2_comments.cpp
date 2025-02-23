#include <iostream>
#include <vector>

// Функция для вычисления массива LPS (Longest Prefix Suffix) для заданной строки
std::vector<int> computeLPS(const std::string& pattern) {
    int m = pattern.size();  // Длина паттерна
    std::vector<int> lps(m, 0);  // Массив LPS, изначально заполнен нулями
    int i = 1, j = 0;  // Индексы для прохода по строке и для сравнения

    std::cout << "\n=== Вычисление LPS массива для шаблона: " << pattern << " ===\n";
    // Проходим по строке с паттерном, начиная с индекса 1
    while (i < m) {
        // Если текущие символы совпадают, увеличиваем индекс j, который отслеживает длину префикса
        if (pattern[i] == pattern[j]) {
            lps[i++] = ++j;  // Обновляем lps[i] и увеличиваем i и j
            std::cout << "Совпадение: pattern[" << i-1 << "] == pattern[" << j-1 << "] (" << pattern[i-1] << ") -> lps[" << i-1 << "] = " << j << "\n";
        } else if (j > 0) {
            // Если есть частичное совпадение (j > 0), пытаемся уменьшить j, используя информацию из lps
            std::cout << "Несовпадение: pattern[" << i << "] != pattern[" << j << "] (" << pattern[i] << " != " << pattern[j] << ") -> j = lps[" << j-1 << "] = " << lps[j-1] << "\n";
            j = lps[j - 1];  // Идем по ранее вычисленным префиксам
        } else {
            // Если символы не совпали, присваиваем lps[i] равным нулю и двигаемся дальше
            lps[i++] = 0;
            std::cout << "Несовпадение: pattern[" << i-1 << "] != pattern[" << j << "] (" << pattern[i-1] << " != " << pattern[j] << ") -> lps[" << i-1 << "] = 0\n";
        }
    }

    std::cout << "LPS массив: ";
    for (int val : lps) std::cout << val << " ";
    std::cout << "\n";

    return lps;  // Возвращаем вычисленный массив LPS
}

// Функция для поиска подстроки (pattern) в строке (text) с использованием алгоритма KMP
int KMP(const std::string& text, const std::string& pattern) {
    std::vector<int> lps = computeLPS(pattern);  // Получаем массив LPS для паттерна
    int n = text.size(), m = pattern.size();  // Длины строк
    int i = 0, j = 0;  // Индексы для прохода по тексту и паттерну

    std::cout << "\n=== Начало поиска (KMP) ===\n";

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
                j = lps[j - 1];
                std::cout << "Откат к lps[" << j << "]\n";
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
    
    // Используем алгоритм KMP для поиска str1 в удвоенной строке str2
    int result = KMP(str, str1);

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

