CC = gcc
CFLAGS = -Wall -Wextra -g

BUILD_DIR = ./build

all: $(BUILD_DIR)/example

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/arena.o: ./arena/arena.c ./arena/arena.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/example.o: ./example/example.c ./example/example.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(BUILD_DIR)/example: $(BUILD_DIR)/arena.o $(BUILD_DIR)/example.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

run:
	$(BUILD_DIR)/example

clean:
	rm -rf ./build
.PHONY: all run clean
