#include "decode.h"
#include "zlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF_CHUNK_TYPE "IHDR"
#define DATA_CHUNK_TYPE "IDAT"
#define DATA_END_CHUNK_TYPE "IEND"

#define FORMAT_ERROR() \
	fprintf(stderr, "File: This format isn't supported\n"); \
	exit(2);

struct png_chunk
{
	int total_length;
	int data_length;
	char *type;
	unsigned char *data;
	unsigned char *crc;
	struct png_chunk *next;
};

static int char_list_to_int(const unsigned char *buf)
{
	return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

static char *get_chunk_type(const unsigned char *buf)
{
	char *type = malloc(sizeof(char) * 5);
	int i;

	for(i=0; i < 4; i++) {
		type[i] = buf[i];
	}
	type[i] = 0;
	return type;
}

static unsigned char *get_chunk_src(const unsigned char *buf)
{
	unsigned char *src = malloc(sizeof(char) * 4);
	int i;

	for(i=0; i < 4; i++) {
		src[i] = buf[i];
	}
	return src;
}

static unsigned char *get_chunk_data(const unsigned char *buf, int length)
{
	unsigned char *data = malloc(sizeof(char) * length);
	int i;

	for(i = 0; i < length; i++) {
		data[i] = buf[i];
	}
	return data;
}

static struct png_chunk *generate_chunks(const unsigned char *bytes, int length)
{
	struct png_chunk *first = NULL, *last = NULL, *tmp;
	int pos;

	for(pos = 8; pos < length; pos += tmp->total_length) {
		tmp = malloc(sizeof(struct png_chunk));
		tmp->data_length = char_list_to_int(bytes + pos);
		tmp->total_length = tmp->data_length + 12;
		tmp->type = get_chunk_type(bytes + pos + 4);
		tmp->data = get_chunk_data(bytes + pos + 8, tmp->data_length);
		tmp->crc = get_chunk_src(bytes + pos + tmp->data_length + 8);
		tmp->next = NULL;

		if(last) {
			last = last->next = tmp;
		} else {
			first = last = tmp;
		}
	}

	return first;
}

static int is_string_the_same(char *str_1, char *str_2)
{
	for(; *str_1 == *str_2 && *str_1 != 0 && *str_2 != 0; str_1++, str_2++) {}
	return *str_1 == *str_2 ? 1 : 0;
}

static struct png_inf *get_png_inf(struct png_chunk *chunk)
{
	struct png_inf *inf;
	while(chunk) {
		if(is_string_the_same(chunk->type, INF_CHUNK_TYPE)) {
			unsigned char *chunk_data = chunk->data;

			inf = malloc(sizeof(struct png_inf));
			inf->width = char_list_to_int(chunk_data);
			inf->height = char_list_to_int(chunk_data + 4);
			inf->bit_depth = chunk_data[8];
			if(inf->bit_depth != 8) {
				FORMAT_ERROR()
			}
			inf->colour_type = chunk_data[9];
			if(inf->colour_type != 6) {
				FORMAT_ERROR()
			}
			inf->compression_method = chunk_data[10];
			if(inf->compression_method != 0) {
				FORMAT_ERROR()
			}
			inf->filter_method = chunk_data[11];
			if(inf->filter_method != 0) {
				FORMAT_ERROR()
			}
			inf->interlaced_method = chunk_data[12];
			if(inf->interlaced_method != 0) {
				FORMAT_ERROR()
			}
			return inf;
		}
		chunk = chunk->next;
	}
	FORMAT_ERROR()
}

static int get_data(unsigned char **dest, struct png_chunk *chunk, int length)
{
	unsigned char *data;
	int res_length = 0;
	data = malloc(length);

	while(chunk) {
		if(is_string_the_same(chunk->type, DATA_CHUNK_TYPE)) {
			memcpy(data + res_length, chunk->data, chunk->data_length);
			res_length += chunk->data_length;
		} else if(is_string_the_same(chunk->type, DATA_END_CHUNK_TYPE)) {
			*dest = data;
			return res_length;
		}
		chunk = chunk->next;
	}
	fprintf(stderr, "File: This file is broken\n");
	exit(2);
}

int decode(unsigned char **dest, struct png_inf **inf, unsigned char *buf, int length)
{
	struct png_chunk *chunks;
	unsigned char *data, *decode_res;
	int data_length, decode_status;
	unsigned long decompress_length;

	chunks = generate_chunks(buf, length);
	*inf = get_png_inf(chunks);
	data_length = get_data(&data, chunks, length);

	decompress_length = 4 * (*inf)->height * (*inf)->width + (*inf)->height;
	decode_res = malloc(sizeof(char) * (int)decompress_length);
	decode_status = uncompress(decode_res, &decompress_length, data, data_length);
	if (decode_status != Z_OK) {
		if (decode_status == Z_BUF_ERROR) {
			fprintf(stderr, "File: Decompress buf size small\n");
			exit(3);
		}
		fprintf(stderr, "File: Decompress failed\n");
		exit(3);
	}

	*dest = decode_res;
	return decompress_length;
}