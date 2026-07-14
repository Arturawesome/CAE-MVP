SHELL := /bin/sh

CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -g

GTEST_INC := -I/opt/homebrew/include
GTEST_LIB := -L/opt/homebrew/lib -lgtest -lgtest_main -pthread

COVFLAGS := --coverage -fprofile-arcs -ftest-coverage

LIB := s21_matrix_oop.a

SRC := $(filter-out test/%, $(wildcard *.cpp))
OBJ := $(SRC:.cpp=.o)

TEST_SRC := test/test.cpp
TEST_BIN := test/test
GCOV_BIN := gcov/gcov_test



all: clean $(LIB)

clean:
	rm -rf *.o *.a *.gcda *.gcno
	rm -rf test/test
	rm -rf gcov



$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@



test: clean $(LIB)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) $(LIB) $(GTEST_INC) $(GTEST_LIB) -o $(TEST_BIN)
	./$(TEST_BIN)



gcov_report: clean
	mkdir -p gcov

	# Библиотека с coverage
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -c $(SRC)
	ar rc $(LIB) *.o

	# Тесты: линковка С coverage runtime
	$(CXX) $(CXXFLAGS) $(COVFLAGS) \
		$(TEST_SRC) $(LIB) \
		$(GTEST_INC) $(GTEST_LIB) \
		-o $(GCOV_BIN)

	cd gcov && ./gcov_test

	lcov --capture \
		--directory . \
		--ignore-errors inconsistent,unsupported,format \
		--output-file gcov/coverage.info

	lcov --remove gcov/coverage.info \
	'*/test/*' \
	'*gtest*' \
	'/opt/homebrew/*' \
	'/usr/*' \
	'*__cxx_global_var_init*' \
	--ignore-errors unused \
	-o gcov/coverage.info


	genhtml gcov/coverage.info -o gcov/report



clang-format:
	clang-format -i *.cpp *.h test/*.cpp test/*.h

clang-check:
	clang-format -n *.cpp *.h test/*.cpp test/*.h

valgrind: test
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test/test


.PHONY: all clean test gcov_report clang-format clang-check valgrind
