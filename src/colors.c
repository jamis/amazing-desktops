#include <math.h>
#include "colors.h"

void hsv2rgb(
  uint8_t *r, uint8_t *g, uint8_t *b,
  float h, float s, float v)
{
  int i;
  float f, p, q, t;

  if (s == 0) {
    *r = *g = *b = roundf(v * 255);
    return;
  }

  h /= 60;
  i = floor(h);
  f = h - i;
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));

  uint8_t vi = roundf(v * 255);
  uint8_t pi = roundf(p * 255);
  uint8_t qi = roundf(q * 255);
  uint8_t ti = roundf(t * 255);

  switch(i) {
    case 0:
      *r = vi;
      *g = ti;
      *b = pi;
      break;
    case 1:
      *r = qi;
      *g = vi;
      *b = pi;
      break;
    case 2:
      *r = pi;
      *g = vi;
      *b = ti;
      break;
    case 3:
      *r = pi;
      *g = qi;
      *b = vi;
      break;
    case 4:
      *r = ti;
      *g = pi;
      *b = vi;
      break;
    default:
      *r = vi;
      *g = pi;
      *b = qi;
      break;
  }
}
