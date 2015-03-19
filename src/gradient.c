#include <stdlib.h>
#include "gradient.h"

gradient_t gr_autumn = {
  .colors = { 0x9a95deff, 0x585680ff, 0x568071ff, 0x7cb9a4ff,
              0x97b97cff, 0x586c49ff, 0x6c6249ff, 0xccb88aff,
              0xcc8f8aff, 0x5c403eff },
  .size = 10
};

rgb_t gradient_at(int n, int max, gradient_t *gradient)
{
  int product = n * (gradient->size - 1);
  int element = product / max;
  int t = product % max;

  int r = (gradient->colors[element] >> 24) & 0xFF;
  int g = (gradient->colors[element] >> 16) & 0xFF;
  int b = (gradient->colors[element] >>  8) & 0xFF;
  int a = (gradient->colors[element] >>  0) & 0xFF;

  if (t > 0) {
    int ur = (gradient->colors[element+1] >> 24) & 0xFF;
    int ug = (gradient->colors[element+1] >> 16) & 0xFF;
    int ub = (gradient->colors[element+1] >>  8) & 0xFF;
    int ua = (gradient->colors[element+1] >>  0) & 0xFF;

    r += (ur - r) * t / max;
    g += (ug - g) * t / max;
    b += (ub - b) * t / max;
    a += (ua - a) * t / max;
  }

  return ((r << 24) + (g << 16) + (b << 8) + a);
}

void gradient_random(gradient_t *gradient, int size)
{
  float h, s, v;

  gradient->size = size;

  h = rand() % 3600 / 10.0;
  s = 1.0 * rand() / RAND_MAX;
  v = 0.2 + 0.8 * rand() / RAND_MAX;

  for(int i = 0; i < size; i++) {
    uint8_t r, g, b;

    hsv2rgb(&r, &g, &b, h, s, v);
    gradient->colors[i] = ((r << 24) + (g << 16) + (b << 8) + 0xff);

    h += ((rand() % 1200) / 10.0) - 60.0;
    s += ((rand() % 40) / 100.0) - 0.2;
    v += ((rand() % 40) / 100.0) - 0.2;

    if(h < 0) h += 360;
    if(h > 360) h -= 360;
    if(s < 0) s = 0;
    if(s > 1) s = 1;
    if(v < 0) v = 0;
    if(v > 1) v = 1;
  }
}

