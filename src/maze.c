#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_GRADIENT_SIZE (100)
#define MAX_WEIGHT (100)
#define MAX_SEARCH_DISTANCE (100)

typedef uint32_t rgb_t;
typedef struct {
  rgb_t   colors[MAX_GRADIENT_SIZE];
  uint8_t size;
} gradient_t;

gradient_t gr_autumn = {
  .colors = { 0x9a95deff, 0x585680ff, 0x568071ff, 0x7cb9a4ff,
              0x97b97cff, 0x586c49ff, 0x6c6249ff, 0xccb88aff,
              0xcc8f8aff, 0x5c403eff },
  .size = 10
};

typedef uint8_t  cell_t;
typedef uint32_t seed_t;

typedef union {
  uint32_t rgba;
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  } __attribute__ ((__packed__));
} pixel_t;

typedef struct {
  int width;
  int height;
  pixel_t *pixels;
} image_t;

typedef struct {
  void (*prepare)(cell_t**, int, int);
  void (*cleanup)(void);
  int (*selector)(seed_t*, int);
} algorithm_t;

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

#define TO_SEED(row, col) ( ((row) << 16) + (col) )
#define FROM_SEED(seed, row, col) \
  { row = ((seed) >> 16) & 0xFFFF; col = (seed) & 0xFFFF; }

image_t *create_image(width, height) {
  image_t *img = (image_t*)malloc(sizeof(image_t));
  img->width = width;
  img->height = height;
  img->pixels = (pixel_t*)malloc(sizeof(pixel_t) * width * height);
  memset(img->pixels, 0, sizeof(pixel_t) * width * height);
  return img;
}

void free_image(image_t* img) {
  free(img->pixels);
  img->width = img->height = 0;
  img->pixels = NULL;
  free(img);
}

#define GETPX(img, x, y) \
  (&((img)->pixels[(y)*(img)->width + (x)]))

#define SETPX(img, x, y, color) GETPX((img), (x), (y))->rgba = (color)

void draw_text(cell_t **cells, int width, int height) {
  int row, col;

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++) {
      putchar('+');
      if (HAS(cells[row][col], E)) {
        putchar('-');
        putchar('-');
      } else {
        putchar(' ');
        putchar(' ');
      }
    }
    putchar('\n');
    for (col = 0; col < width; col++) {
      if (HAS(cells[row][col], S))
        putchar('|');
      else
        putchar(' ');
      putchar(' ');
      putchar(' ');
    }
    putchar('\n');
  }
}

void dijkstra(cell_t **cells, int **distances,
  seed_t *starts, int count, int width, int height,
  int *max_distance, int *max_row, int *max_col)
{
  seed_t *frontier;
  seed_t *new_frontier;
  int     frontier_size, new_frontier_size;
  int     i, row, col;

  for(i = 0; i < height; i++)
    memset(distances[i], -1, width*sizeof(**distances));

  frontier = (seed_t*)malloc(width * height * sizeof(seed_t));
  new_frontier = (seed_t*)malloc(width * height * sizeof(seed_t));

  frontier_size = count;
  memcpy(frontier, starts, count*sizeof(seed_t));

  for (i = 0; i < frontier_size; i++) {
    FROM_SEED(frontier[i], row, col);
    distances[row][col] = 0;
  }

  *max_distance = 0;

  while (frontier_size > 0) {
    new_frontier_size = 0;

    for(i = 0; i < frontier_size; i++) {
      int distance;

      FROM_SEED(frontier[i], row, col);
      distance = distances[row][col];

      if (distance > *max_distance) {
        *max_distance = distance;
        *max_row = row;
        *max_col = col;
      }

      distance++;

      if (HAS(cells[row][col], N) && distances[row-1][col] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row-1, col);
        distances[row-1][col] = distance;
      }

      if (HAS(cells[row][col], S) && distances[row+1][col] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row+1, col);
        distances[row+1][col] = distance;
      }

      if (HAS(cells[row][col], E) && distances[row][col+1] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row, col+1);
        distances[row][col+1] = distance;
      }

      if (HAS(cells[row][col], W) && distances[row][col-1] < 0) {
        new_frontier[new_frontier_size++] = TO_SEED(row, col-1);
        distances[row][col-1] = distance;
      }
    }

    seed_t *tmp_frontier;

    tmp_frontier = frontier;
    frontier = new_frontier;
    new_frontier = tmp_frontier;

    frontier_size = new_frontier_size;
  }
}

void find_path(cell_t **cells, int **distances,
  seed_t **path, int *length,
  int row, int col)
{
  *length = distances[row][col] + 1;
  *path = (seed_t*)malloc(*length * sizeof(seed_t));

  int position = *length-1;
  (*path)[position] = TO_SEED(row, col);

  while (position > 0) {
    position--;

    int dist = distances[row][col];
    if (HAS(cells[row][col], N) && distances[row-1][col] == dist-1) {
      (*path)[position] = TO_SEED(--row, col);
    } else if (HAS(cells[row][col], S) && distances[row+1][col] == dist-1) {
      (*path)[position] = TO_SEED(++row, col);
    } else if (HAS(cells[row][col], W) && distances[row][col-1] == dist-1) {
      (*path)[position] = TO_SEED(row, --col);
    } else if (HAS(cells[row][col], E) && distances[row][col+1] == dist-1) {
      (*path)[position] = TO_SEED(row, ++col);
    } else {
      printf("uh-oh, shouldn't have reached here...\n");
    }
  }
}

rgb_t gradient_at(int n, int max, gradient_t *gradient)
{
  int product = n * (gradient->size - 1);
  int element = product / max;
  int t = product % max;

  int r = (gradient->colors[element] >> 24) & 0xFF;
  int g = (gradient->colors[element] >> 16) & 0xFF;
  int b = (gradient->colors[element] >>  8) & 0xFF;
  int a = (gradient->colors[element] >>  0) & 0xFF;

  if (t > 0) {
    int ur = (gradient->colors[element+1] >> 24) & 0xFF;
    int ug = (gradient->colors[element+1] >> 16) & 0xFF;
    int ub = (gradient->colors[element+1] >>  8) & 0xFF;
    int ua = (gradient->colors[element+1] >>  0) & 0xFF;

    r += (ur - r) * t / max;
    g += (ug - g) * t / max;
    b += (ub - b) * t / max;
    a += (ua - a) * t / max;
  }

  return ((r << 24) + (g << 16) + (b << 8) + a);
}

void color_distances(image_t *img,
  cell_t **cells, int rows, int cols,
  int **distances, int max,
  gradient_t *gradient)
{
  rgb_t *colors;

  colors = (rgb_t*)malloc((max+1) * sizeof(int));
  for(int color = 0; color < max+1; color++) {
    colors[color] = gradient_at(color, max, gradient);
  }

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int distance = distances[row][col];
      SETPX(img, col, row, colors[distance]);
    }
  }

  free(colors);
}

void color_path(image_t *img, seed_t *path, int length, rgb_t color) {
  for(int i = 0; i < length; i++) {
    int row, column;
    FROM_SEED(path[i], row, column);
    SETPX(img, column, row, color);
  }
}

void save(image_t *img, char *name) {
  FILE *fp;
  png_structp png = NULL;
  png_infop info = NULL;
  size_t x, y;
  png_byte **rows = NULL;

  fp = fopen(name, "wb");
  if (!fp) {
    printf("could not open file `%s'\n", name);
  } else {
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) {
      printf("could not create PNG struct\n");
    } else {
      info = png_create_info_struct(png);
      if (info == NULL) {
        printf("could not create info struct\n");
      } else {
        if (setjmp(png_jmpbuf(png))) {
          printf("error creating png\n");
        } else {
          png_set_IHDR(png, info,
                       img->width, img->height, 8,
                       PNG_COLOR_TYPE_RGB,
                       PNG_INTERLACE_NONE,
                       PNG_COMPRESSION_TYPE_DEFAULT,
                       PNG_FILTER_TYPE_DEFAULT);

          rows = png_malloc(png, img->height * sizeof(png_byte*));
          for (y = 0; y < img->height; y++) {
            png_byte *row = png_malloc(png, sizeof(uint8_t) * img->width * 3);
            rows[y] = row;
            for (x = 0; x < img->width; x++) {
              pixel_t *pixel = GETPX(img, x, y);
              *row++ = pixel->r;
              *row++ = pixel->g;
              *row++ = pixel->b;
            }
          }

          png_init_io(png, fp);
          png_set_rows(png, info, rows);
          png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

          for (y = 0; y < img->height; y++)
            png_free(png, rows[y]);
          png_free(png, rows);
        }
      }
      png_destroy_write_struct(&png, &info);
    }

    fclose(fp);
  }
}

static int s_jump_factor = 5;

void growing_tree_mostly_longest_prepare(cell_t **cells, int width, int height) {
  s_jump_factor = sqrt(width > height ? height : width);
  if (s_jump_factor < 5) s_jump_factor = 5;
}

void growing_tree_mostly_longest_cleanup() {
}

int growing_tree_mostly_longest_selector(seed_t *seeds, int count) {
  int index = count-1;
  if (rand() % s_jump_factor == 0) index = rand() % count;

  return index;
}

static uint8_t **s_weights = NULL;

void growing_tree_weighted_prepare(cell_t **cells, int width, int height) {
  s_weights = (uint8_t**)malloc(sizeof(uint8_t*) * height);
  for(int i = 0; i < height; i++) {
    s_weights[i] = (uint8_t*)malloc(sizeof(uint8_t) * width);
    for(int j = 0; j < width; j++) {
      s_weights[i][j] = rand() % (MAX_WEIGHT+1);
    }
  }
}

void growing_tree_weighted_cleanup(void) {
  free(s_weights);
  s_weights = NULL;
}

int growing_tree_weighted_selector(seed_t *seeds, int count) {
  int i = count;
  int largest_weight = 0;
  int best_index;

  if (i > MAX_SEARCH_DISTANCE) i = MAX_SEARCH_DISTANCE;

  while (i > 0) {
    int row, col;
    FROM_SEED(seeds[count - i], row, col);
    if (s_weights[row][col] > largest_weight) {
      largest_weight = s_weights[row][col];
      best_index = count - i;
      if (largest_weight == MAX_WEIGHT) break;
    }
    i--;
  }

  return best_index;
}

static algorithm_t growing_tree_mostly_longest = {
  .prepare = growing_tree_mostly_longest_prepare,
  .cleanup = growing_tree_mostly_longest_cleanup,
  .selector = growing_tree_mostly_longest_selector
};

static algorithm_t growing_tree_weighted = {
  .prepare = growing_tree_weighted_prepare,
  .cleanup = growing_tree_weighted_cleanup,
  .selector = growing_tree_weighted_selector
};

void growing_tree(cell_t **cells, int width, int height, int (*selector)(seed_t*, int)) {
  seed_t *seeds;
  int seed_count;
  int row = rand() % height;
  int col = rand() % width;

  seeds = (seed_t*)malloc(width * height * sizeof(seed_t));
  seed_count = 0;

  seeds[seed_count++] = TO_SEED(row, col);

  while(seed_count > 0) {
    int index = selector(seeds, seed_count);
    FROM_SEED(seeds[index], row, col);

    char neighbors[4];
    int n_count = 0;

    if (row > 0 && cells[row-1][col] == 0) neighbors[n_count++] = N;
    if (col > 0 && cells[row][col-1] == 0) neighbors[n_count++] = W;
    if (row+1 < height && cells[row+1][col] == 0) neighbors[n_count++] = S;
    if (col+1 < width && cells[row][col+1] == 0) neighbors[n_count++] = E;

    if (n_count == 0) {
      seeds[index] = seeds[seed_count-1];
      seed_count--;
    } else {
      char dir = neighbors[rand() % n_count];
      int nrow = row + DY(dir);
      int ncol = col + DX(dir);

      cells[row][col] |= dir;
      cells[nrow][ncol] |= OPP(dir);

      seeds[seed_count++] = TO_SEED(nrow, ncol);
    }
  }

  free(seeds);
}

void hsv2rgb(
  uint8_t *r, uint8_t *g, uint8_t *b,
  float h, float s, float v)
{
  int i;
  float f, p, q, t;

  if (s == 0) {
    *r = *g = *b = roundf(v * 255);
    return;
  }

  h /= 60;
  i = floor(h);
  f = h - i;
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));

  uint8_t vi = roundf(v * 255);
  uint8_t pi = roundf(p * 255);
  uint8_t qi = roundf(q * 255);
  uint8_t ti = roundf(t * 255);

  switch(i) {
    case 0:
      *r = vi;
      *g = ti;
      *b = pi;
      break;
    case 1:
      *r = qi;
      *g = vi;
      *b = pi;
      break;
    case 2:
      *r = pi;
      *g = vi;
      *b = ti;
      break;
    case 3:
      *r = pi;
      *g = qi;
      *b = vi;
      break;
    case 4:
      *r = ti;
      *g = pi;
      *b = vi;
      break;
    default:    // case 5:
      *r = vi;
      *g = pi;
      *b = qi;
      break;
  }
}

void random_gradient(gradient_t *gradient, int size)
{
  float h, s, v;

  gradient->size = size;

  h = rand() % 3600 / 10.0;
  s = 1.0 * rand() / RAND_MAX;
  v = 0.2 + 0.8 * rand() / RAND_MAX;

  for(int i = 0; i < size; i++) {
    uint8_t r, g, b;

    hsv2rgb(&r, &g, &b, h, s, v);
    gradient->colors[i] = ((r << 24) + (g << 16) + (b << 8) + 0xff);

    h += ((rand() % 1200) / 10.0) - 60.0;
    s += ((rand() % 40) / 100.0) - 0.2;
    v += ((rand() % 40) / 100.0) - 0.2;

    if(h < 0) h += 360;
    if(h > 360) h -= 360;
    if(s < 0) s = 0;
    if(s > 1) s = 1;
    if(v < 0) v = 0;
    if(v > 1) v = 1;
  }
}

int main(int argc, char *argv[]) {
  cell_t **cells;
  int     width = 640, height = 480;
  int     gradient_size = 5;
  long    rseed = time(NULL);
  gradient_t gradient;
  rgb_t path_color = 0x0;
  //algorithm_t *algo = &growing_tree_weighted;
  algorithm_t *algo = &growing_tree_mostly_longest;

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

      default:
        printf("ignoring unknown argument `%s'\n", argv[i]);
    }
  }

  printf("seed: %ld\n", rseed);
  srand(rseed);

  if (gradient.size == 0) {
    if (gradient_size < 1 || gradient_size > MAX_GRADIENT_SIZE)
      gradient_size = 5;
    random_gradient(&gradient, gradient_size);

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

  cells = (cell_t**)malloc(height*sizeof(cell_t*));
  for(int i = 0; i < height; i++) {
    cells[i] = (cell_t*)malloc(width*sizeof(cell_t));
    memset(cells[i], 0, width);
  }
  printf("- running Growing Tree algorithm\n");
  algo->prepare(cells, width, height);
  growing_tree(cells, width, height, algo->selector);
  algo->cleanup();

  printf("- running Dijkstra's algorithm\n");

  int **distances = (int**)malloc(height * sizeof(*distances));
  int max_distance, max_row, max_col;

  for (int i = 0; i < height; i++)
    distances[i] = (int*)malloc(width * sizeof(**distances));

  seed_t start = TO_SEED(rand() % height, rand() % width);
  dijkstra(cells, distances, &start, 1, width, height,
    &max_distance, &max_row, &max_col);

  printf("  greatest distance from 0,0: %d\n", max_distance);
  printf("  most distant cell: %d,%d\n", max_row, max_col);

  printf("- finding distances from most distant cell\n");
  start = TO_SEED(max_row, max_col);
  dijkstra(cells, distances, &start, 1, width, height,
    &max_distance, &max_row, &max_col);

  printf("- finding longest path\n");

  seed_t *path;
  int length;
  find_path(cells, distances, &path, &length, max_row, max_col);

  printf("- finding distances from path\n");
  dijkstra(cells, distances,
    path, length, width, height,
    &max_distance, &max_row, &max_col);

  printf("  greatest distance from path: %d\n", max_distance);
  printf("  most distant cell: %d,%d\n", max_row, max_col);

  image_t *img = create_image(width, height);
  color_distances(img, cells, height, width, distances, max_distance, &gradient);
  color_path(img, path, length, path_color);
  save(img, "maze.png");
  free_image(img);

  free(path);

  for (int i = 0; i < height; i++)
    free(distances[i]);
  free(distances);

  for(int i = 0; i < height; i++) free(cells[i]);
  free(cells);

  return 0;
}
