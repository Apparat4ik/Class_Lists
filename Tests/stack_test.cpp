#define BOOST_TEST_MODULE MyStackTests
#include <boost/test/included/unit_test.hpp>
#include "Classes/MyStack.hpp" 
#include <string>
#include <fstream>
#include <cstdio> 


BOOST_AUTO_TEST_SUITE(BasicStackOperations)

BOOST_AUTO_TEST_CASE(test_default_constructor) {
    MyStack<int> stack;
    BOOST_CHECK_EQUAL(stack.stsize(), 0);
}

BOOST_AUTO_TEST_CASE(test_initializer_list_constructor) {
    MyStack<int> stack{1, 2, 3, 4, 5};
    BOOST_CHECK_EQUAL(stack.stsize(), 5);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 5);
}

BOOST_AUTO_TEST_CASE(test_push_single_element) {
    MyStack<int> stack;
    stack.SPUSH(42);
    BOOST_CHECK_EQUAL(stack.stsize(), 1);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 42);
}

BOOST_AUTO_TEST_CASE(test_push_multiple_elements) {
    MyStack<int> stack;
    stack.SPUSH(10);
    stack.SPUSH(20);
    stack.SPUSH(30);
    BOOST_CHECK_EQUAL(stack.stsize(), 3);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 30);
}

BOOST_AUTO_TEST_CASE(test_pop_single_element) {
    MyStack<int> stack;
    stack.SPUSH(100);
    int value = stack.SPOP();
    BOOST_CHECK_EQUAL(value, 100);
    BOOST_CHECK_EQUAL(stack.stsize(), 0);
}

BOOST_AUTO_TEST_CASE(test_pop_multiple_elements) {
    MyStack<int> stack;
    stack.SPUSH(1);
    stack.SPUSH(2);
    stack.SPUSH(3);
    BOOST_CHECK_EQUAL(stack.SPOP(), 3);
    BOOST_CHECK_EQUAL(stack.SPOP(), 2);
    BOOST_CHECK_EQUAL(stack.SPOP(), 1);
    BOOST_CHECK_EQUAL(stack.stsize(), 0);
}

BOOST_AUTO_TEST_CASE(test_get_head) {
    MyStack<int> stack;
    stack.SPUSH(99);
    stack.SPUSH(88);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 88);
    BOOST_CHECK_EQUAL(stack.stsize(), 2);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 88);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(StringStackOperations)

BOOST_AUTO_TEST_CASE(test_string_stack) {
    MyStack<std::string> stack;
    stack.SPUSH("Hello");
    stack.SPUSH("World");
    stack.SPUSH("!");

    BOOST_CHECK_EQUAL(stack.stsize(), 3);
    BOOST_CHECK_EQUAL(stack.SGET_head(), "!");
    BOOST_CHECK_EQUAL(stack.SPOP(), "!");
    BOOST_CHECK_EQUAL(stack.SPOP(), "World");
    BOOST_CHECK_EQUAL(stack.SPOP(), "Hello");
}

BOOST_AUTO_TEST_CASE(test_string_initializer_list) {
    MyStack<std::string> stack{"first", "second", "third"};
    BOOST_CHECK_EQUAL(stack.stsize(), 3);
    BOOST_CHECK_EQUAL(stack.SGET_head(), "third");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TextFileIOOperations)

BOOST_AUTO_TEST_CASE(test_serialize_text) {
    MyStack<int> stack;
    stack.SPUSH(10);
    stack.SPUSH(20);
    stack.SPUSH(30); 

    const std::string filename = "test_stack_text.txt";
    stack.serialize_text(filename);

    std::ifstream file(filename);
    BOOST_REQUIRE(file.is_open());


    int val1, val2, val3;
    file >> val1 >> val2 >> val3;
    
    BOOST_CHECK_EQUAL(val1, 30);
    BOOST_CHECK_EQUAL(val2, 20);
    BOOST_CHECK_EQUAL(val3, 10);

    file.close();
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_deserialize_text) {
    const std::string filename = "test_stack_read.txt";
    std::ofstream outfile(filename);
    outfile << "300 200 100"; 
    outfile.close();

    MyStack<int> stack;
    stack.deserialize_text(filename);

    BOOST_CHECK_EQUAL(stack.stsize(), 3);
    BOOST_CHECK_EQUAL(stack.SPOP(), 300);
    BOOST_CHECK_EQUAL(stack.SPOP(), 200);
    BOOST_CHECK_EQUAL(stack.SPOP(), 100);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_text_roundtrip) {
    MyStack<int> originalStack;
    originalStack.SPUSH(5);
    originalStack.SPUSH(15);
    originalStack.SPUSH(25); 

    const std::string filename = "test_text_roundtrip.txt";
    originalStack.serialize_text(filename);

    MyStack<int> loadedStack;
    loadedStack.deserialize_text(filename);

    BOOST_CHECK_EQUAL(loadedStack.stsize(), originalStack.stsize());
    BOOST_CHECK_EQUAL(loadedStack.SGET_head(), originalStack.SGET_head());
    

    BOOST_CHECK_EQUAL(loadedStack.SPOP(), 25);
    BOOST_CHECK_EQUAL(loadedStack.SPOP(), 15);
    BOOST_CHECK_EQUAL(loadedStack.SPOP(), 5);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(BinarySerializationOperations)

BOOST_AUTO_TEST_CASE(test_binary_int_roundtrip) {
    MyStack<int> original;
    original.SPUSH(100);
    original.SPUSH(200);
    original.SPUSH(300);

    const std::string filename = "test_data.bin";
    original.serialize_binary(filename);

    MyStack<int> loaded;
    loaded.deserialize_binary(filename);

    BOOST_CHECK_EQUAL(loaded.stsize(), 3);
    BOOST_CHECK_EQUAL(loaded.SPOP(), 300);
    BOOST_CHECK_EQUAL(loaded.SPOP(), 200);
    BOOST_CHECK_EQUAL(loaded.SPOP(), 100);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_binary_double_precision) {
    MyStack<double> original;
    original.SPUSH(1.11111);
    original.SPUSH(2.22222);

    const std::string filename = "test_double.bin";
    original.serialize_binary(filename);

    MyStack<double> loaded;
    loaded.deserialize_binary(filename);

    BOOST_CHECK_CLOSE(loaded.SPOP(), 2.22222, 0.0001);
    BOOST_CHECK_CLOSE(loaded.SPOP(), 1.11111, 0.0001);

    std::remove(filename.c_str());
}


struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "(" << p.x << "," << p.y << ")";
    }
};

BOOST_AUTO_TEST_CASE(test_binary_struct_pod) {
    MyStack<Point> original;
    original.SPUSH({1, 2});
    original.SPUSH({3, 4});

    const std::string filename = "test_struct.bin";
    original.serialize_binary(filename);

    MyStack<Point> loaded;
    loaded.deserialize_binary(filename);

    Point p1 = loaded.SPOP();
    Point p2 = loaded.SPOP();

    BOOST_CHECK_EQUAL(p1.x, 3);
    BOOST_CHECK_EQUAL(p1.y, 4);
    BOOST_CHECK_EQUAL(p2.x, 1);
    BOOST_CHECK_EQUAL(p2.y, 2);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(EdgeCases)

BOOST_AUTO_TEST_CASE(test_large_stack) {
    MyStack<int> stack;
    const int count = 1000;
    for (int i = 0; i < count; ++i) {
        stack.SPUSH(i);
    }
    BOOST_CHECK_EQUAL(stack.stsize(), count);
    for (int i = count - 1; i >= 0; --i) {
        BOOST_CHECK_EQUAL(stack.SPOP(), i);
    }
    BOOST_CHECK_EQUAL(stack.stsize(), 0);
}

BOOST_AUTO_TEST_CASE(test_push_pop_alternating) {
    MyStack<int> stack;
    stack.SPUSH(1);
    BOOST_CHECK_EQUAL(stack.SPOP(), 1);
    stack.SPUSH(2);
    stack.SPUSH(3);
    BOOST_CHECK_EQUAL(stack.SPOP(), 3);
    stack.SPUSH(4);
    BOOST_CHECK_EQUAL(stack.SGET_head(), 4);
    BOOST_CHECK_EQUAL(stack.SPOP(), 4);
    BOOST_CHECK_EQUAL(stack.SPOP(), 2);
}

BOOST_AUTO_TEST_CASE(test_empty_initializer_list) {
    MyStack<int> stack{};
    BOOST_CHECK_EQUAL(stack.stsize(), 0);
}


BOOST_AUTO_TEST_CASE(test_deserialize_overwrites_existing) {
    MyStack<int> stack;
    stack.SPUSH(999);

    const std::string filename = "test_overwrite.txt";
    std::ofstream outfile(filename);
    outfile << "10 20";
    outfile.close();

    stack.deserialize_text(filename);


    BOOST_CHECK_EQUAL(stack.stsize(), 2);
    BOOST_CHECK_EQUAL(stack.SPOP(), 10);
    BOOST_CHECK_EQUAL(stack.SPOP(), 20);

    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(CopyConstructorTests)

BOOST_AUTO_TEST_CASE(test_copy_constructor_integrity) {
    MyStack<int> original;
    original.SPUSH(10);
    original.SPUSH(20);
    original.SPUSH(30);

    MyStack<int> stCopy(original);

    BOOST_CHECK_EQUAL(stCopy.stsize(), 3);
    BOOST_CHECK_EQUAL(original.stsize(), 3);
    BOOST_CHECK_EQUAL(stCopy.SGET_head(), 30);
    BOOST_CHECK_EQUAL(stCopy.SPOP(), 30);
}

BOOST_AUTO_TEST_CASE(test_assignment_operator) {
    MyStack<std::string> s1;
    s1.SPUSH("A");
    s1.SPUSH("B");

    MyStack<std::string> s2;
    s2.SPUSH("Old");

    s2 = s1;

    BOOST_CHECK_EQUAL(s2.stsize(), 2);
    BOOST_CHECK_EQUAL(s2.SPOP(), "B");
    BOOST_CHECK_EQUAL(s2.SPOP(), "A");
    BOOST_CHECK_EQUAL(s1.stsize(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
