#pragma once

#include <string>
#include "List_header.hpp" 


template<typename T>
struct StNode{
    T key;
    StNode* next;
    StNode() : key(T()), next(nullptr) {}
    StNode(T value, StNode* ptr) : key(value), next(ptr){}
};

template<typename T>
class MyStack{
 private:
    StNode<T>* head = nullptr;
    size_t size = 0;

    void destroy_stack() {
        StNode<T>* current = head;
        while (current != nullptr) {
            StNode<T>* next = current->next;
            delete current;
            current = next;
        }
        size = 0;
    }

 public:
    MyStack() : head(nullptr) {}

    explicit MyStack(initializer_list<T> list) :
        size(list.size()) {
        for (auto item : list) {
            if (head == nullptr) {
                head = new StNode<T>(item, nullptr);
            } else {
                auto newNode = new StNode<T>{item, head};
                head = newNode;
            }
        }
    }

    ~MyStack() {
        destroy_stack();
    }

    void SPUSH(T key) {   // O(1)
        auto newNode = new StNode<T>;
        newNode -> key = key;
        newNode -> next = head;
        head = newNode;
        size++;
    }

    auto SPOP() -> T {    // O(1)
        StNode<T>* delNode = head;
        head = head -> next;
        T Nodekey = delNode -> key;
        delete delNode;
        size--;
        return Nodekey;
    }

    void PRINT() const {
        StNode<T>* ptr = head;
        while (ptr) {
            cout << ptr -> key << " ";
            ptr = ptr -> next;
        }
        cout << endl;
    }

    auto SGET_head() const  -> T {  // O(1)
        return head -> key;
    }

    void stack_write_file(const string& filename) const {
        ofstream file(filename);        // запись стека в файл
        if (file.is_open()) {
            file << size << ' ';
            StNode<T>* current = head;
            while (current) {
                file << current -> key << ' ';
                current = current->next;
            }
        }
        file.close();
    }

    void stack_read_file(const string& filename){   // чтение из файла
        destroy_stack();
        ifstream file(filename);
        if (is_file_empty(filename)) {
            return;
        }
        head = nullptr;

        int listsize;
        file >> listsize;

        MyStack<T> tempStack;
        for (int i = 0; i < listsize; i++) {
            T data;
            file >> data;
            SPUSH(tempStack, data);
        }

        while (tempStack.head) {
            SPUSH(SGET_head(tempStack));
            tempStack.SPOP();
        }

        file.close();
    }
};



