#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

const int AMP_BCLK_PIN = 2;
const int AMP_LRC_PIN  = 1;
const int AMP_DIN_PIN  = 3;
const int AMP_SD_PIN   = 4;

//I2S configuration for the amplifier
const i2s_port_t AMP_I2S_PORT = I2S_NUM_1;
const int AMP_SAMPLE_RATE = 16000;
const int AMP_DMA_BUFFER_COUNT = 8;
const int AMP_DMA_BUFFER_LENGTH = 1024;

//Functions
void amp_init();

int amp_play(
    int16_t* audioData,
    size_t sampleCount
);

// NEW
int amp_play_chunk(
    uint8_t* data,
    size_t length
);

int amp_play_chunk(uint8_t* data, size_t length);

void amp_stop();

void amp_deinit();