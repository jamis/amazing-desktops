#ifndef __DISTANCES_H__
#define __DISTANCES_H__

#include "grid.h"

typedef struct {
  int **matrix;
  int width;
  int height;
  int max;
  seed_t max_cell;
} distances_t;

typedef struct {
  seed_t *steps;
  int length;
} path_t;

distances_t *dijkstra(grid_t *grid, seed_t *starts, int count);
void distances_free(distances_t **distances);

path_t *path_find(grid_t *grid, distances_t *distances, seed_t goal);
void path_free(path_t **path);

#endif
