#include "binary_tree.h"

void binary_tree_run(algorithm_t *algo, grid_t *grid);

static algorithm_t static_binary_tree = {
  .prepare = NULL,
  .cleanup = NULL,
  .run = binary_tree_run
};

algorithm_t *binary_tree = &static_binary_tree;


void binary_tree_run(algorithm_t *algo, grid_t *grid) {
  int row, column;

  for(row = 0; row < grid->height; row++) {
    for(column = 0; column < grid->width; column++) {
      int dirs[2];
      int dir_count = 0;

      if(row > 0) dirs[dir_count++] = N;
      if(column > 0) dirs[dir_count++] = W;

      if (dir_count > 0) {
        int dir = dirs[rand() % dir_count];
        int nr, nc;

        nr = row + DY(dir);
        nc = column + DX(dir);

        AT(grid, row, column) |= dir;
        AT(grid, nr, nc) |= OPP(dir);
      }
    }
  }
}
