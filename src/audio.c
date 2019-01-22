#include "audio.h"

#include <stdio.h>

/*
  CALLBACK FUNCTION
*/

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
  if(uMsg == WOM_OPEN) {
  } else if(uMsg == WOM_CLOSE) {
  } else if(uMsg == WOM_DONE) {
    // WAVEHDR* hdr = (WAVEHDR*)dwParam1;
    AudioODevice* device = (AudioODevice*)dwInstance;
    EnterCriticalSection(&device->critical_section);
    device->buffers_available++;
    LeaveCriticalSection(&device->critical_section);
  }
};

/*
  AUDIO FORMAT
 */

/*
  AUDIO DATA
 */

int load_audio_data_from_file(AudioData* audio, const char* filename) {
  FILE* f = fopen(filename, "rb");
  if(f == NULL) {
    return 1;
  }

  fseek(f, 0, SEEK_END);
  long long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  audio->current_position = 0;
  audio->data = (char*)malloc(size);
  audio->data_size = size;

  fread(audio->data, 1, size, f);
  fclose(f);

  return 0;
};

int free_audio_data(AudioData* audio) {
  free(audio->data);
  return 0;
};

int has_ended(AudioData* audio) {
  return audio->current_position >= audio->data_size ? 1 : 0;
}

void reset_audio_position(AudioData* audio) {
  audio->current_position = 0;
};

/*
  AUDIO MIXER
 */

int create_mixer_with_format(AudioMixer* mixer, int buffers, int data_size, AudioFormat format) {
  mixer->n_buffers = buffers;
  mixer->mixed_data = (char*)malloc(data_size * buffers);
  mixer->data_size = data_size;
  mixer->desired_format = format;
  return 0;
};

int free_mixer(AudioMixer* mixer) {
  free(mixer->mixed_data);
  return 0;
};

int prepare_mixer(AudioMixer* mixer) {
  mixer->current_buffer++;
  if(mixer->current_buffer >= mixer->n_buffers) {
    mixer->current_buffer = 0;
  }
  int offset = mixer->current_buffer * mixer->data_size;

  memset(((char*)mixer->mixed_data) + offset, 0, mixer->data_size);
  mixer->mixed_byte_count = 0;
  return 0;
};

int mix_audio(AudioMixer* mixer, AudioData* data, float volume) {
  int bytes = min(data->data_size - data->current_position, mixer->data_size);

  if(mixer->desired_format.bits_per_sample == 8) {
    int offset = mixer->current_buffer * mixer->data_size;
    for(int i = 0; i < bytes; i++) {
      ((char*)mixer->mixed_data)[i + offset] += (char)((float)(((char*)data->data)[data->current_position + i]) * volume);
    }
  }
  else if(mixer->desired_format.bits_per_sample == 16) {
    int offset = mixer->current_buffer * mixer->data_size / 2;
    for(int i = 0;i < bytes / 2;i++) {
      ((short*)mixer->mixed_data)[i + offset] += (short)((float)(((short*)data->data)[data->current_position / 2 + i]) * volume);
    }
  }

  data->current_position += bytes;
  mixer->mixed_byte_count = max(mixer->mixed_byte_count, bytes);
  return 0;
};

void* get_current_audio_data(AudioMixer* mixer) {
  int off = mixer->current_buffer * mixer->data_size;
  return (void*)(((char*)mixer->mixed_data) + off);
};

/*
  AUDIO OUTPUT BUFFER
 */
int init_audio_output_buffer(AudioOBuffer* buffer, int buffer_size) {
  WAVEHDR hdr = { 0 };
  hdr.lpData = (void*)0;
  hdr.dwBufferLength = buffer_size;
  hdr.dwFlags = 0;

  buffer->header = hdr;
  buffer->max_size = buffer_size;
  buffer->prepared = 0;
  buffer->data = (char*)malloc(buffer_size);
  return 0;
};

int free_audio_output_buffer(void* device_ptr, AudioOBuffer* buffer) {
  AudioODevice* device = (AudioODevice*)device_ptr;
  waveOutUnprepareHeader(device->device, &buffer->header, sizeof(WAVEHDR));
  free(buffer->data);
  return 0;
};

void write_buffer(AudioOBuffer* buffer, void* data, int byte_count) {
  memcpy((void*)buffer->data, (void*)data, byte_count);
  WAVEHDR* hdr = &buffer->header;
  hdr->lpData = (void*)buffer->data;
  hdr->dwBufferLength = byte_count;
  hdr->dwFlags = 0;
  hdr->dwBytesRecorded = 0;
  hdr->dwUser = 0;
  hdr->dwLoops = 0;
  hdr->lpNext = 0;
  hdr->reserved = 0;
};

/*
  AUDIO OUTPUT DEVICE
 */

int create_output_device(AudioODevice* device, int buffers, int buffer_size, int channels, int samples, int bits_per_sample) {
  WAVEFORMATEX wave_format = {
			      WAVE_FORMAT_PCM, // wFormatTag
			      channels, // nChannels
			      samples, // nSamplesPerSecond
			      samples * ((channels * bits_per_sample) / 8), // nAvgBytesPerSec = nSamplesPerSecond * nBlockAlign
			      (channels * bits_per_sample) / 8, // nBlockAlign = nChannels * wBitsPerSample / 8
			      bits_per_sample, // wBitsPerSample,
			      0 // cbSize, not used with PCM data
  };

  HWAVEOUT wave_device;
  if(waveOutOpen(&wave_device, -1, &wave_format, (DWORD_PTR)waveOutProc, (DWORD_PTR)device, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
    return 1;
  }

  AudioFormat format = { 0 };
  format.channels = channels;
  format.samples = samples;
  format.bits_per_sample = bits_per_sample;

  device->win_format = wave_format;
  device->format = format;
  device->device = wave_device;
  device->buffer_size = buffer_size;

  device->n_buffers = buffers;
  device->buffers = (AudioOBuffer*)malloc(sizeof(AudioOBuffer) * buffers);
  for(int b = 0;b < buffers;b++) {
    init_audio_output_buffer(&device->buffers[b], buffer_size);
  }
  device->buffers_available = buffers;
  InitializeCriticalSection(&device->critical_section);
  return 0;
};

int free_output_device(AudioODevice* device) {
  waveOutClose(device->device);
  DeleteCriticalSection(&device->critical_section);
  for(int b = 0;b < device->n_buffers;b++) {
    free_audio_output_buffer((void*)device, &device->buffers[b]);
  }
  free(device->buffers);
  return 0;
};

int queue_data_to_output_device(AudioODevice* device, void* data, int byte_count) {
  if(device->buffers_available <= 0) {
    printf("No buffers available.\n");
    return 1;
  }

  AudioOBuffer* buffer = &device->buffers[device->buffers_available - 1];
  write_buffer(buffer, data, byte_count);

  if(buffer->prepared) {
    waveOutUnprepareHeader(device->device, &buffer->header, sizeof(WAVEHDR));
  }
  waveOutPrepareHeader(device->device, &buffer->header, sizeof(WAVEHDR));
  buffer->prepared = 1;

  MMRESULT write_success = waveOutWrite(device->device, &buffer->header, sizeof(WAVEHDR));
  if(write_success != MMSYSERR_NOERROR) {
    char err_buffer[256]; 
    waveOutGetErrorText(write_success, err_buffer, 255);
    printf("Failed to write to audio buffer. Error: %s\n", err_buffer);
    return 1;
  }
  device->buffers_available--;
  return 0;
};

int is_format_supported(WAVEFORMATEX format, UINT device) {
  return waveOutOpen(NULL, device, &format, (DWORD_PTR)NULL, 0, WAVE_FORMAT_QUERY ) == MMSYSERR_NOERROR ? 1 : 0;
};

void enumerate_output_devices(WAVEFORMATEX format) {
  UINT num_devs = waveOutGetNumDevs();
  for(int i = num_devs - 1; i >= 0; i--) {
    WAVEOUTCAPS caps;
    waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS));
    int support = is_format_supported(format, i);
    printf("\tCan use: [%s] - Name: %s\n", support ? "x" : " ", caps.szPname);
  }
};
