CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -g
TARGET = jsh
SOURCES = main.c parser.c executor.c jobs.c
HEADERS = jsh.h parser.h executor.h jobs.h
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

# Dependencies
main.o: main.c jsh.h parser.h executor.h jobs.h
parser.o: parser.c parser.h jsh.h jobs.h
executor.o: executor.c executor.h jsh.h parser.h jobs.h
jobs.o: jobs.c jobs.h jsh.h
