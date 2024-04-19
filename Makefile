CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = chash
SRC = chash.c hashdb.c rwlocks.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

chash.o: chash.c hashdb.h rwlocks.h common.h common_threads.h
	$(CC) $(CFLAGS) -c chash.c

hashdb.o: hashdb.c hashdb.h rwlocks.h common.h
	$(CC) $(CFLAGS) -c hashdb.c

rwlocks.o: rwlocks.c rwlocks.h common.h
	$(CC) $(CFLAGS) -c rwlocks.c

clean:
	rm -f $(TARGET) $(OBJ)
