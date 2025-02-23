MAKEFLAGS += --silent

SRC = $(wildcard src/*.c)
BIN_DIR = bin
TARGET = $(BIN_DIR)/sealox
CC = gcc
CFLAGS = -g -Wall

TEST_SRC = $(wildcard test/*.c)
TEST_TARGET = $(BIN_DIR)/test_runner
TEST_INCLUDE_SRC = $(filter-out src/main.c, $(SRC))

build: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(BIN_DIR)/* 2>/dev/null

run:
	./$(TARGET)

build_test: $(SRC) $(TEST_SRC)
	$(CC) $(TEST_SRC) $(TEST_INCLUDE_SRC) $(CFLAGS) -o $(TEST_TARGET)

build_and_test: $(SRC) $(TEST_SRC)
	$(CC) $(TEST_SRC) $(TEST_INCLUDE_SRC) $(CFLAGS) -o $(TEST_TARGET)
	./$(TEST_TARGET)

r: run

b: build

br: build run

t: build_and_test

bt: build_test

.PHONY: build clean run test build_test r b br t bt
