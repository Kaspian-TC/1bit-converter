CC = gcc
CFLAGS = -Wall -g 
SRCS = main_code.c 1bit_funcs.c imgutils.c read_write/1bit_read_functions.c read_write/1bit_write_functions.c read_write/huffman.c data_types/minheap.c data_types/1bit_types.c
OBJS = $(SRCS:.c=.o)

# 1bit_converter.exe: main_code.c imgutils.o 1bit_funcs.o 1bit_read_functions.o 1bit_write_functions.o
	# $(CC) $(CFLAGS) -o $@ main_code.c imgutils.o 1bit_funcs.o 1bit_read_functions.o 1bit_write_functions.o
# imgutils.o: imgutils.c
	# $(CC) $(CFLAGS) -c imgutils.c -o $@
# 1bit_funcs.o: 1bit_funcs.c
	# $(CC) $(CFLAGS) -c 1bit_funcs.c -o $@
# 1bit_read_functions.o: 1bit_read_functions.c
	# $(CC) $(CFLAGS) -c 1bit_read_functions.c -o $@
# 1bit_write_functions.o: 1bit_write_functions.c
	# $(CC) $(CFLAGS) -c 1bit_write_functions.c -o $@
# Define the source files and object files

# Define the main target
1bit_converter.exe: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Define the pattern rule for compiling .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Define a variable for the subdirectory
SUBDIR = read_write

# Define dependencies for the files in the subdirectory
$(SUBDIR)/%.o: $(SUBDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

INPUT_PPM = david
1BIT_FILE = anime_image
PNG_NAME = Spirited_Sea.png
1BIT_VER = .2

create: 1bit_converter.exe
	$^ -i "ppm images"/$(INPUT_PPM).ppm "1bit images"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER) 
read: 1bit_converter.exe
	$^ -i "1bit images"/$(1BIT_FILE).1bit "ppm images"/$(1BIT_FILE)_test.ppm -r
upgrade: 1bit_converter.exe
	$^ -i "1bit images"/$(1BIT_FILE).1bit $(1BIT_FILE)_2.1bit -u $(1BIT_VER)
png: 1bit_converter.exe
	$^ -i "png images"/$(PNG_NAME) "1bit images"/$(PNG_NAME).1bit -c $(1BIT_VER) -d bayer-2
	$^ -i "1bit images"/$(PNG_NAME).1bit "PNG images"/1bit_$(PNG_NAME).png -r
gdb: 1bit_converter.exe
	gdb 1bit_converter.exe
gdb-create:
	gdb 1bit_converter.exe -ex "run -i \"ppm images\"/$(INPUT_PPM).ppm \"1bit images\"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER)"
clean:
	del 1bit_converter.exe
	del /s *.o
	