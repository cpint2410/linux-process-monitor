CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = process-monitor

SRC = src/main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)