#include "read_png.h"
#include "decode.h"
#include "print_png.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	unsigned char *buf, *decode_res;
	unsigned long decode_length;
	struct png_inf *inf;
	int buf_length;

	if(argc != 2) {
		fprintf(stderr, "Error: Params invalid\n");
		return 4;
	}

	buf_length = read_png(&buf, argv[1]);
	decode_length = decode(&decode_res, &inf,  buf, buf_length);
	print_png(decode_res, decode_length, inf);

	return 0;
}