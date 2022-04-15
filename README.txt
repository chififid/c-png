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