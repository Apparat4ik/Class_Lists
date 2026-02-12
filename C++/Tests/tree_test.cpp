#define BOOST_TEST_MODULE FBTreeTests
#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include "Classes/BinaryTree.hpp" 

struct TempFile {
    std::string path;
    explicit TempFile(std::string p) : path(std::move(p)) {}
    ~TempFile() { std::remove(path.c_str()); }
};

struct CoutRedirect {
    CoutRedirect(std::streambuf* new_buffer) 
        : old(std::cout.rdbuf(new_buffer)) {}  // Сохраняем старый буфер и подменяем на новый
    ~CoutRedirect() {
        std::cout.rdbuf(old);  // Восстанавливаем оригинальный буфер
    }
    std::streambuf* old;
};


BOOST_AUTO_TEST_SUITE(FBTreeTestSuite)


BOOST_AUTO_TEST_CASE(TestConstructionAndInsert) {
    FBTree<int> tree;

    tree.TINSERT(10);
    BOOST_CHECK(tree.TISMEMBER(tree, 10));

    tree.TINSERT(5);
    BOOST_CHECK(tree.TISMEMBER(tree, 5));

    tree.TINSERT(15);
    BOOST_CHECK(tree.TISMEMBER(tree, 15));

    BOOST_CHECK(!tree.TISMEMBER(tree, 99));
}


BOOST_AUTO_TEST_CASE(TestDuplicateInsertion) {
    FBTree<int> tree;
    tree.TINSERT(10);
    tree.TINSERT(10);

    BOOST_CHECK(tree.TISMEMBER(tree, 10));
}


BOOST_AUTO_TEST_CASE(TestBSTLogic) {
    FBTree<int> tree;

    std::vector<int> inputs = {50, 30, 20, 40, 70, 60, 80};
    for (int x : inputs) {
        tree.TINSERT(x);
    }

    for (int x : inputs) {
        BOOST_CHECK_MESSAGE(tree.TISMEMBER(tree, x), "Element " << x << " should be in tree");
    }
}


struct CoutRedirect {
    CoutRedirect(std::streambuf* new_buffer) 
        : old(std::cout.rdbuf(new_buffer)) {}
    ~CoutRedirect() {std::cout.rdbuf(old);}
    std::streambuf* old;
};

BOOST_AUTO_TEST_CASE(TestPreOrderTraversal) {
    FBTree<int> tree;
    tree.TINSERT(2);
    tree.TINSERT(1);
    tree.TINSERT(3);

    std::stringstream buffer;
    {
        CoutRedirect guard(buffer.rdbuf());
        tree.PRINT_preorder();
    }

    std::string expected = "2 1 3 \n"; 
    BOOST_CHECK_EQUAL(buffer.str(), expected);
}

BOOST_AUTO_TEST_CASE(TestInOrderTraversal) {
    FBTree<int> tree;
    tree.TINSERT(2);
    tree.TINSERT(1);
    tree.TINSERT(3);

    std::stringstream buffer;
    {
        CoutRedirect guard(buffer.rdbuf());
        tree.PRINT_inorder();
    }


    std::string expected = "1 2 3 \n";
    BOOST_CHECK_EQUAL(buffer.str(), expected);
}



BOOST_AUTO_TEST_CASE(TestBFS) {
    FBTree<int> tree;
    tree.TINSERT(4);
    tree.TINSERT(2);
    tree.TINSERT(6);
    tree.TINSERT(1);
    tree.TINSERT(3);

    std::stringstream buffer;

    CoutRedirect guard(buffer.rdbuf());
    tree.PRINT_BFS();

    std::string expected = "4 2 6 1 3 \n";
    BOOST_CHECK_EQUAL(buffer.str(), expected);
}


BOOST_AUTO_TEST_CASE(TestEmptyTree) {
    FBTree<int> tree;

    BOOST_CHECK(!tree.TISMEMBER(tree, 10));


    BOOST_CHECK_THROW(tree.PRINT_BFS(), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(TestSerializeDeserializeText_RoundTrip) {
    TempFile tf("test_tree_text.txt");

    FBTree<int> out;
    std::vector<int> inputs = {50, 30, 20, 40, 70, 60, 80};
    for (int x : inputs) out.TINSERT(x);

    out.SERIALIZE_TEXT(tf.path);

    FBTree<int> in;
    in.DESERIALIZE_TEXT(tf.path);

    for (int x : inputs) {
        BOOST_CHECK_MESSAGE(in.TISMEMBER(in, x), "Missing element after text deserialize: " << x);
    }
    BOOST_CHECK(!in.TISMEMBER(in, 999));

    // Дополнительно фиксируем структуру (preorder для данного порядка вставки)
    std::stringstream buffer;
    {
        CoutRedirect guard(buffer.rdbuf());
        in.PRINT_preorder();
    }
    BOOST_CHECK_EQUAL(buffer.str(), "50 30 20 40 70 60 80 \n");
}

BOOST_AUTO_TEST_CASE(TestSerializeDeserializeBinary_RoundTrip) {
    TempFile tf("test_tree_bin.dat");

    FBTree<int> out;
    std::vector<int> inputs = {10, 5, 2, 7, 15, 12, 20};
    for (int x : inputs) out.TINSERT(x);

    out.SERIALIZE_BINARY(tf.path);

    FBTree<int> in;
    in.DESERIALIZE_BINARY(tf.path);

    for (int x : inputs) {
        BOOST_CHECK_MESSAGE(in.TISMEMBER(in, x), "Missing element after binary deserialize: " << x);
    }
    BOOST_CHECK(!in.TISMEMBER(in, -1));

    // Проверим обход в ширину (он тоже детерминированен для данного дерева)
    std::stringstream buffer;
    {
        CoutRedirect guard(buffer.rdbuf());
        in.PRINT_BFS();
    }
    BOOST_CHECK_EQUAL(buffer.str(), "10 5 15 2 7 12 20 \n");
}

BOOST_AUTO_TEST_CASE(TestSerializeDeserializeText_EmptyTree) {
    TempFile tf("test_empty_tree.txt");

    FBTree<int> out;                 // пустое
    out.SERIALIZE_TEXT(tf.path);

    FBTree<int> in;
    in.DESERIALIZE_TEXT(tf.path);

    BOOST_CHECK(!in.TISMEMBER(in, 1));
    BOOST_CHECK_THROW(in.PRINT_BFS(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(TestSerializeDeserializeBinary_EmptyTree) {
    TempFile tf("test_empty_tree.bin");

    FBTree<int> out;                 // пустое
    out.SERIALIZE_BINARY(tf.path);

    FBTree<int> in;
    in.DESERIALIZE_BINARY(tf.path);

    BOOST_CHECK(!in.TISMEMBER(in, 1));
    BOOST_CHECK_THROW(in.PRINT_BFS(), std::out_of_range);
}
