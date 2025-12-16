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
        size = 0;
    }

 public:
    MyQueue() : head(nullptr), tail(head) {}

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

    void get_tail() {
        return tail -> key;
    }

    void queue_write_file(const string& filename) const {
        ofstream file(filename);    // запись стека в файл
        if (file.is_open()) {
            file << size << ' ';
            QNode<T>* current = q.head;
            while (current) {
                file << current -> key << ' ';
                current = current->next;
            }
        }
        file.close();
    }


    void queue_read_file(const string& filename) {
        destroy_queue();    // чтение из файла
        ifstream file(filename);
        if (is_file_empty(filename)) {
            return;
        }
        head = nullptr;
        tail = head;

        int listsize;
        file >> listsize;
        for (int i = 0; i < listsize; i++) {
            T data;
            file >> data;
            QPUSH(data);
        }
        file.close();
    }
};