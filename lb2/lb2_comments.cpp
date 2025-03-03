#include <iostream>
#include <vector>

// Функция для вычисления массива LPS (Longest Prefix Suffix)
// Этот массив используется для оптимизации поиска, указывая, 
// насколько можно сдвинуть шаблон при несовпадении
std::vector<int> computeLPS(const std::string &pattern) {
    int m = pattern.size(); // Длина шаблона
    std::vector<int> lps(m, 0); // Вектор для хранения значений LPS
    int i = 1, j = 0; // i - текущий индекс в шаблоне, j - длина предыдущего наибольшего префикса
    
    while (i < m) {
        if (pattern[i] == pattern[j]) { // Если символы совпадают
            j++; // Увеличиваем длину префикса
            lps[i] = j; // Записываем значение в LPS-массив
            i++; // Переходим к следующему символу
        } else {
            if (j > 0) {
                j = lps[j - 1]; // Перемещаем j по LPS-массиву
            } else {
                lps[i] = 0; // Если нет совпадений, записываем 0
                i++; // Двигаем i дальше
            }
        }
    }
    return lps;
}

// Функция поиска подстроки в строке с помощью алгоритма Кнута-Морриса-Пратта
// Использует предварительно вычисленный LPS-массив для эффективного поиска
std::vector<int> searchKMP(const std::string &text, const std::string &pattern) {
    std::vector<int> result; // Вектор для хранения индексов вхождения шаблона в текст
    std::vector<int> lps = computeLPS(pattern); // Вычисляем LPS-массив
    int n = text.size(), m = pattern.size(); // Длины текста и шаблона
    int i = 0, j = 0; // i - текущий индекс в тексте, j - текущий индекс в шаблоне

    while (i < n) {
        if (text[i] == pattern[j]) { // Если символы совпадают
            i++; // Двигаем оба указателя вперёд
            j++;
            if (j == m) { // Если достигли конца шаблона, значит нашли вхождение
                result.push_back(i - m); // Записываем индекс начала вхождения
                j = lps[j - 1]; // Сдвигаем j в соответствии с LPS-массивом
            }
        } else {
            if (j > 0) {
                j = lps[j - 1]; // Перемещаем j по LPS-массиву, не двигая i
            } else {
                i++; // Если j == 0, просто двигаем i
            }
        }
    }

    return result; // Возвращаем вектор индексов всех вхождений шаблона в текст
}

int main() {
    std::string pattern, text;
    std::cin >> pattern >> text; // Ввод шаблона и текста с консоли
    std::vector<int> result = searchKMP(text, pattern); // Выполняем поиск searchKMP

    if (result.empty()) {
        std::cout << -1; // Если вхождений нет, выводим -1
    } else {
        for (size_t i = 0; i < result.size(); i++) {
            if (i > 0) std::cout << ','; // Разделяем индексы запятой для удобного вывода
            std::cout << result[i]; // Выводим найденный индекс
        }
    }
    std::cout << '\n'; // Перенос строки в конце вывода
}
