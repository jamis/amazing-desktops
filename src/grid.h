#ifndef __GRID_H__
#define __GRID_H__

#include <stdlib.h>

typedef uint8_t  cell_t;
typedef uint32_t seed_t;

typedef struct {
  cell_t **cells;
  int      width;
  int      height;
} grid_t;

#define N ( 0x01 )
#define S ( 0x02 )
#define E ( 0x04 )
#define W ( 0x08 )

#define OPP(dir) \
  ((dir == N) ? S : \
   ((dir == S) ? N : \
    ((dir == E) ? W : E)))

#define DX(dir) \
  ((dir == E) ? 1 : \
   ((dir == W) ? -1 : 0))

#define DY(dir) \
  ((dir == S) ? 1 : \
   ((dir == N) ? -1 : 0))

#define HAS(cell, dir) (((cell) & (dir)) == dir)

#define ROW(seed) (((seed) >> 16) & 0xFFFF)
#define COL(seed) ((seed) & 0xFFFF)

#define TO_SEED(row, col) ( ((row) << 16) + (col) )
#define FROM_SEED(seed, row, col) { row = ROW(seed); col = COL(seed); }

#define AT(grid, row, col) ((grid)->cells[(row)][(col)])

grid_t *grid_create(int width, int height);
void grid_free(grid_t** grid);

#endif
