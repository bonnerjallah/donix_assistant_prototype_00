#include <Arduino.h>
#include "mic.h"
#include "esp_heap_caps.h"
#include "ws_socket.h"

unsigned long micStartTime = 0;

const unsigned long MIC_TIMEOUT = 60000;

bool micActive = false;

int micBufferCount = 0;

const int MIC_WARMUP_BUFFERS = 10;

const int32_t SOUND_THRESHOLD = 30000000;

const int RECORD_SAMPLES = 16000 * 5;

int32_t micBuffer[1024];

int16_t pcmBuffer[1024];

int32_t* audioBuffer = nullptr;

size_t audioIndex = 0;

bool recording = false;


void mic_init() {

    Serial.println("Initializing microphone...");

    i2s_config_t i2s_config = {

        .mode = (i2s_mode_t)(
            I2S_MODE_MASTER |
            I2S_MODE_RX
        ),

        .sample_rate = MIC_SAMPLE_RATE,

        .bits_per_sample = MIC_BITS_PER_SAMPLE,

        .channel_format = MIC_CHANNEL_FORMAT,

        .communication_format = (i2s_comm_format_t)(
            I2S_COMM_FORMAT_I2S |
            I2S_COMM_FORMAT_I2S_MSB
        ),

        .intr_alloc_flags = 0,

        .dma_buf_count = MIC_DMA_BUF_COUNT,

        .dma_buf_len = MIC_DMA_BUF_LEN,

        .use_apll = false,

        .tx_desc_auto_clear = false,

        .fixed_mclk = 0
    };


    i2s_pin_config_t pin_config = {

        .bck_io_num = MIC_SCK_PIN,

        .ws_io_num = MIC_WS_PIN,

        .data_out_num = I2S_PIN_NO_CHANGE,

        .data_in_num = MIC_SD_PIN
    };


    esp_err_t result = i2s_driver_install(
        MIC_I2S_PORT,
        &i2s_config,
        0,
        NULL
    );


    if (result != ESP_OK) {

        Serial.print("I2S install failed: ");

        Serial.println(result);

        return;
    }


    result = i2s_set_pin(
        MIC_I2S_PORT,
        &pin_config
    );


    if (result != ESP_OK) {

        Serial.print("I2S pin setup failed: ");

        Serial.println(result);

        return;
    }

    i2s_start(MIC_I2S_PORT);


    Serial.println("Microphone initialized.");
}


int mic_read(int32_t* buffer, size_t samples) {

    size_t bytes_read = 0;


    esp_err_t result = i2s_read(
        MIC_I2S_PORT,

        buffer,

        samples * sizeof(int32_t),

        &bytes_read,

        portMAX_DELAY
    );


    if (result != ESP_OK) {

        Serial.print("Mic read failed: ");

        Serial.println(result);

        return 0;
    }


    return bytes_read / sizeof(int32_t);
}


void mic_deinit() {

    i2s_driver_uninstall(MIC_I2S_PORT);
}

void startRecording() {
    recording = true;
}

void stopRecording() {
    recording = false;

    sendWebSocketText("RECORDING_COMPLETED");

    Serial.println("Recording completed.");
}