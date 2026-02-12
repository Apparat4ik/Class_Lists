#pragma once

#include "List_header.hpp"

template<typename T>
struct ArNode {
    T key = T();
};

template<typename T>
class MyArray {
 public:
    ArNode<T>* data;
    int size;
    int capacity; 

    MyArray() : data(new ArNode<T>[10]), size(0), capacity(10) {}


    T& operator[](int index) {
        if (index < 0 || index >= capacity) {
            throw out_of_range("Index out of array bounds");
        }
        return data[index].key;
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= capacity) {
            throw out_of_range("Index out of array bounds");
        }
        return data[index].key;
    }

    MyArray(const MyArray& other) {
        capacity = other.capacity;
        size = other.size;
        data = new ArNode<T>[capacity];
        for (int i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
    }

    MyArray& operator=(const MyArray& other) {
        if (this == &other) return *this;
        delete[] data;
        capacity = other.capacity;
        size = other.size;
        data = new ArNode<T>[capacity];
        for (int i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }



    explicit MyArray(int init_cap) {
        data = new ArNode<T>[init_cap];
        size = 0;
        capacity = init_cap;
    }

    ~MyArray() {
        delete[] data;
        data = nullptr;
        size = 0;
        capacity = 1;
    }

    void resize() {
        int newCap = capacity * 2;
        if (newCap == 0) newCap = 1;
        ArNode<T>* newData = new ArNode<T>[newCap];

        // Копируем только если есть что копировать
        if (size > 0) {
            for (int i = 0; i < capacity; i++) {
                newData[i] = data[i];
            }
        }

        if (!(data == nullptr)) {
            delete[] data;
        }
        data = newData;
        capacity = newCap;
    }

    void MPUSH_back(T key) {                 // O(1)
        if (size >= capacity) {
            resize();
        }
        data[size].key = key;
        size++;

    }


    void MPUSH_index(int index, T key) {     // O(N)
        if (index > capacity || index < 0) {
            throw invalid_argument("Array index out od bounds");
        }

        if (size >= capacity) {
            resize();
        }

        for (int i = capacity - 1; i >= index; i--) {
            data[i + 1] = data[i];
        }
        data[index].key = key;
        size++;
    }

    T MGET(int index) const {             // O(1)
        if (index >= capacity || index < 0) {
            throw invalid_argument("Array index out od bounds");
        }
        return data[index].key;
    }

    void MDEL(int index) {       // O(N)
        if (index >= capacity || index < 0) {
            throw invalid_argument("Array index out od bounds");
        }
        for (int i = index; i < capacity - 1; i++) {
            data[i] = data[i + 1];
        }
        data[capacity - 1].key = T();
        size--;
    }

    void MSWAP(int index, T swapkey) {
        if (index >= capacity || index < 0) {
            throw invalid_argument("Array index out od bounds");
        }
        data[index].key = swapkey;
    }

    void PRINT() const {
        for (int i = 0; i < capacity; i++) {
            cout << data[i].key << ' ';
        }
        cout << endl;
    }

    int msize() const {
        return size;
    }

    void saveText(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for writing: " + filename);
        }

        file << size << " " << capacity << "\n";

        for (int i = 0; i < capacity; ++i) {
            file << data[i].key << " ";
        }
        
        file.close();
    }

    void loadText(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for reading: " + filename);
        }

        int newSize, newCapacity;
        if (!(file >> newSize >> newCapacity)) {
            throw runtime_error("Error reading header info");
        }

        // Очищаем текущие данные
        delete[] data;

        size = newSize;
        capacity = newCapacity;
        data = new ArNode<T>[capacity];

        for (int i = 0; i < capacity; ++i) {
            if (!(file >> data[i].key)) {
                data[i].key = T(); 
            }
        }

        file.close();
    }

 
    void saveBinary(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for writing: " + filename);
        }

        // Записываем size и capacity
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));

        // Записываем массив данных
        for (int i = 0; i < capacity; ++i) {
            file.write(reinterpret_cast<const char*>(&data[i].key), sizeof(T));
        }

        file.close();
    }

    void loadBinary(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for reading: " + filename);
        }

        int newSize, newCapacity;
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));
        file.read(reinterpret_cast<char*>(&newCapacity), sizeof(newCapacity));

        delete[] data;
        size = newSize;
        capacity = newCapacity;
        data = new ArNode<T>[capacity];

        for (int i = 0; i < capacity; ++i) {
            file.read(reinterpret_cast<char*>(&data[i].key), sizeof(T));
        }

        file.close();
    }
};