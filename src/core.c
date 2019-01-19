#include "core.h"
#include <stdlib.h>

float randomf() {
  return (float)rand() / (float)RAND_MAX;
};

float randombif() {
  return (randomf() - 0.5f) * 2.0f;
};

unsigned int randomi(unsigned int limit) {
  return rand() % limit;
};

unsigned int randomi_range(unsigned int min, unsigned int max) {
  if(max <= min) return 0;
  return randomi(max - min) + min;
};
