/*
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 * Copyright 2008 James Bursa <james@netsurf-browser.org>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libnsbmp.h>

#define BITMAP_BYTES_PER_PIXEL 4

unsigned char *load_file(const char *path, size_t *data_size);
void warning(const char *context, bmp_result code);
void *bitmap_create(int width, int height, unsigned int state);
void bitmap_set_suspendable(void *bitmap, void *private_word,
		void (*invalidate)(void *bitmap, void *private_word));
void invalidate(void *bitmap, void *private_word);
unsigned char *bitmap_get_buffer(void *bitmap);
size_t bitmap_get_bpp(void *bitmap);
void bitmap_destroy(void *bitmap);


int main(int argc, char *argv[])
{
	bmp_bitmap_callback_vt bitmap_callbacks = {
		bitmap_create,
		bitmap_destroy,
		bitmap_set_suspendable,
		bitmap_get_buffer,
		bitmap_get_bpp
	};
	bmp_result code;
	ico_collection ico;
	struct bmp_image *bmp;
	size_t size;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s collection.ico\n", argv[0]);
		return 1;
	}

	/* create our bmp image */
	ico_collection_create(&ico, &bitmap_callbacks);

	/* load file into memory */
	unsigned char *data = load_file(argv[1], &size);

	/* analyse the BMP */
	code = ico_analyse(&ico, size, data);
	if (code != BMP_OK) {
		warning("ico_analyse", code);
		exit(1);
	}

	printf("P3\n");
	printf("# %s\n", argv[1]);
	printf("# width                %u \n", ico.width);
	printf("# height               %u \n", ico.height);
	printf("%u %u 256\n", ico.width, ico.height);

	/* decode the image */
	bmp = ico_find(&ico, 255, 255);
	assert(bmp);
	bmp_decode(bmp);
	{
		unsigned int row, col;
		unsigned char *image;
		image = (unsigned char *) bmp->bitmap;
		for (row = 0; row != bmp->height; row++) {
			for (col = 0; col != bmp->width; col++) {
				size_t z = (row * bmp->width + col) * BITMAP_BYTES_PER_PIXEL;
				printf("%u %u %u ",
					(unsigned char) image[z],
					(unsigned char) image[z + 1],
					(unsigned char) image[z + 2]);
			}
			printf("\n");
		}
	}

	/* clean up */
	bmp_finalise(bmp);
	ico_finalise(&ico);
	free(data);

	return 0;
}


unsigned char *load_file(const char *path, size_t *data_size)
{
	FILE *fd;
	struct stat sb;
	unsigned char *buffer;
	size_t size;
	size_t n;

	fd = fopen(path, "rb");
	if (!fd) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	if (stat(path, &sb)) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	size = sb.st_size;

	buffer = malloc(size);
	if (!buffer) {
		fprintf(stderr, "Unable to allocate %lld bytes\n",
				(long long) size);
		exit(EXIT_FAILURE);
	}

	n = fread(buffer, 1, size, fd);
	if (n != size) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	fclose(fd);

	*data_size = size;
	return buffer;
}


void warning(const char *context, bmp_result code)
{
	fprintf(stderr, "%s failed: ", context);
	switch (code) {
		case BMP_INSUFFICIENT_MEMORY:
			fprintf(stderr, "BMP_INSUFFICIENT_MEMORY");
			break;
		case BMP_INSUFFICIENT_DATA:
			fprintf(stderr, "BMP_INSUFFICIENT_DATA");
			break;
		case BMP_DATA_ERROR:
			fprintf(stderr, "BMP_DATA_ERROR");
			break;
		default:
			fprintf(stderr, "unknown code %i", code);
			break;
	}
	fprintf(stderr, "\n");
}


void *bitmap_create(int width, int height, unsigned int state)
{
	(void) state;  /* unused */
	return calloc(width * height, BITMAP_BYTES_PER_PIXEL);
}


void bitmap_set_suspendable(void *bitmap, void *private_word,
			     void (*invalidate)(void *bitmap, void *private_word))
{
	(void) bitmap;  /* unused */
	(void) private_word;  /* unused */
	(void) invalidate;  /* unused */
}


void invalidate(void *bitmap, void *private_word)
{
	(void) bitmap;  /* unused */
	(void) private_word;  /* unused */
}


unsigned char *bitmap_get_buffer(void *bitmap)
{
	assert(bitmap);
	return bitmap;
}


size_t bitmap_get_bpp(void *bitmap)
{
	(void) bitmap;  /* unused */
	return BITMAP_BYTES_PER_PIXEL;
}


void bitmap_destroy(void *bitmap)
{
	assert(bitmap);
	free(bitmap);
}

