#include "recursive_subdivision.h"

static void recursive_subdivision_prepare(algorithm_t *algo, grid_t *grid);
static void recursive_subdivision_run(algorithm_t *algo, grid_t *grid);

static void split(grid_t *grid, int row, int col, int height, int width);
static void split_horizontally(grid_t *grid, int row, int col, int height, int width);
static void split_vertically(grid_t *grid, int row, int col, int height, int width);

static algorithm_t static_recursive_subdivision = {
  .prepare = recursive_subdivision_prepare,
  .cleanup = NULL,
  .run = recursive_subdivision_run
};

algorithm_t *recursive_subdivision = &static_recursive_subdivision;


static void recursive_subdivision_prepare(algorithm_t *algo, grid_t *grid)
{
  for(int row = 0; row < grid->height; row++) {
    for(int col = 0; col < grid->width; col++) {
      int dirs = 0;

      if(row > 0) dirs |= N;
      if(row+1 < grid->height) dirs |= S;
      if(col > 0) dirs |= W;
      if(col+1 < grid->width) dirs |= E;

      AT(grid, row, col) = dirs;
    }
  }
}

static void recursive_subdivision_run(algorithm_t *algo, grid_t* grid)
{
  split(grid, 0, 0, grid->height, grid->width);
}

static void split(grid_t *grid, int row, int col, int height, int width)
{
  if (height <= 1 || width <= 1)
    return;

  if (height > width)
    split_horizontally(grid, row, col, height, width);
  else
    split_vertically(grid, row, col, height, width);
}

static void split_horizontally(grid_t *grid, int row, int col, int height, int width)
{
  int split_south_of = rand() % (height-1);
  int passage_at = rand() % width;

  for(int x = 0; x < width; x++) {
    if(x == passage_at) continue;

    AT(grid, row+split_south_of, col+x) &= ~S;
    AT(grid, row+split_south_of+1, col+x) &= ~N;
  }

  split(grid, row, col, split_south_of+1, width);
  split(grid, row+split_south_of+1, col, height-split_south_of-1, width);
}

static void split_vertically(grid_t *grid, int row, int col, int height, int width)
{
  int split_east_of = rand() % (width-1);
  int passage_at = rand() % height;

  for(int y = 0; y < height; y++) {
    if (y == passage_at) continue;

    AT(grid, row+y, col+split_east_of) &= ~E;
    AT(grid, row+y, col+split_east_of+1) &= ~W;
  }

  split(grid, row, col, height, split_east_of+1);
  split(grid, row, col+split_east_of+1, height, width-split_east_of-1);
}
