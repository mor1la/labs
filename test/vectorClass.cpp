#include <array>
#include <cstdint>
#include <iostream>


template <typename T>
class vectorClass {
    size_t capacity;
    size_t currentSize;
    T* arr;

    void reserve() {
        T* temp = new T[capacity * 2];
        for (size_t i = 0; i < currentSize; i++) {
            temp[i] = arr[i];
        }
        
        delete[] arr;  
        arr = temp;
        capacity *= 2;
    }

    public:
    vectorClass() {
        arr = new T[1];
        capacity = 1;
        currentSize = 0;
    }

    ~vectorClass() {
        delete[] arr;  
    }

    size_t getSize() {
        return currentSize;
    }

    size_t getCapacity() {
        return capacity;
    }
    
    T getFront(){
        return arr[0];
    }

    T getBack() {
        return arr[currentSize - 1];
    }

    bool isEmpty() {
        if (currentSize == 0) 
            return true;
        return false;
    }

    void pushBack(T data) {
        if (currentSize == capacity) {
            reserve(); 
        }

        arr[currentSize] = data;
        currentSize++;
    }

    void popBack() {
        currentSize--;
    }

    void insert(size_t pos, T data) {
        if (currentSize == capacity) {
            reserve(); 
        }
        for (size_t i = currentSize; i > pos; i--) {
            arr[i] = arr[i - 1]; 
        }

        arr[pos] = data;
        currentSize++;
    }

    void erase(size_t pos) {
        for (size_t i = pos; i < currentSize - 1; i++) {
            arr[i] = arr[i + 1];
        }
        currentSize--;
    }

    void print() {
        for (size_t i = 0; i < currentSize; i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    vectorClass<int> meow;
    meow.pushBack(5);
    meow.print();
    return 0;
}
