#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "grid.h"

typedef struct algorithm algorithm_t;

struct algorithm {
  void (*prepare)(algorithm_t*, grid_t*);
  void (*cleanup)(algorithm_t*, grid_t*);
  void (*run)(algorithm_t*, grid_t*);
};

void algorithm_run(algorithm_t *algo, grid_t *grid);

#endif
