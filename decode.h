#ifndef DECODE_H
#define DECODE_H

struct png_inf
{
	int width;
	int height;
	unsigned char bit_depth;
	unsigned char colour_type;
	unsigned char compression_method;
	unsigned char filter_method;
	unsigned char interlaced_method;
};

int decode(unsigned char **dest, struct png_inf **inf, unsigned char *buf, int length);
#endif