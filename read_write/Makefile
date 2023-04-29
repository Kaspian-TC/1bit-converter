CC = gcc
CFLAGS = -Wall -g 

all: 1bit_read_functions.o 1bit_write_functions.o


1bit_read_functions.o: 1bit_read_functions.c
	$(CC) $(CFLAGS) -c 1bit_read_functions.c -o $@
1bit_write_functions.o: 1bit_write_functions.c
	$(CC) $(CFLAGS) -c 1bit_write_functions.c -o $@

clean:
	del *.o