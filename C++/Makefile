SHELL := /bin/bash

# Настройки компилятора
CXX      := g++
STD      := -std=c++17
WARN     := -Wall -Wextra -Wpedantic
BREW_PREFIX ?= /opt/homebrew
INCLUDES := -I. -I$(BREW_PREFIX)/include

# Флаги для тестов
USE_BOOST_INCLUDED ?= 1

ifeq ($(USE_BOOST_INCLUDED),1)
  BOOST_FLAGS :=
  BOOST_LIBS  :=
else
  BOOST_FLAGS := -DBOOST_TEST_DYN_LINK
  BOOST_LIBS  := -L$(BREW_PREFIX)/lib -lboost_unit_test_framework
endif

# Флаги для покрытия кода (gcov)
COV_FLAGS := --coverage -fprofile-arcs -ftest-coverage -O0 -g

# Переменные файлов
SRC_DIR  := Classes
TEST_DIR := Tests
BUILD    := build
BIN      := bin
COV_DIR  := coverage_html

# Исходники классов и тестов
CLASS_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS  := $(wildcard $(TEST_DIR)/*.cpp)

# Исполняемые файлы тестов (bin/test_name)
TEST_BINS  := $(patsubst $(TEST_DIR)/%.cpp,$(BIN)/%,$(TEST_SRCS))

# Основные цели
.PHONY: all tests run coverage clean clean_cov

all: tests

# Сборка тестов
tests: $(TEST_BINS)

$(BIN)/%: $(TEST_DIR)/%.cpp $(CLASS_SRCS) | $(BIN) $(BUILD)
	$(CXX) $(STD) $(WARN) $(INCLUDES) $(COV_FLAGS) $(BOOST_FLAGS) \
	$^ -o $@ $(BOOST_LIBS)

$(BIN) $(BUILD):
	mkdir -p $@

# Запуск тестов
run: tests
	@set -e; \
	for t in $(TEST_BINS); do \
		echo "--> Running $$t"; \
		./$$t; \
	done

# Генерация покрытия
coverage: clean_cov tests run
	@echo "--> Generating Coverage Report..."
	
	# 1. Захватываем данные. --ignore-errors подавляет версии и несовпадения
	lcov --capture --directory . --include '*Classes/*' --output-file $(BUILD)/coverage.info \
		--ignore-errors mismatch,gcov,usage
	
	# 2. Генерируем HTML. Добавлен 'category' в игнор-лист
	genhtml $(BUILD)/coverage.info --output-directory $(COV_DIR) \
		--ignore-errors mismatch,category,usage,empty
	
	@echo "--> Report created: $(COV_DIR)/index.html"


# Очистка
clean:
	rm -rf $(BUILD) $(BIN) $(COV_DIR) *.gcda *.gcno

clean_cov:
	rm -rf $(COV_DIR)
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	rm -f $(BUILD)/coverage.info