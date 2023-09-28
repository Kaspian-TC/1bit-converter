CC = gcc
CFLAGS = -Wall -g 
EXE_NAME = 1bit_converter.exe
SRC_DIR = .
SRCS := $(wildcard *.c) $(wildcard */*.c)
OBJS = $(SRCS:.c=.o)
ifeq ($(OS),Windows_NT)
    PLATFORM_OS=WINDOWS
else
	PLATFORM_OS=UNIX
endif

# main target
$(EXE_NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# pattern rule for compiling .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


INPUT_PPM = david
1BIT_FILE = anime_image
PNG_NAME = david.png
1BIT_VER = .1

# phony targets to help with testing
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

# phony target to clean up the directory
clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del /Q /S $(EXE_NAME) *.o
else
	rm -rf $(EXE_NAME)
	rm -rf *.o
endif

	