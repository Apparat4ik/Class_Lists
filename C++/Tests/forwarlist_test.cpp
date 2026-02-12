#define BOOST_TEST_MODULE ForwardListTests
#include <boost/test/included/unit_test.hpp>
#include "Classes/MyFList.hpp"
#include <string>
#include <vector>
#include <cstdio>


template<typename T>
bool check_list_content(const ForwardList<T>& list, const std::vector<T>& expected) {
    try {
        for (size_t i = 0; i < expected.size(); ++i) {
            if (list.FGET_key(i) != expected[i]) return false;
        }
        return true;
    } catch (exception& err) {
        return false;
    }
}

BOOST_AUTO_TEST_SUITE(ForwardListSuite)

BOOST_AUTO_TEST_CASE(ConstructorAndPushFront) {
    ForwardList<int> list;

    list.FPUSH_front(10);
    list.FPUSH_front(20);
    list.FPUSH_front(30);


    BOOST_CHECK_EQUAL(list.FGET_key(0), 30);
    BOOST_CHECK_EQUAL(list.FGET_key(1), 20);
    BOOST_CHECK_EQUAL(list.FGET_key(2), 10);
}

BOOST_AUTO_TEST_CASE(PushBack) {
    ForwardList<int> list;

    list.FPUSH_back(10);
    list.FPUSH_back(20);
    list.FPUSH_back(30);


    BOOST_CHECK_EQUAL(list.FGET_key(0), 10);
    BOOST_CHECK_EQUAL(list.FGET_key(1), 20);
    BOOST_CHECK_EQUAL(list.FGET_key(2), 30);
}

BOOST_AUTO_TEST_CASE(CopyConstructor) {
    ForwardList<int> original;
    original.FPUSH_back(1);
    original.FPUSH_back(2);
    original.FPUSH_back(3);

    ForwardList<int> flCopy(original);

    BOOST_CHECK_EQUAL(flCopy.FGET_key(0), 1);
    BOOST_CHECK_EQUAL(flCopy.FGET_key(1), 2);
    BOOST_CHECK_EQUAL(flCopy.FGET_key(2), 3);

    flCopy.FDEL_front();
    BOOST_CHECK_EQUAL(flCopy.FGET_key(0), 2);
    BOOST_CHECK_EQUAL(original.FGET_key(0), 1);
}

BOOST_AUTO_TEST_CASE(AssignmentOperator) {
    ForwardList<int> list1;
    list1.FPUSH_back(100);
    list1.FPUSH_back(200);

    ForwardList<int> list2;
    list2.FPUSH_back(5);

    list2 = list1;

    BOOST_CHECK_EQUAL(list2.FGET_key(0), 100);
    BOOST_CHECK_EQUAL(list2.FGET_key(1), 200);


    list2 = list2;
    BOOST_CHECK_EQUAL(list2.FGET_key(0), 100);
}

BOOST_AUTO_TEST_CASE(DeleteFront) {
    ForwardList<int> list;
    list.FPUSH_back(10);
    list.FPUSH_back(20);

    list.FDEL_front();

    BOOST_CHECK_EQUAL(list.FGET_key(0), 20);
}

BOOST_AUTO_TEST_CASE(DeleteBack) {
    ForwardList<int> list;
    list.FPUSH_back(10);
    list.FPUSH_back(20);
    list.FPUSH_back(30);

    list.FDEL_back();

    BOOST_CHECK_EQUAL(list.FGET_key(0), 10);
    BOOST_CHECK_EQUAL(list.FGET_key(1), 20);

    BOOST_CHECK_EQUAL(list.FGET_key(2), 0);
}

BOOST_AUTO_TEST_CASE(InsertNext) {
    ForwardList<std::string> list;
    list.FPUSH_back("A");
    list.FPUSH_back("C");

    list.FPUSH_next(0, "B");

    BOOST_CHECK_EQUAL(list.FGET_key(0), "A");
    BOOST_CHECK_EQUAL(list.FGET_key(1), "B");
    BOOST_CHECK_EQUAL(list.FGET_key(2), "C");
}

BOOST_AUTO_TEST_CASE(DeleteByValue) {
    ForwardList<int> list;
    list.FPUSH_back(10);
    list.FPUSH_back(20);
    list.FPUSH_back(30);

    list.FDEL_val(20);

    BOOST_CHECK_EQUAL(list.FGET_key(0), 10);
    BOOST_CHECK_EQUAL(list.FGET_key(1), 30);

    list.FDEL_val(10);
    BOOST_CHECK_EQUAL(list.FGET_key(0), 30);
}

BOOST_AUTO_TEST_CASE(ClearList) {
    ForwardList<int> list;
    list.FPUSH_back(1);
    list.FPUSH_back(2);

    list.destroy_list();

    BOOST_CHECK_EQUAL(list.FGET_key(0), 0);
}

BOOST_AUTO_TEST_CASE(OutOfBoundsAccess) {
    ForwardList<int> list;
    list.FPUSH_back(5);

    BOOST_CHECK_EQUAL(list.FGET_key(100), 0);
    BOOST_CHECK_EQUAL(list.FGET_key(-1), 0);
}


BOOST_AUTO_TEST_CASE(SerializationText_Int) {
    const std::string filename = "test_data.txt";
    
    // 1. Создаем и заполняем список
    ForwardList<int> listOut;
    listOut.FPUSH_back(42);
    listOut.FPUSH_back(100);
    listOut.FPUSH_back(-5);

    // 2. Сохраняем
    BOOST_CHECK_NO_THROW(listOut.serialize_text(filename));

    // 3. Загружаем в новый список
    ForwardList<int> listIn;
    BOOST_CHECK_NO_THROW(listIn.deserialize_text(filename));

    // 4. Проверяем содержимое
    BOOST_CHECK_EQUAL(listIn.flsize(), 3);
    BOOST_CHECK_EQUAL(listIn.FGET_key(0), 42);
    BOOST_CHECK_EQUAL(listIn.FGET_key(1), 100);
    BOOST_CHECK_EQUAL(listIn.FGET_key(2), -5);

    // Очистка файла
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(SerializationText_String) {
    const std::string filename = "test_strings.txt";
    
    ForwardList<std::string> listOut;
    listOut.FPUSH_back("Hello");
    listOut.FPUSH_back("World");
    listOut.FPUSH_back("Boost");

    BOOST_CHECK_NO_THROW(listOut.serialize_text(filename));

    ForwardList<std::string> listIn;
    BOOST_CHECK_NO_THROW(listIn.deserialize_text(filename));

    BOOST_CHECK_EQUAL(listIn.FGET_key(0), "Hello");
    BOOST_CHECK_EQUAL(listIn.FGET_key(1), "World");
    BOOST_CHECK_EQUAL(listIn.FGET_key(2), "Boost");

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(SerializationBinary_POD) {
    const std::string filename = "test_data.bin";

    ForwardList<double> listOut;
    listOut.FPUSH_back(3.14);
    listOut.FPUSH_back(2.71);
    listOut.FPUSH_back(1.41);

    BOOST_CHECK_NO_THROW(listOut.serialize_binary(filename));

    ForwardList<double> listIn;
    BOOST_CHECK_NO_THROW(listIn.deserialize_binary(filename));

    BOOST_CHECK_EQUAL(listIn.flsize(), 3);
    
    BOOST_CHECK_CLOSE(listIn.FGET_key(0), 3.14, 0.001);
    BOOST_CHECK_CLOSE(listIn.FGET_key(1), 2.71, 0.001);
    BOOST_CHECK_CLOSE(listIn.FGET_key(2), 1.41, 0.001);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE_END()
