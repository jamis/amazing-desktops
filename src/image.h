#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdlib.h>

#define GETPX(img, x, y) \
  (&((img)->pixels[(y)*(img)->width + (x)]))

#define SETPX(img, x, y, color) GETPX((img), (x), (y))->rgba = (color)

typedef union {
  uint32_t rgba;
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  } __attribute__ ((__packed__));
} pixel_t;

typedef struct {
  int width;
  int height;
  pixel_t *pixels;
} image_t;

image_t *image_create(size_t width, size_t height);
void image_free(image_t *img);
void image_save(image_t *img, char *name);

#endif
