CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Имя исполняемого файла
TARGET = main

# Исходники и объектные файлы
SRCS = main.cpp multiply.cpp matrix.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

# Сборка
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Правило для .o файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)
