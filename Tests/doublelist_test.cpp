#define BOOST_TEST_MODULE DoubleListTests
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <string>
#include <fstream>
#include <cstdio>
#include "Classes/MyDoubleList.hpp" 

BOOST_AUTO_TEST_SUITE(DoubleListSuite)


BOOST_AUTO_TEST_CASE(ConstructorTest) {
    DoubleList<int> list;
    BOOST_CHECK_THROW(list.LGET_index(0), std::out_of_range);
}

// 2. Тест добавления в конец и в начало
BOOST_AUTO_TEST_CASE(PushFrontBackTest) {
    DoubleList<int> list;

    list.LPUSH_back(10);
    list.LPUSH_back(20);
    list.LPUSH_front(5);

    BOOST_CHECK_EQUAL(list.LGET_key(0), 5);
    BOOST_CHECK_EQUAL(list.LGET_key(1), 10);
    BOOST_CHECK_EQUAL(list.LGET_key(2), 20);
}

// 3. Тест удаления с начала и конца
BOOST_AUTO_TEST_CASE(DeleteFrontBackTest) {
    DoubleList<int> list;
    list.LPUSH_back(1);
    list.LPUSH_back(2);
    list.LPUSH_back(3);

    list.LDEL_front();
    BOOST_CHECK_EQUAL(list.LGET_key(0), 2);

    list.LDEL_back();
    BOOST_CHECK_EQUAL(list.LGET_key(0), 2);

    BOOST_CHECK_THROW(list.LGET_index(1), std::out_of_range);
}

// 4. Тест доступа по индексу и обработки ошибок
BOOST_AUTO_TEST_CASE(OutOfRangeTest) {
    DoubleList<int> list;
    list.LPUSH_back(100);

    BOOST_CHECK_NO_THROW(list.LGET_index(0));
    BOOST_CHECK_THROW(list.LGET_index(1), std::out_of_range);
}

// 5. Тест вставки после элемента (LPUSH_next)
BOOST_AUTO_TEST_CASE(PushNextTest) {
    DoubleList<int> list;
    list.LPUSH_back(10);
    list.LPUSH_back(30);


    list.LPUSH_next(0, 20);

    BOOST_CHECK_EQUAL(list.LGET_key(0), 10);
    BOOST_CHECK_EQUAL(list.LGET_key(1), 20);
    BOOST_CHECK_EQUAL(list.LGET_key(2), 30);
}

// 6. Тест вставки перед элементом (LPUSH_prev)
BOOST_AUTO_TEST_CASE(PushPrevTest) {
    DoubleList<int> list;
    list.LPUSH_back(10);
    list.LPUSH_back(30);

    list.LPUSH_prev(1, 20);

    BOOST_CHECK_EQUAL(list.LGET_key(1), 20);
}

// 7. Тест удаления по значению (LDEL_val)
BOOST_AUTO_TEST_CASE(DeleteByValueTest) {
    DoubleList<int> list;
    list.LPUSH_back(10);
    list.LPUSH_back(20);
    list.LPUSH_back(30);

    list.LDEL_val(20);

    BOOST_CHECK_EQUAL(list.LGET_key(0), 10);
    BOOST_CHECK_EQUAL(list.LGET_key(1), 30);

    list.LDEL_val(10);
    BOOST_CHECK_EQUAL(list.LGET_key(0), 30);
}

// 8. Тест поиска узла (LGET)
BOOST_AUTO_TEST_CASE(GetNodeTest) {
    DoubleList<std::string> list;
    list.LPUSH_back("hello");
    list.LPUSH_back("world");

    auto node = list.LGET("world");
    BOOST_CHECK(node != nullptr);
    BOOST_CHECK_EQUAL(node->key, "world");

    BOOST_CHECK_THROW(list.LGET("unknown"), std::invalid_argument);
}

// 9. Тест текстовой сериализации и десериализации
BOOST_AUTO_TEST_CASE(TextSerializationTest) {
    DoubleList<int> list;
    list.LPUSH_back(100);
    list.LPUSH_back(200);
    list.LPUSH_back(300);

    const std::string filename = "test_list_data.txt";


    BOOST_CHECK_NO_THROW(list.serialize_text(filename));


    DoubleList<int> loaded_list;
    BOOST_CHECK_NO_THROW(loaded_list.deserialize_text(filename));


    BOOST_CHECK_EQUAL(loaded_list.LGET_key(0), 100);
    BOOST_CHECK_EQUAL(loaded_list.LGET_key(1), 200);
    BOOST_CHECK_EQUAL(loaded_list.LGET_key(2), 300);

  
    std::remove(filename.c_str());
}

// 10. Тест бинарной сериализации и десериализации
BOOST_AUTO_TEST_CASE(BinarySerializationTest) {
    DoubleList<double> list;
    list.LPUSH_back(1.1);
    list.LPUSH_back(2.2);
    list.LPUSH_back(3.3);

    const std::string filename = "test_list_data.bin";


    BOOST_CHECK_NO_THROW(list.serialize_binary(filename));


    DoubleList<double> loaded_list;
    BOOST_CHECK_NO_THROW(loaded_list.deserialize_binary(filename));


    BOOST_CHECK_CLOSE(loaded_list.LGET_key(0), 1.1, 0.001);
    BOOST_CHECK_CLOSE(loaded_list.LGET_key(1), 2.2, 0.001);
    BOOST_CHECK_CLOSE(loaded_list.LGET_key(2), 3.3, 0.001);


    std::remove(filename.c_str());
}

// 11. Тест перезаписи существующего списка при десериализации
BOOST_AUTO_TEST_CASE(DeserializeOverwriteTest) {
    // Создаем файл с данными [10, 20]
    DoubleList<int> source_list;
    source_list.LPUSH_back(10);
    source_list.LPUSH_back(20);
    const std::string filename = "overwrite_test.txt";
    source_list.serialize_text(filename);

    // Создаем список-приемник с мусором [999, 999, 999]
    DoubleList<int> target_list;
    target_list.LPUSH_back(999);
    target_list.LPUSH_back(999);
    target_list.LPUSH_back(999);

    // Загружаем данные (должно очистить старые и записать новые)
    target_list.deserialize_text(filename);

    // Проверяем, что старых данных нет, а новые есть
    BOOST_CHECK_EQUAL(target_list.LGET_key(0), 10);
    BOOST_CHECK_EQUAL(target_list.LGET_key(1), 20);
    // Проверяем, что размер корректно уменьшился (доступ к 2 индексу должен дать ошибку)
    BOOST_CHECK_THROW(target_list.LGET_index(2), std::out_of_range);

    std::remove(filename.c_str());
}

// 12. Тест сериализации пустого списка
BOOST_AUTO_TEST_CASE(EmptyListSerializationTest) {
    DoubleList<int> empty_list;
    const std::string txt_file = "empty_test.txt";
    const std::string bin_file = "empty_test.bin";

    // Текстовый формат
    empty_list.serialize_text(txt_file);
    DoubleList<int> loaded_txt;
    loaded_txt.deserialize_text(txt_file);
    BOOST_CHECK_THROW(loaded_txt.LGET_index(0), std::out_of_range);

    // Бинарный формат
    empty_list.serialize_binary(bin_file);
    DoubleList<int> loaded_bin;
    loaded_bin.deserialize_binary(bin_file);
    BOOST_CHECK_THROW(loaded_bin.LGET_index(0), std::out_of_range);

    std::remove(txt_file.c_str());
    std::remove(bin_file.c_str());
}

// 13. Тест обработки ошибок (несуществующий файл)
BOOST_AUTO_TEST_CASE(FileNotFoundTest) {
    DoubleList<int> list;
    BOOST_CHECK_THROW(list.deserialize_text("non_existent_file.txt"), std::runtime_error);
    BOOST_CHECK_THROW(list.deserialize_binary("non_existent_file.bin"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
