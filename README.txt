Render png in console

Compile modules:
gcc -Wall -g -c read_png.c
gcc -Wall -g -c decode.c
gcc -Wall -g -c print_png.c

Compile main:
gcc -Wall -g main.c decode.o read_png.o print_png.o -o main -I/home/freedomsky/c/libs/zlib/include -L/home/freedomsky/c/libs/zlib/lib -l:libz.a

Run:
./main filename.png

Tests:
./main imgs/small.png
./main imgs/big.png

Askii set:
32, 46, 39, 96, 44, 34, 94, 42, 58, 59, 45, 126, 43, 124, 47, 92, 40, 41, 33, 93, 91, 60, 123, 125, 62, 61, 63, 37, 38, 36, 64, 35