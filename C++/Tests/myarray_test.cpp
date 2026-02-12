#define BOOST_TEST_MODULE MyArrayTests
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <string>
#include <cstdio>

#include "Classes/MyArray.hpp"

BOOST_AUTO_TEST_SUITE(MyArraySuite)

// 1. Тест конструктора по умолчанию и метода MSIZE
BOOST_AUTO_TEST_CASE(test_initialization) {
    MyArray<int> arr;

    BOOST_CHECK_EQUAL(arr.capacity, 10);
    BOOST_CHECK_EQUAL(arr.msize(), 0);
}

// 2. Тест добавления элементов в конец (MPUSH_back)
BOOST_AUTO_TEST_CASE(test_push_back) {
    MyArray<int> arr;
    arr.MPUSH_back(10);
    arr.MPUSH_back(20);

    BOOST_CHECK_EQUAL(arr.msize(), 2);
    BOOST_CHECK_EQUAL(arr.MGET(0), 10);
    BOOST_CHECK_EQUAL(arr.MGET(1), 20);
}

// 3. Тест автоматического расширения (resize)
BOOST_AUTO_TEST_CASE(test_resize) {
    MyArray<int> arr;


    for (int i = 0; i < 15; ++i) {
        arr.MPUSH_back(i);
    }

    BOOST_CHECK_EQUAL(arr.msize(), 15);

    BOOST_CHECK_GE(arr.capacity, 20);
    BOOST_CHECK_EQUAL(arr.MGET(14), 14);
}

// 4. Тест получения элементов и оператора []
BOOST_AUTO_TEST_CASE(test_access) {
    MyArray<int> arr;
    arr.MPUSH_back(100);
    arr.MPUSH_back(200);


    BOOST_CHECK_EQUAL(arr.MGET(1), 200);


    BOOST_CHECK_EQUAL(arr[0], 100);


    arr[0] = 500;
    BOOST_CHECK_EQUAL(arr.MGET(0), 500);
}

// 5. Тест исключений (выход за границы)
BOOST_AUTO_TEST_CASE(test_out_of_bounds) {
    MyArray<int> arr;
    arr.MPUSH_back(1);


    BOOST_CHECK_THROW(arr.MGET(-1), std::invalid_argument);
    BOOST_CHECK_THROW(arr.MGET(100), std::invalid_argument);


    BOOST_CHECK_THROW(arr[100], std::out_of_range);
}

// 6. Тест удаления элемента (MDEL)
BOOST_AUTO_TEST_CASE(test_delete) {
    MyArray<int> arr;
    arr.MPUSH_back(10);
    arr.MPUSH_back(20);
    arr.MPUSH_back(30);


    arr.MDEL(1);

    BOOST_CHECK_EQUAL(arr.msize(), 2);
    BOOST_CHECK_EQUAL(arr.MGET(0), 10);
    BOOST_CHECK_EQUAL(arr.MGET(1), 30);
}

// 7. Тест замены (MSWAP)
BOOST_AUTO_TEST_CASE(test_swap) {
    MyArray<int> arr;
    arr.MPUSH_back(5);
    arr.MSWAP(0, 99);

    BOOST_CHECK_EQUAL(arr.MGET(0), 99);
}

// 8. Тест конструктора копирования
BOOST_AUTO_TEST_CASE(test_copy_constructor) {
    MyArray<int> arr1;
    arr1.MPUSH_back(10);
    arr1.MPUSH_back(20);

    MyArray<int> arr2 = arr1;

    BOOST_CHECK_EQUAL(arr2.msize(), 2);
    BOOST_CHECK_EQUAL(arr2.MGET(0), 10);

    arr2.MSWAP(0, 999);
    BOOST_CHECK_EQUAL(arr2.MGET(0), 999);
    BOOST_CHECK_EQUAL(arr1.MGET(0), 10);
}

// 9. Тест текстовой сериализации
BOOST_AUTO_TEST_CASE(test_serialization_text) {
    std::string filename = "test_data.txt";
    
    // Подготовка данных
    MyArray<int> original;
    original.MPUSH_back(111);
    original.MPUSH_back(222);
    original.MPUSH_back(333);
    
    // Сохранение
    original.saveText(filename);

    // Загрузка в новый массив
    MyArray<int> loaded;
    loaded.loadText(filename);

    // Проверки
    BOOST_CHECK_EQUAL(loaded.msize(), original.msize());
    BOOST_CHECK_EQUAL(loaded.capacity, original.capacity);
    
    BOOST_CHECK_EQUAL(loaded.MGET(0), 111);
    BOOST_CHECK_EQUAL(loaded.MGET(1), 222);
    BOOST_CHECK_EQUAL(loaded.MGET(2), 333);

    // Удаляем временный файл
    std::remove(filename.c_str());
}

// 10. Тест бинарной сериализации
BOOST_AUTO_TEST_CASE(test_serialization_binary) {
    std::string filename = "test_data.bin";

    // Подготовка данных (больше элементов, чтобы проверить массив)
    MyArray<int> original;
    for(int i = 0; i < 50; ++i) {
        original.MPUSH_back(i * 10);
    }

    // Сохранение
    original.saveBinary(filename);

    // Загрузка
    MyArray<int> loaded;
    loaded.loadBinary(filename);

    // Проверки
    BOOST_CHECK_EQUAL(loaded.msize(), original.msize());

    // Проверяем первый, последний и случайный элемент в середине
    BOOST_CHECK_EQUAL(loaded.MGET(0), 0);
    BOOST_CHECK_EQUAL(loaded.MGET(25), 250);
    BOOST_CHECK_EQUAL(loaded.MGET(49), 490);

    // Удаляем временный файл
    std::remove(filename.c_str());
}

// 11. Тест загрузки несуществующего файла
BOOST_AUTO_TEST_CASE(test_load_missing_file) {
    MyArray<int> arr;
    BOOST_CHECK_THROW(arr.loadText("non_existent_file.txt"), std::runtime_error);
    BOOST_CHECK_THROW(arr.loadBinary("non_existent_file.bin"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
