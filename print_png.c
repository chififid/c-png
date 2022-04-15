#include "decode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static  unsigned char rgba_to_monochrome(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned char rgb = (0.2125 * r) + (0.7154 * g) + (0.0721 * b);
	return rgb * ((double)a / (double)255);
}

static void parseSubFilter(unsigned char *dest, unsigned char *data, int pos, int length)
{
	unsigned char last_px[] = {0, 0, 0, 0};
	int i;

	for(i = 0; i < length; i++) {
		unsigned char px[4];
		int j;
		for(j = 0; j < 4; j++) {
			px[j] = (data[pos + i * 4 + j] + last_px[j]) % 256;
			dest[j] = last_px[j] = px[j];
		}

		dest += 4;
	}
}

static void parseUpFilter(unsigned char *dest, unsigned char *data, int pos, int length, unsigned char *pxs_last)
{
	int i;

	for(i = 0; i < length; i++) {
		unsigned char px[4], top_px[4];
		int j;

		for(j = 0; j < 4; j++) {
			top_px[j] = pxs_last[i * 4 + j];
			px[j] = (data[pos + i * 4 + j] + top_px[j]) % 256;
			dest[j] = px[j];
		}

		dest += 4;
	}
}


static void parseAverageFilter(unsigned char *dest, unsigned char *data, int pos, int length, unsigned char *pxs_last)
{
	unsigned char last_px[] = {0, 0, 0, 0};
	int i;

	for(i = 0; i < length; i++) {
		unsigned char px[4], top_px[4];
		int j;

		for(j = 0; j < 4; j++) {
			top_px[j] = pxs_last[i * 4 + j];
			px[j] = (data[pos + i * 4 + j] + (top_px[j] + last_px[j]) / 2) % 256;
			dest[j] = last_px[j] = px[j];
		}

		dest += 4;
	}
}

static int absN(int n) {
	return n < 0 ? -n : n;
}

static void parsePaethFilter(unsigned char *dest, unsigned char *data, int pos, int length, unsigned char *pxs_last)
{
	int i;

	unsigned char last_px[] = {0, 0, 0, 0}, top_left_px[] = {0, 0, 0, 0};
	for(i = 0; i < length; i++) {
		unsigned char px[4], top_px[4];
		int j;

		for(j = 0; j < 4; j++) {
			int p, pa, pb, pc;
			top_px[j] = pxs_last[i * 4 + j];

			p = last_px[j] + top_px[j] - top_left_px[j];
			pa = absN(p - last_px[j]);
			pb = absN(p - top_px[j]);
			pc = absN(p - top_left_px[j]);

			if (pa <= pb && pa <= pc) {
				px[j] = data[pos + i * 4 + j] + last_px[j];
			} else if (pb <= pc) {
				px[j] = data[pos + i * 4 + j] + top_px[j];
			} else {
				px[j] = data[pos + i * 4 + j] + top_left_px[j];
			}

			dest[j] = last_px[j] = px[j];
			top_left_px[j] = top_px[j];
		}

		dest += 4;
	}
}

int print_png(unsigned char *decode_res, unsigned long decode_length, struct png_inf *inf)
{
	int line_length, pos = 0;
	unsigned char *pxs_last;
	int chars_len = 32;
	char chars[chars_len];
	int chi;


	for(chi = 33; chi < 65; chi++) {
		chars[chi-33] = chi;
	}

	line_length = inf->width * 4 + 1;

	while(pos < decode_length) {
		unsigned char *pxs = malloc(line_length * sizeof(char) - 1);
		int i;

		switch (decode_res[pos]) {
		case 0:
			memcpy(pxs, decode_res + pos + 1, line_length * sizeof(char) - 1);
			break;
		case 1:
			parseSubFilter(pxs, decode_res, pos + 1, inf->width);
			break;
		case 2:
			parseUpFilter(pxs, decode_res, pos + 1, inf->width, pxs_last);
			break;
		case 3:
			parseAverageFilter(pxs, decode_res, pos + 1, inf->width, pxs_last);
			break;
		case 4:
			parsePaethFilter(pxs, decode_res, pos + 1, inf->width, pxs_last);
			break;
		}

		pxs_last = pxs;

		for(i = 0; i < inf->width; i++) {
			int r, g, b, a;
			unsigned char monochrome_px, askii_px;

			r = pxs[i * 4];
			g = pxs[i * 4 + 1];
			b = pxs[i * 4 + 2];
			a = pxs[i * 4 + 3];
			monochrome_px = rgba_to_monochrome(r, g, b, a);
			askii_px = chars[monochrome_px / (256 / chars_len)];
			printf("%c%c", askii_px, askii_px);
		}

		pos += line_length;
		printf("\n");
	}

	return 0;
}