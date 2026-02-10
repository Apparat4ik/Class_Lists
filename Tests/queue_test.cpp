#define BOOST_TEST_MODULE QueueTests
#include <boost/test/included/unit_test.hpp>
#include "Classes/MyQueue.hpp"
#include <fstream>
#include <cstdio>


BOOST_AUTO_TEST_CASE(test_push_pop_basic) {
    MyQueue<int> q;
    BOOST_CHECK(q.QEMPTY());
    BOOST_CHECK_EQUAL(q.qsize(), 0);

    q.QPUSH(10);
    q.QPUSH(20);
    q.QPUSH(30);

    BOOST_CHECK(!q.QEMPTY());
    BOOST_CHECK_EQUAL(q.qsize(), 3);
    BOOST_CHECK_EQUAL(q.get_head(), 10);
    BOOST_CHECK_EQUAL(q.get_tail(), 30);

    q.QPOP();
    BOOST_CHECK_EQUAL(q.get_head(), 20);
    BOOST_CHECK_EQUAL(q.qsize(), 2);

    q.QPOP();
    q.QPOP();
    BOOST_CHECK(q.QEMPTY());
    BOOST_CHECK_EQUAL(q.qsize(), 0);
}


BOOST_AUTO_TEST_CASE(test_copy_constructor) {
    MyQueue<int> q1;
    q1.QPUSH(1);
    q1.QPUSH(2);
    q1.QPUSH(3);

    MyQueue<int> q2(q1);

    BOOST_CHECK_EQUAL(q2.qsize(), 3);
    BOOST_CHECK_EQUAL(q2.get_head(), 1);
    BOOST_CHECK_EQUAL(q2.get_tail(), 3);

    q1.QPOP();
    BOOST_CHECK_EQUAL(q1.qsize(), 2);
    BOOST_CHECK_EQUAL(q2.qsize(), 3);
    BOOST_CHECK_EQUAL(q2.get_head(), 1);
}

// Тест оператора присваивания
BOOST_AUTO_TEST_CASE(test_assignment_operator) {
    MyQueue<int> q1;
    q1.QPUSH(100);

    MyQueue<int> q2;
    q2.QPUSH(5);
    q2.QPUSH(6);

    q2 = q1;

    BOOST_CHECK_EQUAL(q2.qsize(), 1);
    BOOST_CHECK_EQUAL(q2.get_head(), 100);

    q1.QPUSH(200);
    BOOST_CHECK_EQUAL(q1.qsize(), 2);
    BOOST_CHECK_EQUAL(q2.qsize(), 1);
}

// Тест корректности работы tail после полного опустошения
BOOST_AUTO_TEST_CASE(test_tail_after_full_pop) {
    MyQueue<int> q;
    q.QPUSH(1);
    q.QPOP();

    q.QPUSH(2); 
    BOOST_CHECK_EQUAL(q.get_head(), 2);
    BOOST_CHECK_EQUAL(q.get_tail(), 2);
}


BOOST_AUTO_TEST_CASE(test_serialize_text_basic) {
    MyQueue<int> q_out;
    q_out.QPUSH(10);
    q_out.QPUSH(20);
    q_out.QPUSH(30);
    q_out.QPUSH(40);

    string filename = "test_text_serialize.txt";
    q_out.serialize_text(filename);

    MyQueue<int> q_in;
    q_in.deserialize_text(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 4);
    BOOST_CHECK_EQUAL(q_in.get_head(), 10);
    BOOST_CHECK_EQUAL(q_in.get_tail(), 40);

    q_in.QPOP();
    BOOST_CHECK_EQUAL(q_in.get_head(), 20);
    
    q_in.QPOP();
    q_in.QPOP();
    BOOST_CHECK_EQUAL(q_in.get_head(), 40);
    BOOST_CHECK_EQUAL(q_in.qsize(), 1);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_text_empty_queue) {
    MyQueue<int> q_out;

    string filename = "test_text_empty.txt";
    q_out.serialize_text(filename);

    MyQueue<int> q_in;
    q_in.deserialize_text(filename);

    BOOST_CHECK(q_in.QEMPTY());
    BOOST_CHECK_EQUAL(q_in.qsize(), 0);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_text_strings) {
    MyQueue<string> q_out;
    q_out.QPUSH("Alpha");
    q_out.QPUSH("Beta");
    q_out.QPUSH("Gamma");

    string filename = "test_text_strings.txt";
    q_out.serialize_text(filename);

    MyQueue<string> q_in;
    q_in.deserialize_text(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 3);
    BOOST_CHECK_EQUAL(q_in.get_head(), "Alpha");
    BOOST_CHECK_EQUAL(q_in.get_tail(), "Gamma");

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_text_overwrite_existing) {
    MyQueue<double> q_out;
    q_out.QPUSH(1.5);
    q_out.QPUSH(2.5);
    q_out.QPUSH(3.5);

    string filename = "test_text_overwrite.txt";
    q_out.serialize_text(filename);

    MyQueue<double> q_in;
    q_in.QPUSH(999.0);
    q_in.QPUSH(888.0);

    BOOST_CHECK_EQUAL(q_in.qsize(), 2);

    q_in.deserialize_text(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 3);
    BOOST_CHECK_EQUAL(q_in.get_head(), 1.5);
    BOOST_CHECK_EQUAL(q_in.get_tail(), 3.5);

    remove(filename.c_str());
}



BOOST_AUTO_TEST_CASE(test_serialize_binary_basic) {
    MyQueue<int> q_out;
    q_out.QPUSH(100);
    q_out.QPUSH(200);
    q_out.QPUSH(300);
    q_out.QPUSH(400);
    q_out.QPUSH(500);

    string filename = "test_binary_serialize.bin";
    q_out.serialize_binary(filename);

    MyQueue<int> q_in;
    q_in.deserialize_binary(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 5);
    BOOST_CHECK_EQUAL(q_in.get_head(), 100);
    BOOST_CHECK_EQUAL(q_in.get_tail(), 500);

    q_in.QPOP();
    q_in.QPOP();
    BOOST_CHECK_EQUAL(q_in.get_head(), 300);
    BOOST_CHECK_EQUAL(q_in.qsize(), 3);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_binary_empty_queue) {
    MyQueue<int> q_out;

    string filename = "test_binary_empty.bin";
    q_out.serialize_binary(filename);

    MyQueue<int> q_in;
    q_in.deserialize_binary(filename);

    BOOST_CHECK(q_in.QEMPTY());
    BOOST_CHECK_EQUAL(q_in.qsize(), 0);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_binary_doubles) {
    MyQueue<double> q_out;
    q_out.QPUSH(3.14159);
    q_out.QPUSH(2.71828);
    q_out.QPUSH(1.41421);

    string filename = "test_binary_doubles.bin";
    q_out.serialize_binary(filename);

    MyQueue<double> q_in;
    q_in.deserialize_binary(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 3);
    BOOST_CHECK_CLOSE(q_in.get_head(), 3.14159, 0.0001);
    
    q_in.QPOP();
    BOOST_CHECK_CLOSE(q_in.get_head(), 2.71828, 0.0001);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_binary_overwrite_existing) {
    MyQueue<int> q_out;
    q_out.QPUSH(111);
    q_out.QPUSH(222);

    string filename = "test_binary_overwrite.bin";
    q_out.serialize_binary(filename);

    MyQueue<int> q_in;
    q_in.QPUSH(999);
    q_in.QPUSH(888);
    q_in.QPUSH(777);

    BOOST_CHECK_EQUAL(q_in.qsize(), 3);

    q_in.deserialize_binary(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 2);
    BOOST_CHECK_EQUAL(q_in.get_head(), 111);
    BOOST_CHECK_EQUAL(q_in.get_tail(), 222);

    remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_serialize_binary_large_queue) {
    MyQueue<int> q_out;
    for (int i = 1; i <= 1000; i++) {
        q_out.QPUSH(i);
    }

    string filename = "test_binary_large.bin";
    q_out.serialize_binary(filename);

    MyQueue<int> q_in;
    q_in.deserialize_binary(filename);

    BOOST_CHECK_EQUAL(q_in.qsize(), 1000);
    BOOST_CHECK_EQUAL(q_in.get_head(), 1);
    BOOST_CHECK_EQUAL(q_in.get_tail(), 1000);

    for (int i = 1; i <= 500; i++) {
        q_in.QPOP();
    }
    BOOST_CHECK_EQUAL(q_in.get_head(), 501);
    BOOST_CHECK_EQUAL(q_in.qsize(), 500);

    remove(filename.c_str());
}



BOOST_AUTO_TEST_CASE(test_deserialize_text_nonexistent_file) {
    MyQueue<int> q;
    BOOST_CHECK_THROW(q.deserialize_text("nonexistent_file.txt"), runtime_error);
}

BOOST_AUTO_TEST_CASE(test_deserialize_binary_nonexistent_file) {
    MyQueue<int> q;
    BOOST_CHECK_THROW(q.deserialize_binary("nonexistent_file.bin"), runtime_error);
}


BOOST_AUTO_TEST_CASE(test_text_vs_binary_consistency) {
    MyQueue<int> q_original;
    q_original.QPUSH(10);
    q_original.QPUSH(20);
    q_original.QPUSH(30);

    string text_file = "test_consistency.txt";
    string binary_file = "test_consistency.bin";

    q_original.serialize_text(text_file);
    q_original.serialize_binary(binary_file);

    MyQueue<int> q_from_text;
    MyQueue<int> q_from_binary;

    q_from_text.deserialize_text(text_file);
    q_from_binary.deserialize_binary(binary_file);

    BOOST_CHECK_EQUAL(q_from_text.qsize(), q_from_binary.qsize());
    BOOST_CHECK_EQUAL(q_from_text.get_head(), q_from_binary.get_head());
    BOOST_CHECK_EQUAL(q_from_text.get_tail(), q_from_binary.get_tail());

    remove(text_file.c_str());
    remove(binary_file.c_str());
}



