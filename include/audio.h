#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <windows.h>

typedef struct {
  int channels;
  int samples;
  int bits_per_sample;
} AudioFormat;

extern AudioFormat create_audio_format(int, int, int);

typedef struct {
  void* data;
  AudioFormat format;
} AudioData;

extern int load_audio_data_from_file(AudioData*, const char*);

typedef struct {
  void* mixed_data;
  AudioFormat desired_format; // Should be the same as AudioODevice format
} AudioMixer;

extern int create_audio_mixer(AudioMixer*);

typedef struct {
  WAVEHDR header;
  int max_size;
  int prepared;
  char* data;
} AudioOBuffer;

extern int init_audio_output_buffer(void*, AudioOBuffer*, int);
extern int free_audio_output_buffer(void*, AudioOBuffer*);

typedef struct {
  WAVEFORMATEX win_format;
  HWAVEOUT     device;
  AudioFormat format;
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
extern void enumerate_output_devices(WAVEFORMATEX format);

#endif
