#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Вывод матрицы
void printD(std::vector<std::vector<int>>& D, std::string& strA, std::string& strB){
    std::cout << "# ";
    for (int x = 0; x < strB.size()+1; x++){ // Проходим по слову Б
        char b = x > 0 ? strB[x-1] : '~';
        std::cout << b << ' '; // Посимвольно печатаем слово Б
    }
    std::cout << '\n';

    for (int y = 0; y < strA.size()+1; y++){ // Проходим по строчкам матрицы (по слову А)
        char a = y > 0 ? strA[y-1] : '~';
        std::cout << a << ' '; // Посимвольно печатаем слово А
        for (int x = 0; x < strB.size()+1; x++){ // Проходим по элементам строк матрицы
            std::cout << D[y][x] << ' '; // Печатаем очередной элемент матрицы
        }
        std::cout << '\n';
    }
}

// Алгоритм Вагнера-Фишера для поиска расстояния Левенштейна
int levenstein(std::string strA, std::string strB, int replaceCost, int insertCost, int deleteCost){
    std::cout << "===========================================\n";
    std::cout << "\tНачало работы алгоритма\n";
    int m = strA.size(); // Длина слова А
    int n = strB.size(); // Длина слова Б

    // Матрица, каждый элемент которой - минимальное количество операций 
    // для преобразованию первых у символов первого слова к первым х символам второго слова
    std::vector<std::vector<int>> D(m+1, std::vector<int>(n+1, 0));
    std::cout << "Матрица сразу после создания:\n";
    printD(D, strA, strB);
    std::cout << '\n';

    std::cout << "Заполняем первую строку и первый столбец:\n";
    for (int x = 1; x < n+1; x++){ // Заполняем первую строку матрицы
        D[0][x] = D[0][x-1] + insertCost; // След. элемент = пред. элемент + цена добавления
        std::cout << "   Значение на позиции (" << x-1 << ", 0) = " << D[0][x-1];
        std::cout << "; стоимость операции добавления = " << insertCost << '\n';
        std::cout << "   Ставим " << D[0][x-1] << '+' << insertCost << '=' << D[0][x] << " на позицию (" << x << ", 0)\n";
    }
    std::cout << '\n';
    for (int y = 1; y < m+1; y++){ // Заполняем первый столбец матрицы
        D[y][0] = D[y-1][0] + deleteCost; // След. элемент = пред. элемент + цена удаления
        std::cout << "   Значение на позиции (0, " << y-1 << ") = " << D[y-1][0];
        std::cout << "; стоимость операции удаления = " << deleteCost << '\n';
        std::cout << "   Ставим " << D[y-1][0] << '+' << deleteCost << '=' << D[y][0] << " на позицию (0, " << y << ")\n";
    }

    std::cout << "\nМатрица после заполнения первой строки и первого столбца:\n";
    printD(D, strA, strB);
    std::cout << '\n';

    for (int y = 1; y < m+1; y++){ // Проходим по строкам матрицы
        std::cout << "Заполняем " << y << "-ю строку:\n";
        for (int x = 1; x < n+1; x++){ // Проходим по элементам строк матрицы
            if (strA[y-1] != strB[x-1]){ // Если пред. символы слова А и слова Б не равны
                std::cout << "   Символ " << strA[y-1] << " слова " << strA << " на позиции " << y-1 << " не равен ";
                std::cout << "символу " << strB[x-1] << " слова " << strB << " на позиции " << x-1 << '\n';
                std::cout << "\tВыбираем минимум из доступных операций:\n";
                std::cout << "\tЗначение на позиции (" << x << ", " << y-1 << ") = " << D[y-1][x];
                std::cout << "; стоимость операции удаления = " << deleteCost << '\n';
                std::cout << "\t\tУдаление: " << D[y-1][x] << '+' << deleteCost << '=' << D[y-1][x] + deleteCost << '\n';
                std::cout << "\tЗначение на позиции (" << x-1 << ", " << y << ") = " << D[y][x-1];
                std::cout << "; стоимость операции добавления = " << insertCost << '\n';
                std::cout << "\t\tДобавление: " << D[y][x-1] << '+' << insertCost << '=' << D[y][x-1] + insertCost << '\n';
                std::cout << "\tЗначение на позиции (" << x-1 << ", " << y-1 << ") = " << D[y-1][x-1];
                std::cout << "; стоимость операции замены = " << replaceCost << '\n';
                std::cout << "\t\tЗамена: " << D[y-1][x-1] << '+' << replaceCost << '=' << D[y-1][x-1] + replaceCost << '\n';

                D[y][x] = std::min( // Текущим элементом выбираем минимум из
                    {D[y-1][x] + deleteCost, // Удаления
                    D[y][x-1] + insertCost, // Вставки
                    D[y-1][x-1] + replaceCost} // Замены
                );

                std::cout << "\t   Записываем " << D[y][x] << " на позицию (" << x << ", " << y << ")\n";
            } 
            else{ // Иначе, если равны
                std::cout << "   Символ " << strA[y-1] << " слова " << strA << " на позиции " << y-1 << " равен ";
                std::cout << "символу " << strB[x-1] << " слова " << strB << " на позиции " << x-1 << '\n';
                D[y][x] = D[y-1][x-1]; // Расстояние не увеличивается
                std::cout << "\tПереписываем " << D[y][x] << " из позиции (" << x-1 << ", " << y-1 << ") ";
                std::cout << "на позицию (" << x << ", " << y << ")\n";
            }
        }

        std::cout << "\nМатрица после заполнения " << y << "-й строки:\n";
        printD(D, strA, strB);
        std::cout << '\n';
    }

    std::cout << "Матрица после полного заполнения:\n";
    printD(D, strA, strB);
    std::cout << "\tКонец работы алгоритма\n";
    std::cout << "===========================================\n";

    return D[m][n]; // Возвращаем нижний правый элемент матрицы (расстояние Левенштейна)
}

int main(){
    int replaceCost, insertCost, deleteCost; // Цены за замену, вставку, удаление
    std::cout << "Введите цены за замену/вставку/удаление: ";
    std::cin >> replaceCost >> insertCost >> deleteCost; // Считывание цен
    std::string strA, strB; // Слова А и Б
    std::cout << "Введите слово А: ";
    std::cin >> strA; // Считываем слово А
    std::cout << "Введите слово Б: ";
    std::cin >> strB; // Считываем слово Б
    // Вызываем алгоритм и выводим результат его выполнения
    int result = levenstein(strA, strB, replaceCost, insertCost, deleteCost);
    std::cout << "Расстояние Левенштейна = " << result << '\n';
    return 0;
}