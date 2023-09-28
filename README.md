# 1bit file converter

1bit is a file type that only stores two colours, 1 and 0. Hence, each pixel
is represented with at most 1 bit. This project is for converting different 
image file types to 1bit, and converting them back. As of last writing this, 
this project supports various types of dithering, run length encoding, and the
 ability to change encoding for existing files.

### Compiling
You can compile 1bit-converter.exe by getting the repository and running make. 
Currently it has only been tested on Windows, but other operating systems 
should work since no code uses anything outside of standard c99.

### Converting from your image file type to 1bit
To convert from a file type, you need to include some flags.

For all 1bit operations, you need to include the flag `-i` followed by an input
 file and and output file. Then to specify you want to create a new 1bit, add
 the `-c` flag. 
To specify the type of compression, type `.0` or `.1` after `-c` for no 
compression and run-length compression respectively. 
Currently supported file types are: `.png`, `.jpg`, and `.ppm`
An example command would look like: 
``` 1bit_converter.exe -i david.png david.1bit -c .0 ```

### Converting from 1bit to your image file type
The process is the same as for creating a 1bit file, however instead of typing
the `-c` flag put `-r`. You do not need to specify the compression type. 
Currently supported file types are: `.png`, `.jpg`, and `.ppm`
An example command would look like: 
``` 1bit_converter.exe -i david.1bit david.png -r```
### Dithering
1bit can dither your images in three ways. The first is a simple threashold 
dither that takes the average luminance (brightness) and returns an image where
 each pixel is black if is below that luminance and white if it is above it. 
 This is the default type of dithering applied. The second is 
 [floyd-steinberg](https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering).
 And the third is [bayer](https://en.wikipedia.org/wiki/Ordered_dithering) dithering. 
To specify other types of dithering, add the `-d` flag followed by the type of 
dithering desired. Currently there is support for `floyd-steinberg`,`bayer-0`,
`bayer-1`, and `bayer-2`.

### Upgrading
You can change from `.0` to `.1` by using the `-u` flag instead of `-c` or `-r`
. 

### Todo list
- [ ] Huffman encoding
- [ ] Suport for different colours
- [ ] Lempel-Zev compression implementaion
- [ ] DEFALTE (LZ + huffman)
- [ ] Different error diffusion dithering algorithms
- [ ] Changing .1 to .0
- [ ] Change folder names so that it can be compatible with both unix and windows environments

### Examples
[![I dithered this image](https://upload.wikimedia.org/wikipedia/commons/c/c1/Michelangelo%27s_David_-_Floyd-Steinberg.png)](https://upload.wikimedia.org/wikipedia/commons/c/c1/Michelangelo%27s_David_-_Floyd-Steinberg.png)
