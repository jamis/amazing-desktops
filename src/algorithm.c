#include "algorithm.h"

void algorithm_run(algorithm_t *algo, grid_t *grid)
{
  if(algo->prepare) algo->prepare(algo, grid);
  algo->run(algo, grid);
  if(algo->cleanup) algo->cleanup(algo, grid);
}
