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

 public:
    DoubleList() : head(nullptr), tail(head) {}

    DoubleList(const DoubleList& other) : head(nullptr), size(0) {
        DLNode<T>* curr = other.head;
        while (curr) {
            LPUSH_back(curr -> key);
            curr = curr -> next;
        }
    }

    DoubleList& operator=(const DoubleList& other) {
        if (this == &other) return *this;  // защита от самоприсваивания
        destroy_list();
        head = nullptr;
        size = 0;
        DLNode<T>* curr = other.head;
        while (curr) {
            LPUSH_back(curr->key);
            curr = curr -> next;
        }
        return *this;
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
        }
        if (size == 0 || index >= size) {
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
            if (ptr == tail) {
                ptr -> next = newNode;
                newNode -> prev = ptr;
                tail = newNode;
                return;
            }
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
    
    // ТЕКСТОВАЯ СЕРИАЛИЗАЦИЯ
    void serialize_text(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file for writing: " + filename);
        }

        file << size << endl;
        DLNode<T>* current = head;
        while (current != nullptr) {
            file << current->key << endl;
            current = current->next;
        }

        file.close();
    }

    // ТЕКСТОВАЯ ДЕСЕРИАЛИЗАЦИЯ
    void deserialize_text(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file for reading: " + filename);
        }

        destroy_list();
        head = nullptr;
        tail = nullptr;
        size = 0;

        int count;
        file >> count;

        T value;
        for (int i = 0; i < count; i++) {
            file >> value;
            LPUSH_back(value);
        }

        file.close();
    }

    // БИНАРНАЯ СЕРИАЛИЗАЦИЯ
    void serialize_binary(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file for binary writing: " + filename);
        }

        // Записываем размер списка
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Записываем все элементы
        DLNode<T>* current = head;
        while (current != nullptr) {
            file.write(reinterpret_cast<const char*>(&current->key), sizeof(T));
            current = current->next;
        }

        file.close();
    }

    // БИНАРНАЯ ДЕСЕРИАЛИЗАЦИЯ
    void deserialize_binary(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Cannot open file for binary reading: " + filename);
        }

        destroy_list();
        head = nullptr;
        tail = nullptr;
        size = 0;

        // Читаем размер списка
        int count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        // Читаем все элементы
        T value;
        for (int i = 0; i < count; i++) {
            file.read(reinterpret_cast<char*>(&value), sizeof(T));
            LPUSH_back(value);
        }

        file.close();
    }
};
