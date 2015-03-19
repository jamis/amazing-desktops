#include "aldous_broder.h"

static void aldous_broder_run(algorithm_t *algo, grid_t *grid);

static algorithm_t static_aldous_broder = {
  .prepare = NULL,
  .cleanup = NULL,
  .run = aldous_broder_run
};

algorithm_t *aldous_broder = &static_aldous_broder;


static void aldous_broder_run(algorithm_t *algo, grid_t *grid) {
  int row, col;
  int count;

  count = grid->width * grid->height - 1;
  row = rand() % grid->height;
  col = rand() % grid->width;

  while(count > 0) {
    int dirs[4];
    int dir_count = 0;

    if (row > 0) dirs[dir_count++] = N;
    if (col > 0) dirs[dir_count++] = W;
    if (row+1 < grid->height) dirs[dir_count++] = S;
    if (col+1 < grid->width) dirs[dir_count++] = E;

    int dir = dirs[rand() % dir_count];

    int nr = row + DY(dir);
    int nc = col + DX(dir);

    if (AT(grid, nr, nc) == 0) {
      AT(grid, row, col) |= dir;
      AT(grid, nr, nc) |= OPP(dir);
      count--;
    }

    row = nr;
    col = nc;
  }
}
