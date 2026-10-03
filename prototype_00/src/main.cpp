#include <Arduino.h>

#include "ws_socket.h"
#include "wifi_manager.h"
#include "oled.h"
#include "donix_face.h"
#include "mic.h"
#include "amp.h"
#include "bttn.h"
#include "audioqueue.h"


// ============================================================
// GLOBALS
// ============================================================

DonixFace donixFace(display);


// 96,000 samples
// 24,000 samples/sec
// = approximately 4 seconds of MONO PCM

PcmAudioQueue audioQueue(96000);


// Playback block
int16_t playbackBuffer[512];


// ============================================================
// PLAYBACK TASK
// ============================================================

void audioPlaybackTask(void* parameter) {

    Serial.println("Audio playback task started.");

    static int16_t playbackBuffer[512];

    while (true) {

        if (audioQueue.available() >= 512) {

            if (audioQueue.pop(playbackBuffer, 512)) {

                amp_play_chunk(
                    playbackBuffer,
                    512
                );
            }

        } else {

            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
}

// ============================================================
// SETUP
// ============================================================

void setup() {
    Serial.begin(115200);

    Serial.setDebugOutput(true);

    delay(1000);


    Serial.println();
    Serial.println("==============================");
    Serial.println("DONIX AI");
    Serial.println("==============================");


    // --------------------------------------------------------
    // WIFI
    // --------------------------------------------------------

    setupWiFi();


    // --------------------------------------------------------
    // WEBSOCKET
    // --------------------------------------------------------

    setupWebSocket();


    // --------------------------------------------------------
    // OLED
    // --------------------------------------------------------

    setupOLED();

    donixFace.begin();


    // --------------------------------------------------------
    // MICROPHONE
    // --------------------------------------------------------

    mic_init();


    // --------------------------------------------------------
    // AMPLIFIER
    // --------------------------------------------------------

    amp_init();


    // --------------------------------------------------------
    // BUTTON
    // --------------------------------------------------------

    setupButton();


    // --------------------------------------------------------
    // START AUDIO PLAYBACK TASK
    // --------------------------------------------------------

    xTaskCreatePinnedToCore(
        audioPlaybackTask,
        "AudioPlayback",
        4096,
        nullptr,
        2,
        nullptr,
        0
    );


    Serial.println(
        "System initialization complete."
    );
}


// ============================================================
// LOOP
// ============================================================

void loop() {
    // --------------------------------------------------------
    // WEBSOCKET
    // --------------------------------------------------------

    loopWebSocket();


    // --------------------------------------------------------
    // FACE
    // --------------------------------------------------------

    donixFace.update();


    // --------------------------------------------------------
    // BUTTON
    // --------------------------------------------------------

    ButtonEvents event = updateButton();


    // --------------------------------------------------------
    // START RECORDING
    // --------------------------------------------------------

    if ( event == LONG_PRESS) {

        Serial.println("Button long pressed.");

        Serial.println("Recording started...");

        startRecording();
    }


    // --------------------------------------------------------
    // RECORDING
    // --------------------------------------------------------

    if (recording) {

        size_t requestedSamples = 1024;

        int count = mic_read( micBuffer, requestedSamples);


        if (count > 0) {

            int16_t minPCM = 32767;

            int16_t maxPCM = -32768;


            for ( int i = 0; i < count; i++) {

                int16_t pcm = (int16_t)(
                    micBuffer[i] >> 16
                );


                pcmBuffer[i] = pcm;


                if (pcm < minPCM) {
                    minPCM = pcm;
                }


                if (pcm > maxPCM) {
                    maxPCM = pcm;
                }
            }


            Serial.printf("PCM RANGE: min=%d max=%d samples=%d\n", minPCM, maxPCM, count);


            sendAudioBIN((uint8_t*)pcmBuffer, count * sizeof(int16_t));
        }
    }


    // --------------------------------------------------------
    // STOP RECORDING
    // --------------------------------------------------------

    if (event == BUTTON_RELEASED) {

        Serial.println("Button released.");


        if (recording) {

            Serial.println("Recording stopped.");

            stopRecording();
        }
    }


    // Give the main loop some breathing room.

    delay(1);
}