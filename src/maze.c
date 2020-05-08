#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "grid.h"
#include "algorithm.h"
#include "gradient.h"
#include "image.h"
#include "distances.h"

#include "growing_tree.h"
#include "binary_tree.h"
#include "aldous_broder.h"
#include "recursive_subdivision.h"


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

void color_distances_with_smoothing(image_t *img, distances_t *distances, gradient_t *gradient, int radius)
{
  rgb_t *colors;
  int max = distances->max;
  float *opacities = (float*)malloc((radius * 2) * sizeof(float));

  colors = (rgb_t*)malloc((max+1) * sizeof(int));
  for(int color = 0; color < max+1; color++) {
    colors[color] = gradient_at(color, max, gradient);
  }

  float step = 1.0 / (radius * 2);
  for(int r = 0; r < radius * 2; r++) {
    opacities[r] = pow(1.0 - (r * step), 2);
  }

  for (int distance = max; distance >= 0; distance--) {
    rgb_t c = colors[distance];
    uint8_t r = (c >> 24) & 0xFF;
    uint8_t g = (c >> 16) & 0xFF;
    uint8_t b = (c >> 8) & 0xFF;

    for (int row = 0; row < distances->height; row++) {
      for (int col = 0; col < distances->width; col++) {
        if (distances->matrix[row][col] == distance) {
          for (int dy = -radius; dy <= radius; dy++) {
            int y = row + dy;
            if (y < 0 || y >= distances->height) continue;
            for (int dx = -radius; dx <= radius; dx++) {
              int x = col + dx;
              if (x < 0 || x >= distances->width) continue;
              pixel_t *p = GETPX(img, x, y);
              float opacity = opacities[abs(dx) + abs(dy)];

              p->r = (uint8_t)(r * opacity + p->r * (1 - opacity));
              p->g = (uint8_t)(g * opacity + p->g * (1 - opacity));
              p->b = (uint8_t)(b * opacity + p->b * (1 - opacity));
              p->a = 0xFF;
            }
          }
        }
      }
    }
  }

  free(colors);
  free(opacities);
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
  int width = 640, height = 480;
  int gradient_size = 5;
  gradient_t gradient;
  rgb_t path_color = 0x0;
  algorithm_t *algo = NULL;
  distances_t *distances = NULL;
  path_t *path = NULL;
  int show_path = 1;
  int blur_radius = 0;
  int blur_given = 0;
  int quiet = 0;
  char output[255] = "maze.png";
  unsigned long rseed = 0;

  gradient.size = 0;

  for (int i = 1; i < argc; i++) {
    switch(argv[i][0]) {
      case 'w': width = atoi(&argv[i][1]); break;
      case 'h': height = atoi(&argv[i][1]); break;
      case 's': rseed = atol(&argv[i][1]); break;
      case 'o': strcpy(output, &argv[i][1]); break;
      case 'q': quiet = 1; break;
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
        switch(argv[i][1]) {
          case '-': show_path = 0; break;
          default: path_color = strtol(&argv[i][1], NULL, 16);
        }
        break;

      case 'b':
        blur_radius = atoi(&argv[i][1]);
        blur_given = 1;
        break;

      case 'a':
        switch(argv[i][1]) {
          case '*': algo = NULL; break;
          case 'a': algo = aldous_broder; break;
          case 'b': algo = binary_tree; break;
          case 'r':
            switch(argv[i][2]) {
              case 's': algo = recursive_subdivision; break;
              default:
                printf("ignoring unrecognized recursive algorithm: `%s'\n", argv[i]);
            }
            break;

          case 'g':
            switch(argv[i][2]) {
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

  if (rseed == 0) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    rseed = (tv.tv_sec % 4000000) * 1000 + tv.tv_usec / 1000;
  }

  if(!quiet) printf("seed: %ld\n", rseed);
  srand(rseed);

  if (algo == NULL) {
    if(!quiet) printf("algorithm: ");
    switch(rand() % 5) {
      case 0:
        if(!quiet) printf("aa");
        algo = aldous_broder;
        break;
      case 1:
        if(!quiet) printf("ab");
        algo = binary_tree;
        break;
      case 2:
        if(!quiet) printf("ars");
        algo = recursive_subdivision;
        break;
      case 3:
        if(!quiet) printf("agl");
        algo = growing_tree_mostly_longest;
        break;
      case 4:
        if(!quiet) printf("agw");
        algo = growing_tree_weighted;
        break;
    }
    if(!quiet) printf("\n");
  }

  if (gradient.size == 0) {
    if (gradient_size < 1 || gradient_size > MAX_GRADIENT_SIZE)
      gradient_size = 5;
    gradient_random(&gradient, gradient_size);

    if (!quiet) {
      printf("colors:");
      for(int i = 0; i < gradient.size; i++)
        printf(" c%08x", gradient.colors[i]);
      printf("\n");
    }
  }

  if (!blur_given) {
    blur_radius = rand() % 4;
    if (!quiet) printf("smoothing: b%d\n", blur_radius);
  }

  if (show_path && path_color == 0) {
    path_color = gradient.colors[rand() % gradient.size];
    if(!quiet) printf("path color: p%08x\n", path_color);
  }

  if(width < 1) width = 1;
  if(height < 1) height = 1;

  grid = grid_create(width, height);

  if (!quiet) printf("- generating maze\n");
  algorithm_run(algo, grid);

  if (!quiet) printf("- running Dijkstra's algorithm\n");

  seed_t start = TO_SEED(rand() % grid->height, rand() % grid->width);
  distances = dijkstra(grid, &start, 1);

  start = distances->max_cell;
  distances_free(&distances);

  if (!quiet) printf("- finding distances from most distant cell\n");
  distances = dijkstra(grid, &start, 1);

  if (!quiet) printf("- finding longest path\n");
  path = path_find(grid, distances, distances->max_cell);
  distances_free(&distances);

  if (!quiet) printf("- finding distances from path\n");
  distances = dijkstra(grid, path->steps, path->length);

  if (!quiet) printf("- drawing image\n");
  image_t *img = image_create(width, height);
  if (blur_radius > 0) {
    color_distances_with_smoothing(img, distances, &gradient, blur_radius);
  } else {
    color_distances(img, distances, &gradient);
  }
  if (show_path) color_path(img, path, path_color);

  if (!quiet) printf("- writing to %s\n", output);
  image_save(img, output);
  image_free(img);

  path_free(&path);
  distances_free(&distances);

  grid_free(&grid);

  return 0;
}
