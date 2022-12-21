CC = gcc
CFLAGS = -Wall -g 
INPUT_PPM = anime_image
1BIT_FILE = anime_image
PNG_NAME = david
1BIT_VER = .1

1bit_converter.exe: main_code.c imgutils.o 1bit_funcs.o 1bit_read_functions.o
	$(CC) $(CFLAGS) -o $@ main_code.c imgutils.o 1bit_funcs.o 1bit_read_functions.o
imgutils.o: imgutils.c
	$(CC) $(CFLAGS) -c imgutils.c -o $@
1bit_funcs.o: 1bit_funcs.c
	$(CC) $(CFLAGS) -c 1bit_funcs.c -o $@
1bit_read_functions.o: 1bit_read_functions.c
	$(CC) $(CFLAGS) -c 1bit_read_functions.c -o $@

create: 1bit_converter.exe
	$^ -i "ppm images"/$(INPUT_PPM).ppm "1bit images"/$(INPUT_PPM)_2.1bit -c $(1BIT_VER) -d
read: 1bit_converter.exe
	$^ -i "1bit images"/$(1BIT_FILE).1bit "ppm images"/$(1BIT_FILE)_test.ppm -r
upgrade: 1bit_converter.exe
	$^ -i "1bit images"/$(1BIT_FILE).1bit $(1BIT_FILE)_2.1bit -u $(1BIT_VER)
png: 1bit_converter.exe
	$^ -i "png images"/$(PNG_NAME).png "1bit images"/$(PNG_NAME).1bit -c $(1BIT_VER) -d
	$^ -i "1bit images"/$(PNG_NAME).1bit $(PNG_NAME)_2.ppm -r
	ffmpeg -i $(PNG_NAME)_2.ppm "PNG images"/1bit_$(PNG_NAME).png -loglevel quiet -y
	del $(PNG_NAME)_2.ppm
gdb: 1bit_converter.exe
	gdb 1bit_converter.exe
clean:
	del 1bit_converter.exe
	del *.o