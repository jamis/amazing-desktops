#include <math.h>
#include <stdlib.h>
#include "growing_tree.h"

#define MAX_WEIGHT (100)
#define MAX_SEARCH_DISTANCE (100)

typedef struct struct_growing_tree_algorithm growing_tree_algorithm_t;

struct struct_growing_tree_algorithm {
  algorithm_t base;
  int (*selector)(growing_tree_algorithm_t*, seed_t*, int);
  int jump_factor;
  uint8_t **weights;
};

static void growing_tree_mostly_longest_prepare(algorithm_t *base, grid_t *grid);
static void growing_tree_mostly_longest_cleanup(algorithm_t *algo, grid_t *grid);
static int growing_tree_mostly_longest_selector(growing_tree_algorithm_t *algo, seed_t *seeds, int count);

static void growing_tree_weighted_prepare(algorithm_t *base, grid_t *grid);
static void growing_tree_weighted_cleanup(algorithm_t *base, grid_t *grid);
static int growing_tree_weighted_selector(growing_tree_algorithm_t* algo, seed_t *seeds, int count);

static void growing_tree_run(algorithm_t *base, grid_t *grid);


static growing_tree_algorithm_t static_growing_tree_mostly_longest = {
  .base = {
    .prepare = growing_tree_mostly_longest_prepare,
    .cleanup = growing_tree_mostly_longest_cleanup,
    .run = growing_tree_run
  },
  .selector = growing_tree_mostly_longest_selector
};

static growing_tree_algorithm_t static_growing_tree_weighted = {
  .base = {
    .prepare = growing_tree_weighted_prepare,
    .cleanup = growing_tree_weighted_cleanup,
    .run = growing_tree_run
  },
  .selector = growing_tree_weighted_selector
};

algorithm_t *growing_tree_mostly_longest = (algorithm_t*)&static_growing_tree_mostly_longest;
algorithm_t *growing_tree_weighted = (algorithm_t*)&static_growing_tree_weighted;


void growing_tree_mostly_longest_prepare(algorithm_t *base, grid_t *grid) {
  growing_tree_algorithm_t *algo = (growing_tree_algorithm_t*)base;

  algo->jump_factor = sqrt(grid->width > grid->height ? grid->height : grid->width);
  if (algo->jump_factor < 5) algo->jump_factor = 5;
}

void growing_tree_mostly_longest_cleanup(algorithm_t *algo, grid_t *grid) {
}

int growing_tree_mostly_longest_selector(growing_tree_algorithm_t *algo, seed_t *seeds, int count) {

  int index = count-1;
  if (rand() % algo->jump_factor == 0) index = rand() % count;

  return index;
}

void growing_tree_weighted_prepare(algorithm_t *base, grid_t *grid) {
  growing_tree_algorithm_t *algo = (growing_tree_algorithm_t*)base;

  algo->weights = (uint8_t**)malloc(sizeof(uint8_t*) * grid->height);
  for(int i = 0; i < grid->height; i++) {
    algo->weights[i] = (uint8_t*)malloc(sizeof(uint8_t) * grid->width);
    for(int j = 0; j < grid->width; j++) {
      algo->weights[i][j] = rand() % (MAX_WEIGHT+1);
    }
  }
}

void growing_tree_weighted_cleanup(algorithm_t *base, grid_t *grid) {
  growing_tree_algorithm_t *algo = (growing_tree_algorithm_t*)base;

  for(int i = 0; i < grid->height; i++)
    free(algo->weights[i]);

  free(algo->weights);
  algo->weights = NULL;
}

int growing_tree_weighted_selector(growing_tree_algorithm_t* algo, seed_t *seeds, int count) {
  int i = count;
  int largest_weight = 0;
  int best_index;

  if (i > MAX_SEARCH_DISTANCE) i = MAX_SEARCH_DISTANCE;

  while (i > 0) {
    int row, col;
    FROM_SEED(seeds[count - i], row, col);
    if (algo->weights[row][col] > largest_weight) {
      largest_weight = algo->weights[row][col];
      best_index = count - i;
      if (largest_weight == MAX_WEIGHT) break;
    }
    i--;
  }

  return best_index;
}

void growing_tree_run(algorithm_t *base, grid_t *grid) {
  growing_tree_algorithm_t *algo = (growing_tree_algorithm_t*)base;

  seed_t *seeds;
  int seed_count;
  int row = rand() % grid->height;
  int col = rand() % grid->width;

  seeds = (seed_t*)malloc(grid->width * grid->height * sizeof(seed_t));
  seed_count = 0;

  seeds[seed_count++] = TO_SEED(row, col);

  while(seed_count > 0) {
    int index = algo->selector(algo, seeds, seed_count);
    FROM_SEED(seeds[index], row, col);

    char neighbors[4];
    int n_count = 0;

    if (row > 0 && AT(grid, row-1, col) == 0) neighbors[n_count++] = N;
    if (col > 0 && AT(grid, row, col-1) == 0) neighbors[n_count++] = W;
    if (row+1 < grid->height && AT(grid, row+1, col) == 0) neighbors[n_count++] = S;
    if (col+1 < grid->width && AT(grid, row, col+1) == 0) neighbors[n_count++] = E;

    if (n_count == 0) {
      seeds[index] = seeds[seed_count-1];
      seed_count--;
    } else {
      char dir = neighbors[rand() % n_count];
      int nrow = row + DY(dir);
      int ncol = col + DX(dir);

      AT(grid, row, col) |= dir;
      AT(grid, nrow, ncol) |= OPP(dir);

      seeds[seed_count++] = TO_SEED(nrow, ncol);
    }
  }

  free(seeds);
}
