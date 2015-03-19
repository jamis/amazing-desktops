#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "grid.h"
#include "algorithm.h"
#include "gradient.h"
#include "image.h"
#include "distances.h"

#include "growing_tree.h"
#include "binary_tree.h"
#include "aldous_broder.h"


void color_distances(image_t *img, distances_t *distances, gradient_t *gradient)
{
  rgb_t *colors;
  int max = distances->max;

  colors = (rgb_t*)malloc((max+1) * sizeof(int));
  for(int color = 0; color < max+1; color++) {
    colors[color] = gradient_at(color, max, gradient);
  }

  for (int row = 0; row < distances->height; row++) {
    for (int col = 0; col < distances->width; col++) {
      int distance = distances->matrix[row][col];
      SETPX(img, col, row, colors[distance]);
    }
  }

  free(colors);
}

void color_path(image_t *img, path_t *path, rgb_t color) {
  for(int i = 0; i < path->length; i++) {
    int row, column;
    FROM_SEED(path->steps[i], row, column);
    SETPX(img, column, row, color);
  }
}


int main(int argc, char *argv[]) {
  grid_t *grid = NULL;
  int     width = 640, height = 480;
  int     gradient_size = 5;
  long    rseed = time(NULL);
  gradient_t gradient;
  rgb_t path_color = 0x0;
  algorithm_t *algo = growing_tree_mostly_longest;
  distances_t *distances = NULL;
  path_t *path = NULL;

  gradient.size = 0;

  for (int i = 1; i < argc; i++) {
    switch(argv[i][0]) {
      case 'w': width = atoi(&argv[i][1]); break;
      case 'h': height = atoi(&argv[i][1]); break;
      case 's': rseed = atol(&argv[i][1]); break;
      case 'g':
        switch(argv[i][1]) {
          case 'a':
            memcpy(&gradient, &gr_autumn, sizeof(gr_autumn));
            break;
          default:
            gradient_size = atoi(&argv[i][1]);
        }
        break;

      case 'c':
        gradient.colors[gradient.size++] = strtol(&argv[i][1], NULL, 16);
        break;

      case 'p':
        path_color = strtol(&argv[i][1], NULL, 16);
        break;

      case 'a':
        switch(argv[i][1]) {
          case 'b': algo = binary_tree; break;
          case 'g':
            switch(argv[i][2]) {
	      case 'a': algo = aldous_broder; break;
              case 'l': algo = growing_tree_mostly_longest; break;
              case 'w': algo = growing_tree_weighted; break;
              default:
                printf("ignoring unrecognized growing tree variant: `%s'\n", argv[i]);
            }
            break;
          default:
            printf("ignoring unrecognized algorithm: `%s'\n", argv[i]);
        }
        break;

      default:
        printf("ignoring unknown argument `%s'\n", argv[i]);
    }
  }

  printf("seed: %ld\n", rseed);
  srand(rseed);

  if (gradient.size == 0) {
    if (gradient_size < 1 || gradient_size > MAX_GRADIENT_SIZE)
      gradient_size = 5;
    gradient_random(&gradient, gradient_size);

    printf("colors:");
    for(int i = 0; i < gradient.size; i++)
      printf(" c%08x", gradient.colors[i]);
    printf("\n");
  }

  if (path_color == 0) {
    path_color = gradient.colors[rand() % gradient.size];
    printf("path color: p%08x\n", path_color);
  }

  if(width < 1) width = 1;
  if(height < 1) height = 1;

  grid = grid_create(width, height);

  printf("- generating maze\n");
  algorithm_run(algo, grid);

  printf("- running Dijkstra's algorithm\n");

  seed_t start = TO_SEED(rand() % grid->height, rand() % grid->width);
  distances = dijkstra(grid, &start, 1);

  start = distances->max_cell;
  distances_free(&distances);

  printf("- finding distances from most distant cell\n");
  distances = dijkstra(grid, &start, 1);

  printf("- finding longest path\n");
  path = path_find(grid, distances, distances->max_cell);
  distances_free(&distances);

  printf("- finding distances from path\n");
  distances = dijkstra(grid, path->steps, path->length);

  image_t *img = image_create(width, height);
  color_distances(img, distances, &gradient);
  color_path(img, path, path_color);
  image_save(img, "maze.png");
  image_free(img);

  path_free(&path);
  distances_free(&distances);

  grid_free(&grid);

  return 0;
}
