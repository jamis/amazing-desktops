#include <stdlib.h>
#include <string.h>
#include "grid.h"

grid_t *grid_create(int width, int height) {
  grid_t *grid = (grid_t*)malloc(sizeof(grid_t));
  grid->width = width;
  grid->height = height;

  grid->cells = (cell_t**)malloc(height*sizeof(cell_t*));
  for(int i = 0; i < height; i++) {
    grid->cells[i] = (cell_t*)malloc(width*sizeof(cell_t));
    memset(grid->cells[i], 0, width);
  }

  return grid;
}

void grid_free(grid_t** grid) {
  for(int i = 0; i < (*grid)->height; i++) free((*grid)->cells[i]);
  free((*grid)->cells);
  (*grid)->cells = NULL;
  free(*grid);
  *grid = NULL;
}
