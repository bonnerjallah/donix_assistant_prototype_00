#pragma once

#include <Arduino.h>
#include <driver/i2s.h>


const int MIC_WS_PIN  = 8;
const int MIC_SD_PIN  = 9;
const int MIC_SCK_PIN = 7;


#define MIC_I2S_PORT I2S_NUM_0
#define MIC_SAMPLE_RATE 24000
#define MIC_BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT
#define MIC_CHANNEL_FORMAT I2S_CHANNEL_FMT_ONLY_RIGHT

#define MIC_DMA_BUF_COUNT 8
#define MIC_DMA_BUF_LEN 1024


extern unsigned long micStartTime;

extern const unsigned long MIC_TIMEOUT;

extern bool micActive;

extern int micBufferCount;

extern const int MIC_WARMUP_BUFFERS;

extern const int32_t SOUND_THRESHOLD;

extern const int RECORD_SAMPLES;

extern int32_t micBuffer[1024];

extern int16_t pcmBuffer[1024];

extern int32_t* audioBuffer;

extern size_t audioIndex;

extern bool recording;


void mic_init();

int mic_read(int32_t* buffer, size_t samples);

void mic_deinit();

void startRecording();
void stopRecording();