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

int randomi(int limit) {
  return rand() % limit;
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

  print_map(&map, &screen);
  print_console(&screen);

  int changed;
  while(1) {
    if(window_handle != GetForegroundWindow()) continue;

    changed = 0;

    if(GetKeyState(VK_SPACE) & 0x8000) {
      break;
    }

    if(GetKeyState(0x57) & 0x8000) {
      changed |= print_string(&screen, "W", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x41) & 0x8000) {
      changed |= print_string(&screen, "A", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x53) & 0x8000) {
      changed |= print_string(&screen, "S", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }
    else if(GetKeyState(0x44) & 0x8000) {
      changed |= print_string(&screen, "D", FG_WHITE | BG_BLACK, 0, 0, ALIGN_LEFT);
    }

    if(changed) {
      print_console(&screen);
    }

    Sleep(10);
  }

  free_map(&map);
  free_screen(&screen);
  
  waveOutClose(wave_device);

  return 0;
};
