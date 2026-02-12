#define BOOST_TEST_MODULE TableHashTest
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <cstdio>
#include <fstream>
#include "Classes/TableHash.hpp" 

using namespace std;

struct FileCleanupFixture {
    ~FileCleanupFixture() {
        std::remove("test_text.txt");
        std::remove("test_bin_str_int.bin");
        std::remove("test_bin_int_str.bin");
    }
};

BOOST_FIXTURE_TEST_SUITE(HashTestSuite, FileCleanupFixture)

BOOST_AUTO_TEST_CASE(Test_Basic_Operations_MixedTypes) {
    TableHash<string, int> table(10);

    BOOST_CHECK(table.empty());

    table.HTADD("One", 1);
    table.HTADD("Two", 2);
    table.HTADD("Three", 3);

    BOOST_CHECK_EQUAL(table.size(), 3);
    BOOST_CHECK(!table.empty());

    BOOST_CHECK_EQUAL(table.GET("One"), 1);
    BOOST_CHECK_EQUAL(table.GET("Two"), 2);
    BOOST_CHECK_EQUAL(table.GET("Three"), 3);

    BOOST_CHECK(table.HT_AT("One"));
    BOOST_CHECK(!table.HT_AT("Four"));

    table.HTDEL("Two");
    BOOST_CHECK_EQUAL(table.size(), 2);
    BOOST_CHECK(!table.HT_AT("Two"));
    
    BOOST_CHECK_EQUAL(table.GET("Two"), 0);
}

BOOST_AUTO_TEST_CASE(Test_Text_Serialization) {
    TableHash<string, string> table(10);
    string filename = "test_text.txt";

    table.HTADD("apple", "red");
    table.HTADD("banana", "yellow");
    table.HTADD("lime", "green");

    table.SaveText(filename);

    TableHash<string, string> loadedTable(10);
    loadedTable.LoadText(filename);

    BOOST_CHECK_EQUAL(loadedTable.size(), 3);
    BOOST_CHECK_EQUAL(loadedTable.GET("apple"), "red");
    BOOST_CHECK_EQUAL(loadedTable.GET("banana"), "yellow");
    BOOST_CHECK_EQUAL(loadedTable.GET("lime"), "green");
}

BOOST_AUTO_TEST_CASE(Test_Binary_Serialization_StringInt) {
    TableHash<string, int> table(10);
    string filename = "test_bin_str_int.bin";

    table.HTADD("User1", 1001);
    table.HTADD("User2", 2002);
    table.HTADD("Admin", 9999);

    table.SaveBinary(filename);

    table.Clear();
    BOOST_CHECK(table.empty());

    table.LoadBinary(filename);

    BOOST_CHECK_EQUAL(table.size(), 3);
    BOOST_CHECK_EQUAL(table.GET("User1"), 1001);
    BOOST_CHECK_EQUAL(table.GET("Admin"), 9999);
}

BOOST_AUTO_TEST_CASE(Test_Binary_Serialization_IntString) {
    TableHash<int, string> table(10);
    string filename = "test_bin_int_str.bin";

    table.HTADD(1, "First");
    table.HTADD(2, "Second");
    table.HTADD(10, "Tenth");

    table.SaveBinary(filename);

    TableHash<int, string> loadedTable(5);
    loadedTable.LoadBinary(filename);

    BOOST_CHECK_EQUAL(loadedTable.size(), 3);
    BOOST_CHECK_EQUAL(loadedTable.GET(1), "First");
    BOOST_CHECK_EQUAL(loadedTable.GET(2), "Second");
    BOOST_CHECK_EQUAL(loadedTable.GET(10), "Tenth");
}


BOOST_AUTO_TEST_CASE(Test_Rehash_MixedTypes) {
    TableHash<int, double> table(2); 

    for(int i = 0; i < 20; ++i) {
        table.HTADD(i, static_cast<double>(i) * 1.5);
    }

    for(int i = 0; i < 20; ++i) {
        BOOST_CHECK_CLOSE(table.GET(i), static_cast<double>(i) * 1.5, 0.001);
    }
    
    BOOST_CHECK_EQUAL(table.size(), 20);
}

BOOST_AUTO_TEST_SUITE_END()
