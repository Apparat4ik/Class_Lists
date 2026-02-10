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
        head = nullptr;
        size = 0;
    }

    void copy_from(const MyStack& other) {
        if (other.head == nullptr) {
            head = nullptr;
            size = 0;
            return;
        }

        head = new StNode<T>(other.head->key, nullptr);

        StNode<T>* currentDest = head;
        StNode<T>* currentSource = other.head->next;

        while (currentSource != nullptr) {
            currentDest->next = new StNode<T>(currentSource->key, nullptr);
            currentDest = currentDest->next;
            currentSource = currentSource->next;
        }
        size = other.size;
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

    MyStack(const MyStack& other) : size(0), head(nullptr) {
        copy_from(other);
    }


    MyStack& operator=(const MyStack& other) {
        if (this != &other) {
            destroy_stack();
            copy_from(other);
        }
        return *this;
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
        if (head == nullptr) {
            throw runtime_error("Stack is empty");
        }
        StNode<T>* delNode = head;
        if (head -> next == nullptr) {
            T val = delNode -> key;
            delete delNode;
            head = nullptr;
            size--;
            return val;
        }
        head = head -> next;
        T val = delNode -> key;
        delete delNode;
        size--;
        return val;
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
        if (head == nullptr) {
            throw runtime_error("Stack is empty");
        }
        return head -> key;
    }


    size_t stsize() {
        return size;
    }

    void serialize_text(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }

     
        StNode<T>* current = head;
        while (current != nullptr) {
            outFile << current->key << " "; 
            current = current->next;
        }
        outFile.close();
    }

   
    void deserialize_text(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }

        destroy_stack(); 

        vector<T> tempBuffer;
        T val;

        while (inFile >> val) {
            tempBuffer.push_back(val);
        }


        for (auto it = tempBuffer.rbegin(); it != tempBuffer.rend(); ++it) {
            SPUSH(*it);
        }
        inFile.close();
    }

    void serialize_binary(const string& filename) const {
        ofstream outFile(filename, ios::binary);
        if (!outFile.is_open()) {
            throw runtime_error("Cannot open file for binary writing: " + filename);
        }


        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

        StNode<T>* current = head;
        while (current != nullptr) {
            outFile.write(reinterpret_cast<const char*>(&current->key), sizeof(T));
            current = current->next;
        }
        outFile.close();
    }


    void deserialize_binary(const string& filename) {
        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open()) {
            throw runtime_error("Cannot open file for binary reading: " + filename);
        }

        destroy_stack();

        size_t file_size = 0;
        if (!inFile.read(reinterpret_cast<char*>(&file_size), sizeof(file_size))) {
            return; 
        }

        vector<T> tempBuffer(file_size);

        for(size_t i = 0; i < file_size; ++i) {
             inFile.read(reinterpret_cast<char*>(&tempBuffer[i]), sizeof(T));
        }

        for (auto it = tempBuffer.rbegin(); it != tempBuffer.rend(); ++it) {
            SPUSH(*it);
        }
        inFile.close();
    }
};



