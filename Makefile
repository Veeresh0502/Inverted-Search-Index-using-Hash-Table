# Makefile for Inverted Search Index

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = inverted_search

# Source files
SRCS = main.c \
       validate.c \
       create.c \
       display.c \
       search.c \
       save.c \
       update.c \
       function.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header file
HEADER = main.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Build successful! Executable: $(TARGET)"

# Compile source files to object files
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run