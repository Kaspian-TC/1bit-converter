CC = gcc
CFLAGS = -Wall -g 
EXE_NAME = 1bit_converter.exe
SRC_DIR = .
# SRCS = main_code.c 1bit_funcs.c imgutils.c read_write/1bit_read_functions.c read_write/1bit_write_functions.c read_write/huffman.c data_types/minheap.c data_types/1bit_types.c
SRCS := $(wildcard *.c) $(wildcard */*.c)
OBJS = $(SRCS:.c=.o)
ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
else
	PLATFORM_OS=UNIX
endif
# Define the main target
$(EXE_NAME): $(OBJS)
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

create: $(EXE_NAME)
	$^ -i "ppm images"/$(INPUT_PPM).ppm "1bit images"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER) 
read: $(EXE_NAME)
	$^ -i "1bit images"/$(1BIT_FILE).1bit "ppm images"/$(1BIT_FILE)_test.ppm -r
upgrade: $(EXE_NAME)
	$^ -i "1bit images"/$(1BIT_FILE).1bit $(1BIT_FILE)_2.1bit -u $(1BIT_VER)
png: $(EXE_NAME)
	$^ -i "png images"/$(PNG_NAME) "1bit images"/$(PNG_NAME).1bit -c $(1BIT_VER) -d bayer-2
	$^ -i "1bit images"/$(PNG_NAME).1bit "PNG images"/1bit_$(PNG_NAME).png -r
gdb: $(EXE_NAME)
	gdb $(EXE_NAME)
gdb-create:
	gdb $(EXE_NAME) -ex "run -i \"ppm images\"/$(INPUT_PPM).ppm \"1bit images\"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER)"

clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del /Q /S $(EXE_NAME) *.o
else
	rm -rf $(EXE_NAME)
	rm -rf *.o
endif

	