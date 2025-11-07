
# Compiler to use
CC = gcc

# flags 
flags =-Wall -Werror

# Name of your executable
TARGET = server

# Source files
SRC = server.c

# Default rule (runs when you just type `make`)
all: $(TARGET)

# How to build the executable
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Remove compiled files
clean:
	rm -f $(TARGET)
