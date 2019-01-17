#ifndef _PERLIN_NOISE_H_
#define _PERLIN_NOISE_H_

// Source code from Rosetta Code:
// https://rosettacode.org/wiki/Perlin_noise#C

extern double fade(double t);
extern double lerp(double t, double a, double b);
extern double grad(int hash, double x, double y, double z);
extern double noise(double x, double y, double z);

extern void load_permutation(const char* filename);

#endif
