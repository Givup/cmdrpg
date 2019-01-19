#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "perlin.h"
#include "screen.h"
#include "map.h"
#include "status.h"

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

  char line_buffer[SW + 1];
  memset(line_buffer, ' ', SW);
  line_buffer[SW] = 0;

  show_cursor(&screen, FALSE);

  Map map;
  create_map(&map, SW, SH - 2);
  generate_map(&map, 0, 0, map.width, map.height);

  int can_move = 0;

  int map_x, map_y;
  int px = -100, py = 0;

  Status status = { 0 };
  status.max_hp = 25;
  status.hp = status.max_hp;
  status.temp = 20;

  int changed = 1;

  const int MODE_WORLD = 0;
  const int MODE_BIOME = 1;

  int mode = MODE_WORLD;
  int space_last = 0;

  int d_x = 0, d_y = 0;

  while(1) {
    if(window_handle != GetForegroundWindow()) continue;
    if(status.hp <= 0) {
      print_string(&screen, "YOU DIED", FG_WHITE | BG_BLACK, SW / 2, SH / 2, ALIGN_CENTER);
      print_string(&screen, "Press escape to exit.", FG_WHITE | BG_BLACK, SW / 2, SH / 2 + 1, ALIGN_CENTER);
      print_console(&screen);
      if(GetKeyState(VK_ESCAPE) & 0x8000) {
	break;
      }
      continue;
    };

    can_move -= can_move > 0 ? 1 : 0;

    if(GetKeyState(VK_ESCAPE) & 0x8000) {
      break;
    }

    if(GetKeyState(VK_SPACE) & 0x8000 && space_last == 0) {
      if(mode == MODE_WORLD) {
	if(generate_biome_at(&map, map.width / 2, map.height / 2)) {
	  map_x = px;
	  map_y = py;
	  px = map.width / 2;
	  py = map.height / 2;
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

    if(GetKeyState(0x57) & 0x8000 && can_move <= 0) {
      d_y = -1; // Move up
    }
    if(GetKeyState(0x41) & 0x8000 && can_move <= 0) {
      d_x = -1; // Move left
    }
    if(GetKeyState(0x53) & 0x8000 && can_move <= 0) {
      d_y = 1; // Move down
    }
    if(GetKeyState(0x44) & 0x8000 && can_move <= 0) {
      d_x = 1; // Move right
    }

    Status env_status = { 0 };
    if(mode == MODE_WORLD) {
      px += d_x;
      py += d_y;

      if(d_x != 0 || d_y != 0) changed = 1;

      env_status.temp = get_tile_temp(get_tile_at(&map, map.width / 2, map.height / 2));
    } 
    else if(mode == MODE_BIOME) {
      if(d_x != 0 || d_y != 0) {
	try_move_to(&map, (px + d_x + map.width) % map.width, py, &env_status);
	try_move_to(&map, px, (py + d_y + map.height) % map.height, &env_status);

	if(can_move_to(&map, (px + d_x + map.width) % map.width, py)) {
	  px += d_x;
	  changed = 1;
	}
	if(can_move_to(&map, px, (py + d_y + map.height) % map.height)) {
	  py += d_y;
	  changed = 1;
	}

	px = (px + map.width) % map.width;
	py = (py + map.height) % map.height;

	env_status.temp = get_tile_temp(get_tile_at(&map, px, py));
      }
    }

    d_x = d_y = 0;

    if(changed) {
      changed = 0;

      int temp_d = env_status.temp - status.temp;
      status.wet += env_status.wet;
      status.bleeding += env_status.bleeding;
      status.temp += temp_d > 0 ? 1 : temp_d < 0 ? -1 : 0;

      if(mode == MODE_WORLD) {
	generate_map(&map, px - map.width / 2, py - map.height / 2, map.width, map.height);
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, map.width / 2, map.height / 2), map.width / 2, map.height / 2, ALIGN_LEFT);
      }
      else {
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, px, py), px, py, ALIGN_LEFT);

	if(get_tile_at(&map, 0, 0) == TILE_MOUNTAIN) {
	  set_entity(&map, px, py, ENTITY_WALKED_SNOW);
	}
      }

      if(mode == MODE_WORLD) {
	int wetness = get_tile_at(&map, map.width / 2, map.height / 2) == TILE_WATER ? 2 : 0;
	if(wetness > 0) {
	  status.wet += wetness;
	} else {
	  if(status.temp > 0) {
	    if(status.wet > 0) status.wet -= 1;
	  }
	}
      }

      if(status.wet && status.bleeding && randomi(1000) > 990) {
	status.infected += 1;
      }

      if(status.temp < 0) {
	if(randomi(1000) > 1000 + status.temp * (status.wet > 0 ? 2 : 1)) {
	  status.hypothermia += 1;
	}
      } else {
	if(randomi(2) > 0) { 
	  status.hypothermia -= status.hypothermia > 0 ? 1 : 0;
	}
      }

      if(status.hypothermia > 0) {
	if(randomi(1000) > 750) {
	  status.hp--;
	}
      }
      if(randomi(1000) > 900) {
	status.bleeding -= status.bleeding > 0 ? 1 : 0;
      }

      if(status.bleeding) {
	if(randomi(1000) > 990) {
	  status.hp--;
	}
      }
      if(status.infected) {
	if(randomi(1000) > 950) {
	  status.hp--;
	}
      }
      if(status.temp < 0) {
	if(randomi(1000) > 1000 - status.temp * 4) {
	  status.hp--;
	}
      }
      status.hp = status.hp < 0 ? 0 : status.hp;

      int penalty = 1;
      if(mode == MODE_WORLD) {
	penalty = get_tile_traverse_penalty(&map, get_tile_at(&map, map.width / 2, map.height / 2));
      }

      can_move = 6 * penalty;

      int print_len;
      if(mode == MODE_BIOME) {
	print_len = snprintf(line_buffer, SW, "Current biome: %s", get_biome_name(get_tile_at(&map, px, py)));
      } else {
	print_len = snprintf(line_buffer, SW, "Current biome: %s", get_biome_name(get_tile_at(&map, map.width / 2, map.height / 2)));
      }
      memset(line_buffer + print_len, ' ', SW - print_len);
      print_string(&screen, line_buffer, FG_WHITE | BG_BLACK, 0, SH - 2, ALIGN_LEFT);

      print_len = snprintf(line_buffer, SW, "Status: [%s%s%s%s%s]", status.wet > 0 ? "Wet": "Dry", status.temp < 0 ? " | Cold" : status.temp > 30 ? " | Hot" : "", status.hypothermia > 0 ? " | Hypothermia" : "", status.bleeding > 10 ? " | Profusely Bleeding" : status.bleeding > 0 ? " | Bleeding" : "", status.infected > 0 ? " | Infected" : "");
      memset(line_buffer + print_len, ' ', SW - print_len);
      print_string(&screen, line_buffer, FG_WHITE | BG_BLACK, 0, SH - 1, ALIGN_LEFT);

      snprintf(line_buffer, SW, "Temp: %d*C HP: %d/%d", status.temp, status.hp, status.max_hp);
      print_string(&screen, line_buffer, get_temp_attributes(status.temp), SW, SH - 1, ALIGN_RIGHT);
      snprintf(line_buffer, SW, "HP: %d/%d", status.hp, status.max_hp);
      print_string(&screen, line_buffer, FG_RED, SW, SH - 1, ALIGN_RIGHT);
      print_console(&screen);
    }

    Sleep(10);
  }

  free_map(&map);
  free_screen(&screen);
  
  waveOutClose(wave_device);

  return 0;
};
