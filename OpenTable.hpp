#pragma once

#include "List_header.hpp"
#include "MyArray.hpp"


template<typename Key, typename Value>
struct OTNode{
    Key key;
    Value value;
    OTNode() = default;
    OTNode(Key k, Value v) : key(k), value(v){}
};


template<typename Key, typename Value>
class OpenTable{
 private:
    MyArray<OTNode<Key, Value>> buckets{10};
    size_t countItems = 0;
    size_t bucketsThersSmtng = 0;

    size_t Hash(int key) const {
        size_t hash = 0;
        while (key > 0) {
            hash ^= (key & 0xFF);     // берём младший байт и XOR с хэшем
            key >>= 8;            // сдвигаем на байт вправо
        }
        return hash % buckets.capacity;
    }

    size_t Hash(const string& key) const {
        size_t sum = 0;
        for (char c : key) {
            sum += static_cast<uint8_t>(c);
        }

        uint32_t result = (sum % (buckets.capacity - 1)) + 1;

        // Делаем результат нечётным, если размер таблицы чётный
        if (buckets.capacity % 2 == 0 && result % 2 == 0) {
            result++;
        }

        return result;
    }

    size_t get_bucket_index(const Key& key) const {
        return Hash(key) % buckets.capacity;
    }

    void rehash() {
        MyArray<OTNode<Key, Value>> oldBuckets = buckets;
        buckets = MyArray<OTNode<Key, Value>>(oldBuckets.capacity * 2);
        countItems = 0;

        for (int i = 0; i < oldBuckets.capacity; i++) {
            Key key = oldBuckets.data[i].key.key;
            Value val = oldBuckets.data[i].key.value;
            if (key != Key()) {
                insert(key, val);   // повторно вставляем все элементы
            }
        }
    }

 public:
    explicit OpenTable(int initcap) :
        buckets(MyArray<OTNode<Key, Value>>(initcap)) {}

    OpenTable() : buckets(MyArray<OTNode<Key, Value>>(10)) {}

    bool find(const Key& key) const {
        if (countItems == 0) return false;

        size_t index = get_bucket_index(key);
        size_t start = index;

        while (buckets.data[index].key.key != Key()) {
            if (buckets.data[index].key.key == key) return true;
            index = (index + 1) % buckets.capacity;
            if (index == start) break;
        }

        return false;
    }

    void insert(const Key& key, const Value& val) {
        if (find(key)) return;  // уже есть

        size_t index = get_bucket_index(key);
        size_t start = index;

        // Линейное пробирование
        while (buckets.data[index].key.key != Key()) {
            index = (index + 1) % buckets.capacity;
            if (index == start) {
                // Таблица переполнена — расширяем
                rehash();
                insert(key, val);
                return;
            }
        }

        if (buckets.data[index].key.key == Key()) {
            bucketsThersSmtng++;
        }
        buckets.data[index].key.key = key;
        buckets.data[index].key.value = val;
        countItems++;

        // если загрузка > 0.75 — увеличиваем таблицу
        if (countItems * 4 > buckets.capacity * 3) {
            rehash();
        }

        if (static_cast<double>(bucketsThersSmtng)
            / static_cast<double>(buckets.capacity) >= 0.9) {
            rehash();
        }
    }

    void erase(const Key& key) {
        if (countItems == 0) return;

        size_t index = get_bucket_index(key);

        while (buckets.data[index].key.key != key) {
            index = (index + 1) % buckets.capacity;
        }

        buckets.data[index].key.key = Key();
        buckets.data[index].key.value = Value();
        countItems--;
    }


    size_t size() const {
        return countItems;
    }

    bool empty() const {
        return countItems == 0;
    }

    int get_cap() const {
        return buckets.capacity;
    }

    Key get_key(int i) const {
        return buckets.data[i].key.key;
    }

    Value get_value(Key k) const {
        size_t index = get_bucket_index(k);
        while (buckets.data[index].key.key != k) {
            index = (index + 1) % buckets.capacity;
        }

        return buckets.data[index].key.value;
    }

    void SPRINT(){
        for (int i = 0; i < buckets.capacity; i++) {
            if (buckets.data[i].key.key != Key()) {
                cout << buckets.data[i].key.key << ' ' << buckets.data[i].key.value << endl;
            }
        }
        cout << endl;
    }
};


