#ifndef __COLORS_H__
#define __COLORS_H__

#include <stdlib.h>

typedef uint32_t rgb_t;

void hsv2rgb(
  uint8_t *r, uint8_t *g, uint8_t *b,
  float h, float s, float v);

#endif
