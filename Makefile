CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

TARGET = process-monitor

SOURCES = src/main.c src/process.c src/system_info.c src/search.c src/sorting.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o process-monitor test-search test-sorting

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

test-search:
	$(CC) $(CFLAGS) tests/test_search.c src/search.c src/process.c -o test-search
	./test-search

test-sorting:
	$(CC) $(CFLAGS) tests/test_sorting.c src/sorting.c -o test-sorting
	./test-sorting

test: test-search test-sorting

.PHONY: all clean test test-search test-sorting