#pragma once

#include "List_header.hpp"
#include "MyArray.hpp"
#include "MyFList.hpp"



template<typename T>
struct HashNode{
    T key;
    T value;
    HashNode(T k, T v) : key(k), value(v) {}
    HashNode() = default;

    bool operator==(const HashNode& other) const {
        return (key == other.key && value == other.value);
    }
};

template<typename T>
ostream& operator<<(ostream& os, HashNode<T> hn) {
    os << hn.key << ' ' << hn.value << "  ";
    return os;
}

template<typename T>
class TableHash{
 private:
    MyArray<ForwardList<HashNode<T>>> buckets;
    size_t countItems = 0;
    size_t bucketsThersSmtng = 0;
    const double loadFactor = 0.75;

    size_t Hash(int key) const {
        size_t hash = 0;
        while (key > 0) {
            hash ^= (key & 0xFF);   // берём младший байт и XOR с хэшем
            key >>= 8;              // сдвигаем на байт вправо
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

    size_t get_bucket_index(const T& key) const {
        return Hash(key) % buckets.capacity;
    }

    void rehash() {
        MyArray<ForwardList<HashNode<T>>> newBuckets{buckets.capacity * 2};

        for (int i = 0; i < buckets.capacity; i++) {
            for (int j = 0; j < buckets.data[i].key.size; j++) {
                HashNode<T> element = FGET_key(buckets.data[i].key, j);
                size_t newIndex = Hash(element.key) % newBuckets.capacity;
                FPUSH_back(newBuckets.data[newIndex].key, element);
            }
        }
        resize(buckets);
        for (int i = 0; i < buckets.capacity; i++) {
            buckets.data[i].key = newBuckets.data[i].key;
        }
    }

 public:
    explicit TableHash(int initcap) :
             buckets(MyArray<ForwardList<HashNode<T>>>(initcap)) {}
    TableHash() : buckets(MyArray<ForwardList<HashNode<T>>>(10)) {}

    bool HT_AT(const T& key) const {
        if (buckets.size == 0) return false;

        size_t index = get_bucket_index(key);
        for (int i = 0; i < buckets.data[index].key.size; i++) {
            if (FGET_key(buckets.data[index].key, i).key == key) {
                return true;
            }
        }

        return false;
    }

    void HTADD(const T& key, const T& value) {
        if (HT_AT(key)) {
            return;
        }

        if (static_cast<double>(countItems)
             / static_cast<double>(buckets.capacity) >= loadFactor) {
            rehash();
        }

        size_t index = get_bucket_index(key);

        if (buckets.data[index].key.size == 0) {
            bucketsThersSmtng++;
        }

        HashNode<T> node{key, value};
        FPUSH_back(buckets.data[index].key, node);
        countItems++;
        buckets.size++;

        if (static_cast<double>(bucketsThersSmtng) 
            / static_cast<double>(buckets.capacity) >= 0.9) {
            rehash();
        }
    }

    T GET(const T& key) {
        size_t index = get_bucket_index(key);
        for (int i = 0; i < buckets.data[index].key.size; i++) {
            HashNode<T> element = FGET_key(buckets.data[index].key, i);
            if (element.key == key) {
                return element.value;
            }
        }
        return T();
    }

    void HTDEL(const T& key) {
        if (countItems == 0) return;

        size_t index = get_bucket_index(key);
        if (buckets.data[index].key.size == 0) {
            return;
        }
        if (buckets.data[index].key.size == 1) {
            bucketsThersSmtng--;
        }

        T val = T();

        for (int i = 0; i < buckets.data[index].key.size; i++) {
            HashNode<T> element = FGET_key(buckets.data[index].key, i);
            if (element.key == key) {
                val = element.value;
            }
        }
        if (val == T()) {
            return;
        }
        HashNode<T> nd{key, val};
        FDEL_val(buckets.data[index].key, nd);
    }


    size_t size() const {
        return countItems;
    }

    bool empty() const {
        return countItems == 0;
    }

    void PRINT(){
        for (int i = 0; i < buckets.capacity; i++) {
            if (buckets.data[i].key.head != nullptr) {
                ::PRINT(buckets.data[i].key);
            }
        }
        cout << endl;
    }
};

