#include "amp.h"


// ============================================================
// AMP INIT
// ============================================================

void amp_init() {
    Serial.println("Initializing amplifier...");

    // Enable amplifier
    pinMode(AMP_SD_PIN, OUTPUT);

    digitalWrite( AMP_SD_PIN,  HIGH);


    // --------------------------------------------------------
    // I2S CONFIGURATION
    // --------------------------------------------------------

    i2s_config_t i2s_config = {

        .mode = (i2s_mode_t)(
            I2S_MODE_MASTER |
            I2S_MODE_TX
        ),

        .sample_rate = AMP_SAMPLE_RATE,

        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,

        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,

        .communication_format = I2S_COMM_FORMAT_STAND_I2S,

        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,

        .dma_buf_count = 8,

        .dma_buf_len = 512,

        .use_apll = false,

        .tx_desc_auto_clear = true,

        .fixed_mclk = 0
    };


    // --------------------------------------------------------
    // INSTALL I2S DRIVER
    // --------------------------------------------------------

    esp_err_t result = i2s_driver_install(
        AMP_I2S_PORT,
        &i2s_config,
        0,
        nullptr
    );

    if (result != ESP_OK) {

        Serial.printf(
            "I2S driver install failed: %d\n",
            result
        );

        return;
    }


    // --------------------------------------------------------
    // I2S PINS
    // --------------------------------------------------------

    i2s_pin_config_t pin_config = {

        .bck_io_num = AMP_BCLK_PIN,

        .ws_io_num = AMP_LRC_PIN,

        .data_out_num = AMP_DIN_PIN,

        .data_in_num = I2S_PIN_NO_CHANGE
    };


    result = i2s_set_pin(
        AMP_I2S_PORT,
        &pin_config
    );

    if (result != ESP_OK) {

        Serial.printf(
            "I2S pin setup failed: %d\n",
            result
        );

        return;
    }


    i2s_zero_dma_buffer( AMP_I2S_PORT);

    Serial.println("Amplifier initialized successfully.");
}


// ============================================================
// PLAY PCM CHUNK
// ============================================================

bool amp_play_chunk( const int16_t* data, size_t sampleCount) {
    if ( data == nullptr || sampleCount == 0) {
        return false;
    }


    // --------------------------------------------------------
    // Convert MONO PCM → STEREO
    // --------------------------------------------------------

    static int16_t stereoBuffer[1024];

    if (sampleCount > 512) {
        return false;
    }


    for (size_t i = 0; i < sampleCount; i++) {

        stereoBuffer[i * 2] = data[i];

        stereoBuffer[i * 2 + 1] = data[i];
    }


    // --------------------------------------------------------
    // SEND TO I2S
    // --------------------------------------------------------

    size_t bytesWritten = 0;

    esp_err_t result = i2s_write(
        AMP_I2S_PORT,

        stereoBuffer,

        sampleCount * 2 * sizeof(int16_t),

        &bytesWritten,

        portMAX_DELAY
    );


    if (result != ESP_OK) {

        Serial.printf("I2S write failed: %d\n", result);

        return false;
    }

    return true;
}


// ============================================================
// STOP
// ============================================================

void amp_stop() {
    i2s_zero_dma_buffer( AMP_I2S_PORT);

    digitalWrite(AMP_SD_PIN, LOW);
}


// ============================================================
// DEINIT
// ============================================================

void amp_deinit() {
    digitalWrite(AMP_SD_PIN, LOW);

    i2s_driver_uninstall(AMP_I2S_PORT);
}