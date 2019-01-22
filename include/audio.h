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
  int current_position;
  int data_size;
  AudioFormat format;
} AudioData;

extern int load_audio_data_from_wav(AudioData*, const char*);
extern int load_audio_data_from_file(AudioData*, const char*);
extern int load_audio_data_from_data(AudioData*, void*, int);
extern int free_audio_data(AudioData*);

extern int has_ended(AudioData*);
extern void reset_audio_position(AudioData*);

typedef struct {
  void* mixed_data;
  int mixed_byte_count;
  int data_size;
  int n_buffers;
  int current_buffer;
  AudioFormat desired_format; // Should be the same as AudioODevice format
} AudioMixer;

extern int create_mixer_with_format(AudioMixer*, int, int, AudioFormat);
extern int free_mixer(AudioMixer*);
extern int prepare_mixer(AudioMixer*);
extern int mix_audio(AudioMixer*, AudioData*, float volume);
extern void* get_current_audio_data(AudioMixer*);

typedef struct {
  WAVEHDR header;
  int max_size;
  char* data;
} AudioOBuffer;

extern int init_audio_output_buffer(AudioOBuffer*, int);
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

extern int queue_data_to_output_device(AudioODevice*, AudioMixer*);

extern int is_format_supported(WAVEFORMATEX format, UINT device);
extern void enumerate_output_devices(WAVEFORMATEX format);

#endif
