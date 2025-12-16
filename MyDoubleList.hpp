#pragma once

#include "List_header.hpp"


template<typename T>
struct DLNode {
    T key;
    DLNode* prev;
    DLNode* next;
    DLNode() : key(T()), prev(nullptr), next(nullptr) {}
    DLNode(T value, DLNode* prevptr, DLNode* nextptr) :
            key(value), prev(prevptr), next(nextptr) {}
};

template<typename T>
class DoubleList {
 private:
    DLNode<T>* head;
    DLNode<T>* tail;
    int size = 0;

 public:
    DoubleList() : head(nullptr), tail(head) {}

    void destroy_list() {
        if (size == 0) {
            return;
        }
        DLNode<T>* current = head;
        while (current != nullptr) {
            DLNode<T>* next = current->next;
            delete current;
            current = next;
        }
        size = 0;
    }

    ~DoubleList() {
        destroy_list();
    }

    void size_check() {
        if (size == 2) {
            tail = head -> next;
        }
        if (size == 1) {
            tail = head;
        }
    }

    DLNode<T>* LGET(T key) {        // O(N)
        DLNode<T>* target = head;
        while (target != nullptr && target->key != key) {
            target = target -> next;
        }
        if (target == nullptr) {
            throw invalid_argument("There is no element");
        }
        return target;
    }

    DLNode<T>* LGET_index(int index) {
        DLNode<T>* ptr;

        if (index > size / 2) {
            ptr = tail;
            for (int i = size - 1; i > index; i--) {
                ptr = ptr -> prev;
            }
        } else if (index <= size / 2) {
            ptr = head;
            for (int i = 0; i < index; i++) {
                ptr = ptr -> next;
            }
        } else {
            throw out_of_range("double list inex out of bouds");
        }
        return ptr;
    }

    T LGET_key(int index) {
        return LGET_index(index) -> key;
    }

    void LPUSH_next(int index, T key) {    // O(1)
        try {
            DLNode<T>* ptr = LGET_index(index);
            DLNode<T>* newNode = new DLNode<T>;
            newNode -> key = key;
            newNode -> next = ptr -> next;
            ptr -> next -> prev = newNode;
            ptr -> next = newNode;
            newNode -> prev = ptr;
            size++;
            size_check();
        } catch (exception& error) {
            cerr << error.what() << endl;
        }
    }


    void LPUSH_prev(int index, T key) {   // O(1)
        try {
            DLNode<T>* ptr = LGET_index(index);
            DLNode<T>* newNode = new DLNode<T>;
            newNode -> key = key;
            newNode -> next = ptr;
            ptr -> prev -> next = newNode;
            newNode -> prev = ptr -> prev;
            ptr -> prev = newNode;
            size++;
            size_check();
        } catch (exception& error) {
            cerr << error.what() << endl;
        }
    }

    void LPUSH_front(T key) {        // O(1)
        DLNode<T>* newNode = new DLNode<T>;
        newNode -> key = key;
        newNode -> prev = nullptr;
        newNode -> next = head;
        if (head != nullptr) {
            head->prev = newNode;
        } else {
            tail = newNode;
        }
        head = newNode;
        size++;
        size_check();
    }


    void LPUSH_back(T key) {       // O(1)
        DLNode<T>* newNode = new DLNode<T>;
        newNode -> key = key;
        newNode -> next = nullptr;

        if (head == nullptr) {
            newNode -> prev = nullptr;
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        size++;
        size_check();
    }


    void LDEL_next(int index) {     // O(1)
        try {
            DLNode<T>* ptr = LGET_index(index);
            DLNode<T>* deleteNode = ptr -> next;
            if (deleteNode == tail) {
                ptr -> next = nullptr;
                tail = ptr;
                delete deleteNode;
                size--;
                size_check();

            } else {
                ptr -> next = deleteNode -> next;
                ptr -> next -> prev = ptr;
                delete deleteNode;
                size--;
                size_check();
            }
        } catch (exception& error) {
            cerr << error.what() << endl;
        }
    }

    void LDEL_prev(int index) {   // O(1)
        try {
            DLNode<T>* ptr = LGET_index(index);
            DLNode<T>* deleteNode = ptr -> prev;
            if (deleteNode == head) {
                head = deleteNode -> next;
                head -> prev = nullptr;
                delete deleteNode;
                size--;
                size_check();

            } else {
                deleteNode -> prev -> next = ptr;
                ptr -> prev = deleteNode -> prev;
                size--;
                delete deleteNode;
                size_check();
            }
        } catch (exception& error) {
            cerr << error.what() << endl;
        }
    }

    void LDEL_front() {              // O(1)
        if (head == nullptr) return;
        DLNode<T>* deleteNode = head;
        head = head -> next;

        if (head != nullptr) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }

        delete deleteNode;
        size--;
        size_check();
    }

    void LDEL_back() {       // O(1)
        if (tail == nullptr) return;  // список пуст

        DLNode<T>* deleteNode = tail;

        if (tail -> prev != nullptr) {
            tail = tail -> prev;
            tail -> next = nullptr;
        } else {
            head = nullptr;
            tail = nullptr;
        }

        delete deleteNode;
        size--;
        size_check();
    }

    void LDEL_val(T key) {      // O(N)
        DLNode<T>* deleteNode = head;
        while (deleteNode -> key != key) {
            deleteNode = deleteNode -> next;
        }
        if (deleteNode == head) {
            head = deleteNode -> next;
            delete deleteNode;
            size--;
            size_check();
            return;

        } else if (deleteNode == tail) {
            deleteNode -> prev -> next = nullptr;
            delete deleteNode;
            size--;
            size_check();
            return;
        }

        deleteNode -> prev -> next = deleteNode -> next;
        deleteNode -> next -> prev = deleteNode -> prev;
        delete deleteNode;
        size--;
        size_check();
    }

    void PRINT() const {
        DLNode<T>* ptr = head;
        while (ptr) {
            cout << ptr -> key << ' ';
            ptr = ptr -> next;
        }
        cout << endl;
    }

    void PRINT_reverse_dl() const {
        DLNode<T>* ptr = tail;
        while (ptr) {
            cout << ptr -> key << " ";
            ptr = ptr -> prev;
        }
        cout << endl;
    }
    // запись списка в файл
    void dlist_write_file(const string& filename) const {
        ofstream file(filename);
        if (file.is_open()) {
            file << size << ' ';
            DLNode<T>* current = head;
            while (current) {
                file << current -> key << ' ';
                current = current -> next;
            }
        }
        file.close();
    }

    // чтение из файла
    void dlist_read_file(const string& filename){
        destroy_list();
        ifstream file(filename);
        if (is_file_empty(filename)) {
            return;
        }
        head = nullptr;

        int listsize;
        file >> listsize;
        for (int i = 0; i < listsize; i++) {
            T data;
            file >> data;
            LPUSH_back(data);
        }
        file.close();
    }
};
