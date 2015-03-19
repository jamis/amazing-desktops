#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "distances.h"

distances_t *dijkstra(grid_t *grid, seed_t *starts, int count)
{
  seed_t *frontier;
  seed_t *new_frontier;
  int     frontier_size, new_frontier_size;
  int     i, row, col;

  distances_t *distances = (distances_t*)malloc(sizeof(distances_t));
  distances->width = grid->width;
  distances->height = grid->height;

  distances->matrix = (int**)malloc(sizeof(int*) * distances->height);
  for(i = 0; i < distances->height; i++) {
    distances->matrix[i] = (int*)malloc(sizeof(int) * distances->width);
    memset(distances->matrix[i], -1, sizeof(int) * distances->width);
  }

  frontier = (seed_t*)malloc(grid->width * grid->height * sizeof(seed_t));
  new_frontier = (seed_t*)malloc(grid->width * grid->height * sizeof(seed_t));

  frontier_size = count;
  memcpy(frontier, starts, count*sizeof(seed_t));

  for (i = 0; i < frontier_size; i++) {
    FROM_SEED(frontier[i], row, col);
    distances->matrix[row][col] = 0;
  }

  distances->max = 0;

  while (frontier_size > 0) {
    new_frontier_size = 0;

    for(i = 0; i < frontier_size; i++) {
      int distance;

      FROM_SEED(frontier[i], row, col);
      distance = distances->matrix[row][col];

      if (distance > distances->max) {
        distances->max = distance;
        distances->max_cell = TO_SEED(row, col);
      }

      distance++;
      cell_t cell = AT(grid, row, col);

      if (HAS(cell, N) && distances->matrix[row-1][col] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row-1, col);
        distances->matrix[row-1][col] = distance;
      }

      if (HAS(cell, S) && distances->matrix[row+1][col] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row+1, col);
        distances->matrix[row+1][col] = distance;
      }

      if (HAS(cell, E) && distances->matrix[row][col+1] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row, col+1);
        distances->matrix[row][col+1] = distance;
      }

      if (HAS(cell, W) && distances->matrix[row][col-1] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row, col-1);
        distances->matrix[row][col-1] = distance;
      }
    }

    seed_t *tmp_frontier;

    tmp_frontier = frontier;
    frontier = new_frontier;
    new_frontier = tmp_frontier;

    frontier_size = new_frontier_size;
  }

  return distances;
}

void distances_free(distances_t **distances)
{
  for(int i = 0; i < (*distances)->height; i++)
    free((*distances)->matrix[i]);
  free((*distances)->matrix);
  (*distances)->matrix = NULL;
  (*distances)->width = 0;
  (*distances)->height = 0;
  free(*distances);
  *distances = NULL;
}

path_t *path_find(grid_t *grid, distances_t *distances, seed_t goal)
{
  int row, col;

  path_t *path = (path_t*)malloc(sizeof(path_t));

  FROM_SEED(goal, row, col);
  path->length = distances->matrix[row][col] + 1;
  path->steps = (seed_t*)malloc(path->length * sizeof(seed_t));

  int position = path->length-1;
  path->steps[position] = goal;

  while (position > 0) {
    position--;

    int dist = distances->matrix[row][col];
    cell_t cell = AT(grid, row, col);

    if (HAS(cell, N) && distances->matrix[row-1][col] == dist-1) {
      path->steps[position] = TO_SEED(--row, col);
    } else if (HAS(cell, S) && distances->matrix[row+1][col] == dist-1) {
      path->steps[position] = TO_SEED(++row, col);
    } else if (HAS(cell, W) && distances->matrix[row][col-1] == dist-1) {
      path->steps[position] = TO_SEED(row, --col);
    } else if (HAS(cell, E) && distances->matrix[row][col+1] == dist-1) {
      path->steps[position] = TO_SEED(row, ++col);
    }
  }

  return path;
}

void path_free(path_t **path)
{
  free((*path)->steps);
  (*path)->steps = NULL;
  (*path)->length = 0;
  free(*path);
  *path = NULL;
}
