# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Target executable name
TARGET = chash

# Source files
SRC = chash.c hashdb.c rwlocks.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable from object files
$(TARGET): $(OBJ)
    $(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Rule to compile chash.c into chash.o
chash.o: chash.c hashdb.h rwlocks.h common.h common_threads.h
    $(CC) $(CFLAGS) -c chash.c

# Rule to compile hashdb.c into hashdb.o
hashdb.o: hashdb.c hashdb.h rwlocks.h common.h
    $(CC) $(CFLAGS) -c hashdb.c

# Rule to compile rwlocks.c into rwlocks.o
rwlocks.o: rwlocks.c rwlocks.h common.h
    $(CC) $(CFLAGS) -c rwlocks.c

# Clean up generated files
clean:
    rm -f $(TARGET) $(OBJ)
