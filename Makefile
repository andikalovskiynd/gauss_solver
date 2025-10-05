# Detect OS and set appropriate paths (I use MacOS as main system)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS with Homebrew
	GTEST_INC_DIR ?= /opt/homebrew/opt/googletest/include
	GTEST_LIB_DIR ?= /opt/homebrew/opt/googletest/lib
	EIGEN_INC_DIR ?= /opt/homebrew/include/eigen3
else
    # Linux (including CI)
	GTEST_INC_DIR ?= /usr/local/include
	GTEST_LIB_DIR ?= /usr/local/lib
	EIGEN_INC_DIR ?= /usr/local/include/eigen3
endif

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -I$(EIGEN_INC_DIR)
TEST_CXXFLAGS = $(CXXFLAGS) -Iinclude -I$(GTEST_INC_DIR) -DBUILDING_TESTS

ifdef CI
	LDFLAGS = -L$(GTEST_LIB_DIR) -lgtest_main -lgtest -pthread
else
	LDFLAGS = -L$(GTEST_LIB_DIR) -lgtest_main -lgtest -pthread -fsanitize=address
endif

MAIN_TARGET = gauss_solver
TEST_TARGET = test_gauss_solver

MAIN_SOURCE = main.cpp
LIB_SOURCE = gauss_solver.cpp
LIB_OBJECT = gauss_solver.o
TEST_SOURCES = test.cpp
TEST_OBJECTS = $(patsubst %.cpp,%.o,$(TEST_SOURCES))

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SOURCE) $(LIB_OBJECT)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_OBJECT)
	$(CXX) $(LDFLAGS) $^ -o $@

$(LIB_OBJECT): $(LIB_SOURCE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(MAIN_TARGET) $(TEST_TARGET) $(TEST_OBJECTS) $(LIB_OBJECT)

.PHONY: all test clean 