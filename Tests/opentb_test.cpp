#define BOOST_TEST_MODULE OpenTableTests
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <iostream>
#include "Classes/OpenTable.hpp"

BOOST_AUTO_TEST_SUITE(OpenTableTestSuite)


BOOST_AUTO_TEST_CASE(TestInitialization) {
    OpenTable<int, string> table;
    BOOST_CHECK_EQUAL(table.size(), 0);
    BOOST_CHECK(table.empty());
    BOOST_CHECK_EQUAL(table.get_cap(), 10);
}

BOOST_AUTO_TEST_CASE(TestInsertAndFind) {
    OpenTable<int, string> table;
    table.insert(1, "One");
    table.insert(5, "Five");

    BOOST_CHECK_EQUAL(table.size(), 2);
    BOOST_CHECK(!table.empty());

    BOOST_CHECK(table.find(1));
    BOOST_CHECK(table.find(5));
    BOOST_CHECK(!table.find(10));

    BOOST_CHECK_EQUAL(table.get_value(1), "One");
    BOOST_CHECK_EQUAL(table.get_value(5), "Five");
}


BOOST_AUTO_TEST_CASE(TestDuplicateInsertion) {
    OpenTable<int, string> table;
    table.insert(10, "Ten");
    table.insert(10, "TenDuplicate");

    BOOST_CHECK_EQUAL(table.size(), 1);
    BOOST_CHECK_EQUAL(table.get_value(10), "Ten");
}


BOOST_AUTO_TEST_CASE(TestStringKeys) {
    OpenTable<string, int> table;
    table.insert("apple", 100);
    table.insert("banana", 200);

    BOOST_CHECK(table.find("apple"));
    BOOST_CHECK_EQUAL(table.get_value("apple"), 100);
    BOOST_CHECK(!table.find("cherry"));
}


BOOST_AUTO_TEST_CASE(TestErase) {
    OpenTable<int, int> table;
    table.insert(1, 10);
    table.insert(2, 20);

    BOOST_CHECK_EQUAL(table.size(), 2);

    table.erase(1);

    BOOST_CHECK_EQUAL(table.size(), 1);
    BOOST_CHECK(!table.find(1));
    BOOST_CHECK(table.find(2));

    table.erase(2);
    BOOST_CHECK(table.empty());
}


BOOST_AUTO_TEST_CASE(TestRehash) {
    OpenTable<int, int> table(4);

    table.insert(1, 1);
    table.insert(2, 2);
    table.insert(3, 3);

    int initialCap = table.get_cap();
    BOOST_CHECK_EQUAL(initialCap, 4);


    table.insert(4, 4);

    BOOST_CHECK_GT(table.get_cap(), initialCap);
    BOOST_CHECK_EQUAL(table.size(), 4);

    BOOST_CHECK(table.find(1));
    BOOST_CHECK(table.find(2));
    BOOST_CHECK(table.find(3));
    BOOST_CHECK(table.find(4));
}


BOOST_AUTO_TEST_CASE(TestCollisions) {
    OpenTable<int, int> table(10);

    table.insert(10, 10);
    table.insert(20, 20);

    BOOST_CHECK(table.find(10));
    BOOST_CHECK(table.find(20));
    BOOST_CHECK_EQUAL(table.size(), 2);
}


BOOST_AUTO_TEST_CASE(TestTextSerialization) {
    OpenTable<int, string> table;
    table.insert(1, "One");
    table.insert(2, "Two");
    table.insert(100, "Hundred");

    string filename = "test_dump.txt";

 
    BOOST_CHECK(table.saveText(filename));

    OpenTable<int, string> loadedTable;
 
    BOOST_CHECK(loadedTable.loadText(filename));

 
    BOOST_CHECK_EQUAL(loadedTable.size(), 3);
    BOOST_CHECK(!loadedTable.empty());
    
    BOOST_CHECK(loadedTable.find(1));
    BOOST_CHECK(loadedTable.find(2));
    BOOST_CHECK(loadedTable.find(100));

    BOOST_CHECK_EQUAL(loadedTable.get_value(1), "One");
    BOOST_CHECK_EQUAL(loadedTable.get_value(100), "Hundred");

 
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(TestBinarySerialization) {
    OpenTable<int, string> table;
    // Вставляем данные, которые вызовут коллизии или рехеширование, если таблица мал
    for(int i = 1; i <= 5; i++) {
        table.insert(i, "Val" + std::to_string(i));
    }

    string filename = "test_dump.bin";


    BOOST_CHECK(table.saveBinary(filename));

    OpenTable<int, string> loadedTable;

    BOOST_CHECK(loadedTable.loadBinary(filename));


    BOOST_CHECK_EQUAL(loadedTable.size(), 5);
    BOOST_CHECK_EQUAL(loadedTable.get_value(1), "Val1");
    BOOST_CHECK_EQUAL(loadedTable.get_value(5), "Val5");


    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(TestEmptyTableSerialization) {
    OpenTable<int, string> table;
    string txtFile = "empty.txt";
    string binFile = "empty.bin";

    // Сохраняем пустую таблицу
    BOOST_CHECK(table.saveText(txtFile));
    BOOST_CHECK(table.saveBinary(binFile));

    OpenTable<int, string> loadedTxt;
    OpenTable<int, string> loadedBin;

    // Загружаем
    BOOST_CHECK(loadedTxt.loadText(txtFile));
    BOOST_CHECK(loadedBin.loadBinary(binFile));

    // Убеждаемся, что они остались пустыми
    BOOST_CHECK(loadedTxt.empty());
    BOOST_CHECK_EQUAL(loadedTxt.size(), 0);
    
    BOOST_CHECK(loadedBin.empty());
    BOOST_CHECK_EQUAL(loadedBin.size(), 0);

    std::remove(txtFile.c_str());
    std::remove(binFile.c_str());
}

BOOST_AUTO_TEST_CASE(TestLoadNonExistentFile) {
    OpenTable<int, string> table;
    BOOST_CHECK_EQUAL(table.loadText("non_existent_file.txt"), false);
    BOOST_CHECK_EQUAL(table.loadBinary("non_existent_file.bin"), false);
}

BOOST_AUTO_TEST_SUITE_END()