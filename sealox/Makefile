MAKEFLAGS += --silent

SRC = $(wildcard src/*.c)
BIN_DIR = bin
TARGET = $(BIN_DIR)/sealox
CC = gcc
CFLAGS = -g

build: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BIN_DIR)
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(BIN_DIR)/* 2>/dev/null

run:
	./$(TARGET)

r: run

b: build

br: build run

.PHONY: build clean run r b br
