#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

#include "core.h"
#include "clock.h"
#include "perlin.h"
#include "audio.h"
#include "screen.h"
#include "map.h"
#include "status.h"
#include "item.h"

#define AUDIO_DISABLE 1

// Macros
// Character screen status print
#define STAT_PRINT(y, format, ...) snprintf(stat_buffer + (y) * (CSW + 1), CSW, format, __VA_ARGS__);

// Status line (bottom one) print
#define STATUS_LINE_PRINT(color, y, format, ...) print_len = snprintf(line_buffer, SW - printed, format, __VA_ARGS__); \
  printed += print_string(&screen, line_buffer, color, printed, y, ALIGN_LEFT); \

// Screen width and height
#define SW 120
#define SH 40

// Character sheet width and height
#define CSW 30
#define CSH (SH - 2)

// sinewave
// Generates a sinewave tone with frequency and amplitude
// Returns a pointer to the data, and len is set to bytes generated
void* sinewave(float amplitude, float frequency, int samples, int bits_per_sample, int channels, int* len) {
  if(!(bits_per_sample == 8 || bits_per_sample == 16)) {
    *len = 0;
    return NULL;
  }

  void* data = malloc((bits_per_sample / 8) * channels * samples);
  *len = (bits_per_sample / 8) * channels * samples;

  for(int i = 0;i < samples;i++) {
    float s = amplitude * sin((float)i * (frequency / (float)samples) * 6.28f);
    if(bits_per_sample == 8) {
      char v = (char)(127.0f * s);
      for(int c = 0;c < channels;c++) {
	((char*)data)[i * channels + c] = v;
      }
    } else if(bits_per_sample == 16) {
      short v = (short)(32767.0f * s);
      for(int c = 0;c < channels;c++) {
	((short*)data)[i * channels + c] = v;
      }
    }
  }
  return data;
};

int main(int argc, char** argv) {

  ItemList item_list;
  if(load_items(&item_list, "items/list.txt")) {
    printf("Failed to load item list!\n");
    return 1;
  }

  Inventory player_inventory;
  if(create_inventory(&player_inventory, item_list.n_items)) {
    printf("Failed to create inventory.\n");
    return 1;
  };

  for(int i = 0;i < item_list.n_items;i++) {
    inventory_add_items(&player_inventory, i, 2);
  }

  load_permutation("perlin_seed"); // Perlin noise seed

  // For detecting if the console is active
  HWND window_handle = GetForegroundWindow();

  // Create screen with dimensions [SW, SH]
  Screen screen;
  if(create_screen(&screen, SW, SH)) {
    printf("Failed to create screen.\n");
    return 1;
  }
  show_cursor(&screen, FALSE); // Disable cursor blinking

#if (AUDIO_DISABLE == 0)
  // Create audio output device
  AudioODevice output_device;
  if(create_output_device(&output_device, 8, 4096, 2, 44100, 16)) {  // Buffers, buffer_size, Channels, samples, bits_per_sample
    printf("Failed to create audio device.\n");
    return 1;
  }

  // Load audio file hurt.raw
  AudioData hurt;
  if(load_audio_data_from_file(&hurt, "Hurt.raw")) {
    printf("Failed to load audiofile: 'Hurt.raw'\n");
    return 1;
  }

  // Load background music from wav file
  AudioData music;
  if(load_audio_data_from_ogg(&music, "Duet_for_Violin_and_Piano.ogg")) {
    printf("Failed to load background music.\n");
    return 1;
  }

  // Create mixer for output device
  AudioMixer mixer;
  if(create_mixer_for_device(&mixer, &output_device)) {
    printf("Failed to create audio mixer.\n");
    return 1;
  }

  // Print all the output devices for the fun of it
  enumerate_output_devices(output_device.win_format);
#endif

  // Line buffer, so we don't have to re-allocate memory every frame
  char line_buffer[SW + 1];
  memset(line_buffer, ' ', SW);
  line_buffer[SW] = 0;

  // Create map
  Map map;
  create_map(&map, SW, SH - 2);
  generate_map(&map, 0, 0, map.width, map.height); // Generates world map using perlin-noise

  // Movement cooldown
  float can_move = 0.0f;

  // Coordinates
  int map_x, map_y; // World offset saved when entering biome mode
  int px = 0, py = 0; // Player coordinates

  // Player status
  Status status = { 0 };
  init_status(&status, 25, 2500, 2500);

  // Flags for rendering and ticking
  int should_tick = 1, should_render = 1;

  // Two different game modes
  const int MODE_WORLD = 0;
  const int MODE_BIOME = 1;

  // Flags
  int show_character_sheet = 0;
  int show_inventory = 0;
  int inventory_scroll = 0;
  int selected_item = inventory_get_next_item(&player_inventory, -1);
  int mode = MODE_WORLD;

  // Input flags so they only trigger once when pressed
  int space_last = 0, c_last = 0, i_last = 0;
  int up_last = 0, down_last = 0, right_last = 0;

  // How much player moved this frame
  int d_x = 0, d_y = 0;

  // Delta time
  Clock runtime_clock;
  start_clock(&runtime_clock);
  reset_clock(&runtime_clock);

  // Flag if 'hurt.raw' should be played
  int play_hurt = 0;

  float total_time = 0.0f;
  float dt = 0.0f;

  while(1) {
    // Current runtime count in seconds
    dt = get_clock_delta_s(&runtime_clock);
    total_time += dt;
    reset_clock(&runtime_clock);

#if AUDIO_DISABLE == 0
    // If there is a free audio buffer available
    if(output_device.buffers_available > 0) {
      // Prepare mixer to receive data (clear last packet data)
      prepare_mixer(&mixer);
      // Should the hurt audio play (This should be later moved into its' own struct)
      if(play_hurt) {
	// Mix the hurt audio clip to the current mix
 	mix_audio(&mixer, &hurt, 0.2f);
	// If clip has ended, stop playing it and set the current position to start
	if(hurt.current_position >= hurt.data_size) {
	  play_hurt = 0;
	  reset_audio_position(&hurt);
	}
      }

      // Play background music 'on loop'
      /*
	mix_audio(&mixer, &music, 1.0f);
      if(has_ended(&music)) {
	reset_audio_position(&music);
      }
      */

      // Actually push the audio data to the output device
      queue_data_to_output_device(&output_device, &mixer);
    }
#endif

    // If the window is not focused, don't bother updating
    if(window_handle != GetForegroundWindow()) continue;
    // If the player has died
    if(status.hp <= 0) {
      print_string(&screen, "YOU DIED", FG_WHITE | BG_BLACK, SW / 2, SH / 2, ALIGN_CENTER);
      print_string(&screen, "Press escape to exit.", FG_WHITE | BG_BLACK, SW / 2, SH / 2 + 1, ALIGN_CENTER);
      print_console(&screen);
      if(GetKeyState(VK_ESCAPE) & 0x8000) {
	break;
      }
      continue;
    };

    if(can_move > 0.0f) {
      can_move -= dt;
    }

    if(GetKeyState(VK_ESCAPE) & 0x8000) {
      break;
    }

    // Toggle inventory (Press I)
    if(GetKeyState(0x49) & 0x8000 && i_last == 0) {
      show_inventory = ~show_inventory & 1;
      should_render = 1;
      show_character_sheet = 0;
    } i_last = GetKeyState(0x49) & 0x8000;
    
    // Toggle character sheet (Press C)
    if(GetKeyState(0x43) & 0x8000 && c_last == 0) {
      show_character_sheet = ~show_character_sheet & 1;
      should_render = 1;
      show_inventory = 0;
    } c_last = GetKeyState(0x43) & 0x8000;

    // Inventory item selection up
    if(GetKeyState(VK_UP) & 0x8000 && up_last == 0) {
      if(show_inventory) {
	int prev = inventory_get_previous_item(&player_inventory, selected_item);
	if(prev != -1) {
	  selected_item = prev;
	  should_render = 1;
	}
      }
    } up_last = GetKeyState(VK_UP) & 0x8000;

    // Inventory item selection down
    if(GetKeyState(VK_DOWN) & 0x8000 && down_last == 0) {
      if(show_inventory) {
	int next = inventory_get_next_item(&player_inventory, selected_item);
	if(next != -1) {
	  selected_item = next;
	  should_render = 1;
	}
      }
    } down_last = GetKeyState(VK_DOWN) & 0x8000;

    // Pressing right (Using items)
    if(GetKeyState(VK_RIGHT) & 0x8000 && right_last == 0) {
      Item item = item_list.items[selected_item];
      if(use_item_for_status(&item, &status)) {
	if(inventory_take_items(&player_inventory, selected_item, 1) == 0) {
	  selected_item = inventory_get_next_item(&player_inventory, selected_item);
	}
      } else if(use_item_for_equipment(&item, &player_inventory)) {
	
      }
      should_render = 1;
    } right_last = GetKeyState(VK_RIGHT) & 0x8000;

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
      should_tick = 1;
    }
    space_last = GetKeyState(VK_SPACE) & 0x8000;

    if(GetKeyState(0x57) & 0x8000 && can_move <= 0.0f) {
      d_y = -1; // Move up
    }
    if(GetKeyState(0x41) & 0x8000 && can_move <= 0.0f) {
      d_x = -1; // Move left
    }
    if(GetKeyState(0x53) & 0x8000 && can_move <= 0.0f) {
      d_y = 1; // Move down
    }
    if(GetKeyState(0x44) & 0x8000 && can_move <= 0.0f) {
      d_x = 1; // Move right
    }

    Status env_status = { 0 };
    if(mode == MODE_WORLD) {
      px += d_x;
      py += d_y;

      if(d_x != 0 || d_y != 0) should_tick = 1;

      env_status.temp = get_tile_temp(get_tile_at(&map, map.width / 2, map.height / 2));
    } 
    else if(mode == MODE_BIOME) {
      if(d_x != 0 || d_y != 0) {
	try_move_to(&map, (px + d_x + map.width) % map.width, py, &env_status);
	try_move_to(&map, px, (py + d_y + map.height) % map.height, &env_status);

	if(can_move_to(&map, (px + d_x + map.width) % map.width, py)) {
	  px += d_x;
	  should_tick = 1;
	}
	if(can_move_to(&map, px, (py + d_y + map.height) % map.height)) {
	  py += d_y;
	  should_tick = 1;
	}

	px = (px + map.width) % map.width;
	py = (py + map.height) % map.height;

	MapEntity* entity = get_entity(&map, px, py);
	if(entity->tile == ENTITY_MONEY) {
	  inventory_add_items(&player_inventory, get_item_by_name(&item_list, "Gold"), entity->metadata);
	  set_entity(&map, px, py, ENTITY_UNDEF, 0);
	}

	env_status.temp = get_tile_temp(get_tile_at(&map, px, py));
      }
    }

    d_x = d_y = 0;

    if(should_render | should_tick) {

      if(should_tick) {
	apply_status(env_status, &status); // Apply environment status to player

	if(mode == MODE_WORLD) {
	  int wetness = get_tile_at(&map, map.width / 2, map.height / 2) == TILE_WATER ? 5 : 0;
	  if(wetness > 0) {
	    status.wet += wetness;
	  } else {
	    if(status.temp > 0) {
	      if(status.wet > 0) status.wet -= 1;
	    }
	  }
	}

	if(tick_status(&status)) { // If player took damage
	  play_hurt = 1;
	}

	int penalty = 1;
	if(mode == MODE_WORLD) {
	  penalty = get_tile_traverse_penalty(&map, get_tile_at(&map, map.width / 2, map.height / 2));
	}
	can_move = (float)penalty * (1.0f / 6.0f);
      }

      // Rendering world map
      if(mode == MODE_WORLD) {
	generate_map(&map, px - map.width / 2, py - map.height / 2, map.width, map.height);
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, map.width / 2, map.height / 2), map.width / 2, map.height / 2, ALIGN_LEFT);
      }
      else {
	print_map(&map, &screen);
	print_string(&screen, "@", FG_BLUE | get_background_of_map_at(&map, px, py), px, py, ALIGN_LEFT);

	if(get_tile_at(&map, 0, 0) == TILE_MOUNTAIN) {
	  set_entity(&map, px, py, ENTITY_WALKED_SNOW, 0);
	}
      }

      // Rendering *UI*
      int print_len;
      int world_x, world_y;
      if(mode == MODE_BIOME) {
	world_x = px;
	world_y = py;
      } else {
	world_x = map.width / 2;
	world_y = map.height / 2;
      }

      // Print current biome
      print_len = snprintf(line_buffer, SW, "Current biome: %s", get_biome_name(get_tile_at(&map, world_x, world_y)));
      memset(line_buffer + print_len, ' ', SW - print_len);
      print_string(&screen, line_buffer, FG_WHITE | BG_BLACK, 0, SH - 2, ALIGN_LEFT);

      // Print current health for faster seeing than character sheet
      snprintf(line_buffer, SW, "HP: %d/%d", status.hp, status.max_hp);
      print_string(&screen, line_buffer, FG_LIGHT_RED, SW, SH - 2, ALIGN_RIGHT);

      // Print all the status effects, colored
      int printed = 0;

      // Clear line
      memset(line_buffer, ' ', SW);
      print_string(&screen, line_buffer, FG_BLACK, 0, SH - 1, ALIGN_LEFT);

      printed += print_string(&screen, "Status: [", FG_WHITE, 0, SH - 1, ALIGN_LEFT);

      STATUS_LINE_PRINT(status.wet > 0 ? FG_TURQUOISE : FG_YELLOW, SH - 1, "%s", status.wet > 0 ? "Wet" : "Dry");

      if(status.hypothermia <= 0 && status.heat_stroke <= 0) {
	STATUS_LINE_PRINT(status.temp > 30 ? FG_RED : status.temp < 0 ? FG_CYAN : FG_WHITE, SH - 1, "%s", status.temp < 0 ? " Cold" : status.temp > 30 ? " Hot" : "");
      } else if(status.hypothermia > 0) {
	STATUS_LINE_PRINT(FG_CYAN, SH - 1, " Hypothermia", 0);
      } else if(status.heat_stroke > 0) {
	STATUS_LINE_PRINT(FG_LIGHT_RED, SH - 1, " Heat Stroke", 0);
      }

      STATUS_LINE_PRINT(FG_LIGHT_RED, SH - 1, "%s", status.bleeding > 10 ? " Profusely Bleeding" : status.bleeding > 0 ? " Bleeding" : "");

      STATUS_LINE_PRINT(FG_RED, SH - 1, "%s", status.infected > 0 ? " Infected" : "");

      print_string(&screen, "]", FG_WHITE, printed, SH - 1, ALIGN_LEFT);

      // Temperature
      snprintf(line_buffer, SW, "Temp: %d*C", status.temp);
      print_string(&screen, line_buffer, get_temp_attributes(status.temp), SW, SH - 1, ALIGN_RIGHT);

      // Character sheet
      if(show_character_sheet) {
	int x = 0;
	if(px < SW / 2 + 1) {
	  x = SW - CSW;
	}

	char c_buffer[CSW + 1];
	c_buffer[CSW] = 0;
	memset(c_buffer, ' ', CSW);
	for(int y = 0;y < CSH; y++) {
	  print_string(&screen, c_buffer, FG_WHITE, x, y, ALIGN_LEFT);
	}

	char stat_buffer[(CSW + 1) * CSH];
	memset(stat_buffer, 0, (CSW + 1) * CSH);

	print_string(&screen, "STATUS", FG_WHITE, x + CSW / 2, 0, ALIGN_CENTER); // Sheet title
	STAT_PRINT(2, "Health: %d / %d", status.hp, status.max_hp);
	STAT_PRINT(3, "Hunger: %d / %d", status.hunger / 25, status.max_hunger / 25);
	STAT_PRINT(4, "Thirst: %d / %d", status.thirst / 25, status.max_thirst / 25);

	int wpn = player_inventory.equipped_items[EQUIP_SLOT_WEAPON];

	STAT_PRINT(6, "WPN: %s", wpn != -1 ? item_list.items[wpn].name : "None");

	for(int y = 0;y < CSH;y++) {
	  int color = FG_WHITE;
	  if(x == 0) {
	    print_string(&screen, stat_buffer + y * (CSW + 1), color, x + 1, y, ALIGN_LEFT);
	  } else {
	    print_string(&screen, stat_buffer + y * (CSW + 1), color, x + CSW - 2, y, ALIGN_RIGHT);
	  }
	}
      }

      // Inventory
      if(show_inventory) {
	// If the inventory should be shown on the left side of the screen or the right side.
	int x = 0;
	if(px < SW / 2 + 1) {
	  x = SW - CSW;
	}

	// Sheet background buffer
	char c_buffer[CSW + 1];
	c_buffer[CSW] = 0;
	memset(c_buffer, ' ', CSW);
	for(int y = 0;y < CSH; y++) {
	  print_string(&screen, c_buffer, FG_WHITE, x, y, ALIGN_LEFT);
	}

	// 'Status line' buffer
	char stat_buffer[(CSW + 1) * CSH];
	memset(stat_buffer, 0, (CSW + 1) * CSH);

	print_string(&screen, "INVENTORY", FG_WHITE, x + CSW / 2, 0, ALIGN_CENTER); // Sheet title

	int line = 2; // Current line we are printing to 
	for(int i = inventory_scroll; i < player_inventory.n_items; i++) { // For each item available
	  if(player_inventory.items[i] > 0) { // If the player has any
	    // Print on the current line the item name + amount
	    STAT_PRINT(line, "[%c] %s x %d", i == selected_item ? 'x' : ' ',
		       item_list.items[i].name,
		       player_inventory.items[i]);
	    line++; // Advance on lines
	    if(line >= CSH - 2) break; // Don't render over the sheet
	  }
	}
	
	for(int y = 0;y < CSH;y++) {
	  int color = inventory_unique_nth_count(&player_inventory, selected_item) - inventory_scroll == y - 2 ? FG_LIGHT_RED : FG_WHITE;
	  if(x == 0) {
	    print_string(&screen, stat_buffer + y * (CSW + 1), color, x + 1, y, ALIGN_LEFT);
	  } else {
	    print_string(&screen, stat_buffer + y * (CSW + 1), color, x + CSW - 2, y, ALIGN_RIGHT);
	  }
	}
      }

      print_console(&screen);

      should_render = should_tick = 0;
    }

    Sleep(1);
  }

  free_map(&map);
  free_screen(&screen);

#if AUDIO_DISABLE == 0
  free_audio_data(&music);
  free_audio_data(&hurt);
  free_output_device(&output_device);
#endif

  free_inventory(&player_inventory);
  free_items(&item_list);

  return 0;
};
