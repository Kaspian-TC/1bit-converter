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
	EXE_NAME = ./1bit_converter.exe
	CFLAGS += -lm
endif

# main target
$(EXE_NAME): $(OBJS)
	$(CC) $^ -o $@  $(CFLAGS)

# pattern rule for compiling .c files into .o files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)


INPUT_PPM = david
1BIT_FILE = anime_image
PNG_NAME = david.png
1BIT_VER = .0

# phony targets to help with testing
create: $(EXE_NAME)
	$(EXE_NAME) -i "ppm images"/$(INPUT_PPM).ppm "1bit images"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER) 
read: $(EXE_NAME)
	$(EXE_NAME) -i "1bit images"/$(1BIT_FILE).1bit "ppm images"/$(1BIT_FILE)_test.ppm -r
upgrade: $(EXE_NAME)
	$(EXE_NAME) -i "1bit images"/$(1BIT_FILE).1bit $(1BIT_FILE)_2.1bit -u $(1BIT_VER)
png: $(EXE_NAME)
	$(EXE_NAME) -i "png images"/$(PNG_NAME) "1bit images"/$(PNG_NAME).1bit -c $(1BIT_VER) -d bayer-2
	$(EXE_NAME) -i "1bit images"/$(PNG_NAME).1bit "PNG images"/1bit_$(PNG_NAME).png -r
gdb: $(EXE_NAME)
	gdb $(EXE_NAME)
gdb-create:
	gdb $(EXE_NAME) -ex "run -i \"ppm images\"/$(INPUT_PPM).ppm \"1bit images\"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER)"
gdb-read:
	gdb $(EXE_NAME) -ex "run -i \"1bit images\"/$(PNG_NAME).1bit \"PNG images\"/1bit_$(PNG_NAME).png -r"

# phony target to clean up the directory
clean:
ifeq ($(PLATFORM_OS),WINDOWS)
	del /Q /S $(EXE_NAME) *.o
else
	rm -rf $(EXE_NAME)
	rm -rf *.o
	rm -rf read_write/*.o
	rm -rf data_types/*.o
endif

	