#include "read_png.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static int is_bufs_same(unsigned char *buf_1, unsigned char *buf_2, int buf_length)
{
	do {
		buf_length--;
	} while(buf_1[buf_length] == buf_2[buf_length] && buf_length >= 0);
	return buf_length == -1 ? 1 : 0;
}

int read_png(unsigned char **res, const char *file_name)
{
	int fd, cs, file_size, read_size;
	unsigned char *buf;
	unsigned char png_magic[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

	fd = open(file_name, O_RDONLY);
	if(fd == -1) {
		perror(file_name);
		exit(1);
	}

	file_size = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);

	buf = malloc(sizeof(char) * file_size);
	read_size = read(fd, buf, file_size);
	if(read_size != file_size) {
		perror(file_name);
		exit(1);
	}

	if(!is_bufs_same(png_magic, buf, 8)) {
		fprintf(stderr, "%s: It's not png file\n", file_name);
		exit(1);
	}

	cs = close(fd);
	if(cs == -1) {
		perror(file_name);
		exit(1);
	}

	*res = buf;

	return read_size;
}