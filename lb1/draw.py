import pandas as pd
import matplotlib.pyplot as plt

# Функция для проверки, является ли число простым
def is_prime(n):
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

# Чтение данных из файла
data = pd.read_csv('time.txt', delim_whitespace=True, header=None, names=['Size', 'Time (ms)'])

# Разделим данные на три категории
even_data = data[data['Size'] % 2 == 0]  # Четные числа
prime_data = data[(data['Size'] % 2 != 0) & (data['Size'].apply(is_prime))]  # Нечетные простые числа
composite_data = data[(data['Size'] % 2 != 0) & (~data['Size'].apply(is_prime))]  # Нечетные составные числа

# Построение графика
plt.figure(figsize=(10, 6))

# График для четных чисел
plt.plot(even_data['Size'], even_data['Time (ms)'], marker='o', linestyle='-', color='r', label='Четные значения')

# График для простых чисел
plt.plot(prime_data['Size'], prime_data['Time (ms)'], marker='o', linestyle='-', color='b', label='Нечетные простые')

# График для составных чисел
plt.plot(composite_data['Size'], composite_data['Time (ms)'], marker='o', linestyle='-', color='g', label='Нечетные составные')

# Настройка графика
plt.title('Зависимость времени от размера квадрата')
plt.xlabel('Размер квадрата')
plt.ylabel('Время (мс)')
plt.legend()
plt.grid(True)

# Показать график
plt.show()