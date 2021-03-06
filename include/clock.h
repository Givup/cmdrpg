#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <windows.h>

struct Clock {
  LARGE_INTEGER start_time, end_time;
  LARGE_INTEGER frequency;
};

void
start_clock(struct Clock* clock) {
  QueryPerformanceFrequency(&clock->frequency);
  QueryPerformanceCounter(&clock->start_time);
};

float
get_clock_delta_s(struct Clock* clock) {
  QueryPerformanceCounter(&clock->end_time); // Get current time
  LARGE_INTEGER elapsed; // Calculate elapsed time between last reset

  // Calculate elapsed time in microseconds, and in the end turn it into seconds
  elapsed.QuadPart = (clock->end_time.QuadPart - clock->start_time.QuadPart);  
  elapsed.QuadPart *= 1000000;
  elapsed.QuadPart /= clock->frequency.QuadPart;

  return (float)elapsed.QuadPart / 1000000.0f;
};

void
reset_clock(struct Clock* clock) {
  clock->start_time = clock->end_time;
};

#endif
