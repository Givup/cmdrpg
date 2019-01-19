#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "perlin.h"
#include "screen.h"
#include "map.h"

#define SW 120
#define SH 40

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

int IsFormatSupported(WAVEFORMATEX format, UINT device) {
  return waveOutOpen(NULL, device, &format, (DWORD_PTR)NULL, 0, WAVE_FORMAT_QUERY ) == MMSYSERR_NOERROR ? 1 : 0;
};

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
  if(uMsg == WOM_OPEN) {
    printf("Audio device opened.\n");
  } else if(uMsg == WOM_CLOSE) {
    printf("Audio device closed.\n");
  } else if(uMsg == WOM_DONE) {
    printf("Done with the audio device.\n");
  }
};

char* generate_sawtooth(int t0, int num_steps, int mod) {
  char* data = (char*)malloc(sizeof(char) * num_steps);

  for(int i = 0;i < num_steps;i++) {
    data[i] = (t0 + i) % mod;
  }

  return data;
};

int main(int argc, char** argv) {

  WORD channels = 1;
  DWORD samples = 44100;
  WORD bits_per_sample = 8;
  
  WAVEFORMATEX wave_format = {
			      WAVE_FORMAT_PCM, // wFormatTag
			      channels, // nChannels
			      samples, // nSamplesPerSecond
			      samples * ((channels * bits_per_sample) / 8), // nAvgBytesPerSec = nSamplesPerSecond * nBlockAlign
			      (channels * bits_per_sample) / 8, // nBlockAlign = nChannels * wBitsPerSample / 8
			      bits_per_sample, // wBitsPerSample,
			      0 // cbSize, not used with PCM data
  };

  char* sawtooth = generate_sawtooth(0, 100, 15);

  // Possibly upload data to audio device

  free(sawtooth);
  
  int selected_device = -1;

  UINT num_devs = waveOutGetNumDevs();
  for(int i = num_devs - 1; i >= 0; i--) {
    WAVEOUTCAPS caps;
    waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS));
    int support = IsFormatSupported(wave_format, i);
    printf("\tCan use: [%s] - Name: %s\n", support ? "x" : " ", caps.szPname);

    if(selected_device == -1 || (i < selected_device && support)) {
      selected_device = i;
    }
  }

  printf("Selected device %d.\n", selected_device);

  HWAVEOUT wave_device;
  if(waveOutOpen(&wave_device, selected_device, &wave_format, (DWORD_PTR)waveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
    printf("Failed to open audio output device.\n");
    return 1;
  }

  load_permutation("perlin_seed");

  HWND window_handle = GetForegroundWindow();
  
  system("title DNG_CMD"); // Change title

  Screen screen;
  if(create_screen(&screen, SW, SH)) {
    printf("Failed to create screen.\n");
    return 1;
  }

  show_cursor(&screen, FALSE);

  Map map;
  create_map(&map, SW, SH);
  generate_map(&map, 0, 0, SW, SH);

  //print_map(&map, &screen);
  //print_console(&screen);

  int can_move_x = 0;
  int can_move_y = 0;

  int map_x, map_y;
  int px = -100, py = 0;

  int changed = 1;

  const int MODE_WORLD = 0;
  const int MODE_BIOME = 1;

  int mode = MODE_WORLD;
  int space_last = 0;

  while(1) {
    if(window_handle != GetForegroundWindow()) continue;

    can_move_x -= can_move_x > 0 ? 1 : 0;
    can_move_y -= can_move_y > 0 ? 1 : 0;

    if(GetKeyState(VK_ESCAPE) & 0x8000) {
      break;
    }

    if(GetKeyState(VK_SPACE) & 0x8000 && space_last == 0) {
      if(mode == MODE_WORLD) {
	if(generate_biome_at(&map, SW / 2, SH / 2)) {
	  map_x = px;
	  map_y = py;
	  px = SW / 2;
	  py = SH / 2;
	  mode = MODE_BIOME;
	}
      } else {
	px = map_x;
	py = map_y;
	mode = MODE_WORLD;
	clear_entities(&map);
      }
      changed = 1;
    }
    space_last = GetKeyState(VK_SPACE) & 0x8000;

    if(GetKeyState(0x57) & 0x8000 && can_move_y <= 0) {
      //changed |= print_string(&screen, "W", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
      py--;
      changed = 1;
    }
    if(GetKeyState(0x41) & 0x8000 && can_move_x <= 0) {
      //changed |= print_string(&screen, "A", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
      px--;
      changed = 1;
    }
    if(GetKeyState(0x53) & 0x8000 && can_move_y <= 0) {
      //changed |= print_string(&screen, "S", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
      py++;
      changed = 1;
    }
    if(GetKeyState(0x44) & 0x8000 && can_move_x <= 0) {
      //changed |= print_string(&screen, "D", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
      px++;
      changed = 1;
    }

    if(mode == MODE_BIOME) {
      px = (px + SW) % SW;
      py = (py + SH) % SH;
    }

    if(changed) {
      changed = 0;
      can_move_x = 9;
      can_move_y = 12;

      if(mode == MODE_WORLD) {
	generate_map(&map, px - SW / 2, py - SH / 2, SW, SH);
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, SW / 2, SH / 2), SW / 2, SH / 2, ALIGN_LEFT);
      }
      else {
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, px, py), px, py, ALIGN_LEFT);
      }


      print_console(&screen);

      char buf[32];
      sprintf(buf, "[%d,%d]       ", px, py);
      print_string(&screen, buf, FG_BLACK | BG_WHITE, 0, 0, ALIGN_LEFT);
    }

    Sleep(10);
  }

  free_map(&map);
  free_screen(&screen);
  
  waveOutClose(wave_device);

  return 0;
};
