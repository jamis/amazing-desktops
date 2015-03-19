#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#define MAX_GRADIENT_SIZE (100)

#include "colors.h"

typedef struct {
  rgb_t   colors[MAX_GRADIENT_SIZE];
  uint8_t size;
} gradient_t;

extern gradient_t gr_autumn;

rgb_t gradient_at(int n, int max, gradient_t *gradient);
void gradient_random(gradient_t *gradient, int size);

#endif
