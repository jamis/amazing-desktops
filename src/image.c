#include <png.h>
#include <stdio.h>
#include <string.h>
#include "image.h"

image_t *image_create(size_t width, size_t height) {
  image_t *img = (image_t*)malloc(sizeof(image_t));
  img->width = width;
  img->height = height;
  img->pixels = (pixel_t*)malloc(sizeof(pixel_t) * width * height);
  memset(img->pixels, 0, sizeof(pixel_t) * width * height);
  return img;
}

void image_free(image_t* img) {
  free(img->pixels);
  img->width = img->height = 0;
  img->pixels = NULL;
  free(img);
}

void image_save(image_t *img, char *name) {
  FILE *fp;
  png_structp png = NULL;
  png_infop info = NULL;
  size_t x, y;
  png_byte **rows = NULL;

  fp = fopen(name, "wb");
  if (!fp) {
    printf("could not open file `%s'\n", name);
  } else {
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) {
      printf("could not create PNG struct\n");
    } else {
      info = png_create_info_struct(png);
      if (info == NULL) {
        printf("could not create info struct\n");
      } else {
        if (setjmp(png_jmpbuf(png))) {
          printf("error creating png\n");
        } else {
          png_set_IHDR(png, info,
                       img->width, img->height, 8,
                       PNG_COLOR_TYPE_RGB,
                       PNG_INTERLACE_NONE,
                       PNG_COMPRESSION_TYPE_DEFAULT,
                       PNG_FILTER_TYPE_DEFAULT);

          rows = png_malloc(png, img->height * sizeof(png_byte*));
          for (y = 0; y < img->height; y++) {
            png_byte *row = png_malloc(png, sizeof(uint8_t) * img->width * 3);
            rows[y] = row;
            for (x = 0; x < img->width; x++) {
              pixel_t *pixel = GETPX(img, x, y);
              *row++ = pixel->r;
              *row++ = pixel->g;
              *row++ = pixel->b;
            }
          }

          png_init_io(png, fp);
          png_set_rows(png, info, rows);
          png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

          for (y = 0; y < img->height; y++)
            png_free(png, rows[y]);
          png_free(png, rows);
        }
      }
      png_destroy_write_struct(&png, &info);
    }

    fclose(fp);
  }
}
