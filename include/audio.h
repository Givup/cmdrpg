#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <windows.h>

typedef struct {
  WAVEHDR header;
  int max_size;
  int prepared;
  char* data;
} AudioOBuffer;

extern int init_audio_output_buffer(void*, AudioOBuffer*, int);
extern int free_audio_output_buffer(void*, AudioOBuffer*);

typedef struct {
  WAVEFORMATEX format;
  HWAVEOUT     device;
  int channels;
  int n_buffers;
  int buffers_available;
  int buffer_size;
  AudioOBuffer* buffers;
  CRITICAL_SECTION critical_section;
} AudioODevice;

extern int create_output_device(AudioODevice*, int, int, int, int, int);
extern int free_output_device(AudioODevice*);

extern int queue_data_to_output_device(AudioODevice*, void*, int);

extern int is_format_supported(WAVEFORMATEX format, UINT device);

#endif
