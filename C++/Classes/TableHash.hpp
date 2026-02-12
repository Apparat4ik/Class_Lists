#pragma once

#include "List_header.hpp"
#include "MyArray.hpp"
#include "MyFList.hpp"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;


template<typename Key, typename Value>
struct HashNode{
    Key key;
    Value value;
    HashNode(Key k, Value v) : key(k), value(v) {}
    HashNode() = default;

    bool operator==(const HashNode& other) const {
        return (key == other.key && value == other.value);
    }

    bool operator!=(const HashNode& other) const {
        return (key == other.key && value != other.value);
    }
};

template<typename Key, typename Value>
ostream& operator<<(ostream& os, HashNode<Key, Value> hn) {
    os << hn.key << ' ' << hn.value << "  ";
    return os;
}

template<typename Key, typename Value>
class TableHash{
 private:
    MyArray<ForwardList<HashNode<Key, Value>>> buckets;
    size_t countItems = 0;
    size_t bucketsThersSmtng = 0;
    const double loadFactor = 0.75;



    size_t Hash(int key) const {
        size_t hash = 0;
        int tempKey = key;
        while (tempKey > 0) {
            hash ^= (tempKey & 0xFF);
            tempKey >>= 8;
        }
        return hash % buckets.capacity;
    }


    size_t Hash(const string& key) const {
        size_t sum = 0;
        for (char c : key) {
            sum += static_cast<uint8_t>(c);
        }
        uint32_t result = (sum % (buckets.capacity - 1)) + 1;
        if (buckets.capacity % 2 == 0 && result % 2 == 0) {
            result++;
        }
        return result;
    }

    size_t get_bucket_index(const Key& key) const {
        return Hash(key) % buckets.capacity;
    }

    void rehash() {
        MyArray<ForwardList<HashNode<Key, Value>>> oldBuckets = buckets;
        
 
        int newCapacity = buckets.capacity * 2;
        buckets = MyArray<ForwardList<HashNode<Key, Value>>>(newCapacity);
        
        countItems = 0;
        bucketsThersSmtng = 0;

        for (int i = 0; i < oldBuckets.capacity; i++) {
            for (int j = 0; j < oldBuckets[i].flsize(); j++) {
                HashNode<Key, Value> element = oldBuckets[i].FGET_key(j);
                HTADD(element.key, element.value);
            }
        }
    }

 public:
    explicit TableHash(int initcap) :
             buckets(MyArray<ForwardList<HashNode<Key, Value>>>(initcap)) {}
    
    TableHash() : buckets(MyArray<ForwardList<HashNode<Key, Value>>>(10)) {}

    void Clear() {
        buckets = MyArray<ForwardList<HashNode<Key, Value>>>(10);
        countItems = 0;
        bucketsThersSmtng = 0;
    }

    bool HT_AT(const Key& key) const {
        if (countItems == 0) return false;

        size_t index = get_bucket_index(key);
        for (int i = 0; i < buckets[index].flsize(); i++) {
            if (buckets[index].FGET_key(i).key == key) {
                return true;
            }
        }
        return false;
    }

    void HTADD(const Key& key, const Value& value) {
        if (HT_AT(key)) {
            return; 
        }

        if (static_cast<double>(countItems)
             / static_cast<double>(buckets.capacity) >= loadFactor) {
            rehash();
        }

        size_t index = get_bucket_index(key);

        if (buckets[index].flsize() == 0) {
            bucketsThersSmtng++;
        }

        HashNode<Key, Value> node{key, value};
        buckets[index].FPUSH_back(node);
        countItems++;

        if (static_cast<double>(bucketsThersSmtng) 
            / static_cast<double>(buckets.capacity) >= 0.9) {
            rehash();
        }
    }

    Value GET(const Key& key) {
        size_t index = get_bucket_index(key);
        for (int i = 0; i < buckets[index].flsize(); i++) {
            HashNode<Key, Value> element = buckets[index].FGET_key(i);
            if (element.key == key) {
                return element.value;
            }
        }
        return Value();
    }

    void HTDEL(const Key& key) {
        if (countItems == 0) return;

        size_t index = get_bucket_index(key);
        if (buckets[index].flsize() == 0) return;

        Value val = Value();
        bool found = false;

        for (int i = 0; i < buckets[index].flsize(); i++) {
            HashNode<Key, Value> element = buckets[index].FGET_key(i);
            if (element.key == key) {
                val = element.value;
                found = true;
                break;
            }
        }

        if (!found) return;

        HashNode<Key, Value> nd{key, val};
        buckets[index].FDEL_val(nd);
        countItems--;

        if (buckets[index].flsize() == 0) {
            bucketsThersSmtng--;
        }
    }

    size_t size() const { return countItems; }
    bool empty() const { return countItems == 0; }

    void PRINT(){
        for (int i = 0; i < buckets.capacity; i++) {
            if (buckets[i].flsize() > 0) {
                buckets[i].PRINT();
            }
        }
        cout << endl;
    }

    
    void SaveText(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Error opening file for text save");

        file << countItems << endl;

        for (int i = 0; i < buckets.capacity; i++) {
            int listSize = buckets[i].flsize();
            for (int j = 0; j < listSize; j++) {
                HashNode<Key, Value> node = buckets[i].FGET_key(j);
                file << node.key << " " << node.value << endl;
            }
        }
        file.close();
    }

    void LoadText(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Error opening file for text load");

        Clear();
        size_t itemsFromFile;
        file >> itemsFromFile;

        Key k;
        Value v;
        for (size_t i = 0; i < itemsFromFile; i++) {
            file >> k >> v;
            HTADD(k, v);
        }
        file.close();
    }


 private:
    template<typename U>
    void bin_write(ofstream& out, const U& val) {
        out.write(reinterpret_cast<const char*>(&val), sizeof(U));
    }

    void bin_write(ofstream& out, const string& val) {
        size_t len = val.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (len > 0) {
            out.write(val.c_str(), len);
        }
    }

    template<typename U>
    void bin_read(ifstream& in, U& val) {
        in.read(reinterpret_cast<char*>(&val), sizeof(U));
    }

    void bin_read(ifstream& in, string& val) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        val.resize(len);
        if (len > 0) {
            in.read(&val[0], len);
        }
    }


 public:
    void SaveBinary(const string& filename) {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) throw runtime_error("Error opening file for binary save");


        file.write(reinterpret_cast<const char*>(&countItems), sizeof(countItems));

        for (int i = 0; i < buckets.capacity; i++) {
            int listSize = buckets[i].flsize();
            for (int j = 0; j < listSize; j++) {
                HashNode<Key, Value> node = buckets[i].FGET_key(j);

                bin_write(file, node.key);
                bin_write(file, node.value);
            }
        }
        file.close();
    }

    void LoadBinary(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) throw runtime_error("Error opening file for binary load");

        Clear();

        size_t itemsFromFile;
        file.read(reinterpret_cast<char*>(&itemsFromFile), sizeof(itemsFromFile));

        for (size_t i = 0; i < itemsFromFile; i++) {
            Key k = Key();
            Value v = Value();

            bin_read(file, k);
            bin_read(file, v);

            HTADD(k, v);
        }
        file.close();
    }
};
