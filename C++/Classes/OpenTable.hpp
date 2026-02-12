#pragma once

#include "List_header.hpp"
#include "MyArray.hpp"


template<typename Key, typename Value>
struct OTNode{
    Key key;
    Value value;
    OTNode() = default;
    OTNode(Key k, Value v) : key(k), value(v) {}
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

    // Сохранение в текстовый файл
    bool saveText(const string& filename) const {
        std::ofstream out(filename);
        if (!out.is_open()) {
            return false;
        }

        // Записываем метаданные
        out << countItems << '\n';
        out << buckets.capacity << '\n';
        out << bucketsThersSmtng << '\n';

        // Записываем пары ключ-значение
        for (int i = 0; i < buckets.capacity; i++) {
            if (buckets.data[i].key.key != Key()) {
                out << buckets.data[i].key.key << '\n';
                out << buckets.data[i].key.value << '\n';
            }
        }

        out.close();
        return true;
    }

    // Загрузка из текстового файла
    bool loadText(const string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            return false;
        }

        size_t newCountItems, newCapacity, newBucketsThersSmtng;
        in >> newCountItems;
        in >> newCapacity;
        in >> newBucketsThersSmtng;
        in.ignore(); // пропускаем перевод строки

        // Пересоздаем таблицу с новой емкостью
        buckets = MyArray<OTNode<Key, Value>>(newCapacity);
        countItems = 0;
        bucketsThersSmtng = 0;

        // Читаем пары ключ-значение и вставляем
        for (size_t i = 0; i < newCountItems; i++) {
            Key key;
            Value val;
            in >> key;
            in >> val;
            in.ignore(); 
            insert(key, val);
        }

        in.close();
        return true;
    }


    // Вспомогательные функции для записи в бинарный файл
    template<typename T>
    static void writeBinary(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    static void writeBinary(std::ofstream& out, const string& str) {
        size_t length = str.size();
        out.write(reinterpret_cast<const char*>(&length), sizeof(length));
        out.write(str.c_str(), length);
    }

    // Вспомогательные функции для чтения из бинарного файла
    template<typename T>
    static void readBinary(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    static void readBinary(std::ifstream& in, string& str) {
        size_t length;
        in.read(reinterpret_cast<char*>(&length), sizeof(length));
        str.resize(length);
        in.read(&str[0], length);
    }

    // Сохранение в бинарный файл
    bool saveBinary(const string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out.is_open()) {
            return false;
        }
        writeBinary(out, countItems);
        writeBinary(out, buckets.capacity);
        writeBinary(out, bucketsThersSmtng);

        // Записываем пары ключ-значение
        for (int i = 0; i < buckets.capacity; i++) {
            if (buckets.data[i].key.key != Key()) {
                writeBinary(out, buckets.data[i].key.key);
                writeBinary(out, buckets.data[i].key.value);
            }
        }

        out.close();
        return true;
    }

    // Загрузка из бинарного файла
    bool loadBinary(const string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            return false;
        }

        size_t newCountItems, newBucketsThersSmtng;
        int newCapacity;
        readBinary(in, newCountItems);
        readBinary(in, newCapacity);
        readBinary(in, newBucketsThersSmtng);

        // Пересоздаем таблицу с новой емкостью
        buckets = MyArray<OTNode<Key, Value>>(newCapacity);
        countItems = 0;
        bucketsThersSmtng = 0;

        // Читаем пары ключ-значение и вставляем
        for (size_t i = 0; i < newCountItems; i++) {
            Key key;
            Value val;
            readBinary(in, key);
            readBinary(in, val);
            insert(key, val);
        }

        in.close();
        return true;
    }
};


