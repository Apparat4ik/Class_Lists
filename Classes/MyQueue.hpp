#pragma once

#include "List_header.hpp"

template<typename T>
struct QNode{
    T key;
    QNode<T>* next;

    QNode() : key(T()), next(nullptr) {}
    QNode(T value, QNode* ptr) : key(value), next(ptr){}
};

template<typename T>
class MyQueue{
 private:
    QNode<T>* head;
    QNode<T>* tail;
    int size = 0;

    void destroy_queue() {
        while (head) {
            QNode<T>* delNode = head;
            head = head -> next;
            delete delNode;
        }
        head = nullptr;
        size = 0;
    }

 public:
    MyQueue() : head(nullptr), tail(nullptr) {}

    MyQueue(const MyQueue& other) : head(nullptr), tail(nullptr), size(0) {
        if (other.head == nullptr) return;

        QNode<T>* temp = other.head;
        while (temp) {
            QPUSH(temp->key);
            temp = temp->next;
        }
    }

    MyQueue& operator=(const MyQueue& other) {
        if (this != &other) {
            destroy_queue();
            QNode<T>* temp = other.head;
            while (temp) {
                QPUSH(temp->key);
                temp = temp->next;
            }
        }
        return *this;
    }

    ~MyQueue() {
        destroy_queue();
    }

    void QPUSH(T key) {    // O(1)
        QNode<T>* newNode = new QNode<T>;
        newNode -> key = key;
        if (head == nullptr) {
            head = newNode;
            newNode -> next = nullptr;
            tail = head;
        } else {
            tail -> next = newNode;
            newNode -> next = nullptr;
            tail = newNode;
        }
        size++;
    }

    void QPOP() {   // O(1)
        QNode<T>* delNode = head;
        head = head -> next;
        delete delNode;
        size--;
    }

    void PRINT() const {
        QNode<T>* ptr = head;
        while (ptr) {
            cout << ptr -> key << " ";
            ptr = ptr -> next;
        }
        cout << endl;
    }

    T get_head() const {     // O(1)
        return head -> key;
    }

    bool QEMPTY() const {
        if (head == nullptr) {
            return true;
        }
        return false;
    }

    T get_tail() {
        return tail -> key;
    }

    size_t qsize() const {
        return size;
    }

    // Сохранение в текстовый файл
    void serialize_text(const string& filename) const {
        ofstream fout(filename);
        if (!fout.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }
        
        fout << size << endl;
        QNode<T>* ptr = head;
        while (ptr) {
            fout << ptr->key << endl;
            ptr = ptr->next;
        }
        
        fout.close();
    }
    
    // Загрузка из текстового файла
    void deserialize_text(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }
        
        destroy_queue();
        
        int count;
        fin >> count;
        
        T value;
        for (int i = 0; i < count; i++) {
            fin >> value;
            QPUSH(value);
        }
        
        fin.close();
    }
    
    
    // Сохранение в бинарный файл
    void serialize_binary(const string& filename) const {
        ofstream fout(filename, ios::binary | ios::out);
        if (!fout.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }
        

        fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
        

        QNode<T>* ptr = head;
        while (ptr) {
            fout.write(reinterpret_cast<const char*>(&ptr->key), sizeof(T));
            ptr = ptr->next;
        }
        
        fout.close();
    }
    
    // Загрузка из бинарного файла
    void deserialize_binary(const string& filename) {
        ifstream fin(filename, ios::binary | ios::in);
        if (!fin.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }
        
        destroy_queue();
        

        int count;
        fin.read(reinterpret_cast<char*>(&count), sizeof(count));
        

        T value;
        for (int i = 0; i < count; i++) {
            fin.read(reinterpret_cast<char*>(&value), sizeof(T));
            QPUSH(value);
        }
        
        fin.close();
    }
};