#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

const int AMP_BCLK_PIN = 2;
const int AMP_LRC_PIN  = 1;
const int AMP_DIN_PIN  = 3;
const int AMP_SD_PIN   = 4;

const i2s_port_t AMP_I2S_PORT = I2S_NUM_1;

const int AMP_SAMPLE_RATE = 24000;

void amp_init();

bool amp_play_chunk( const int16_t* data, size_t sampleCount);

void amp_stop();

void amp_deinit();