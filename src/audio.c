#include "audio.h"

#include <stdio.h>

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

int init_audio_output_buffer(void* device_ptr, AudioOBuffer* buffer, int buffer_size) {
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
};

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
  waveOutSetVolume(wave_device, 0xFFFF);

  device->format = wave_format;
  device->device = wave_device;
  device->channels = channels;
  device->buffer_size = buffer_size;

  device->n_buffers = buffers;
  device->buffers = (AudioOBuffer*)malloc(sizeof(AudioOBuffer) * buffers);
  for(int b = 0;b < buffers;b++) {
    init_audio_output_buffer((void*)device, &device->buffers[b], buffer_size);
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
