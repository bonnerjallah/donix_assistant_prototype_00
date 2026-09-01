#include <Arduino.h>

#include "ws_socket.h"
#include "wifi_manager.h"
#include "oled.h"
#include "donix_face.h"
#include "mic.h"
#include "amp.h"
#include "bttn.h"

DonixFace donixFace(display);

void setup() {
    Serial.begin(115200);

    Serial.setDebugOutput(true);

    delay(1000);

    setupWiFi();

    setupWebSocket();

    setupOLED();

    donixFace.begin();

    mic_init();

    amp_init();

    setupButton();
}

void loop() {

    loopWebSocket();
    donixFace.update();

    ButtonEvents event = updateButton();

    // ==============================
    // LONG PRESS → START RECORDING
    // ==============================

    if (event == LONG_PRESS) {

        Serial.println("Button long pressed.");
        Serial.println("Recording started...");

        startRecording();
    }


    // ==============================
    // RECORD WHILE BUTTON IS HELD
    // ==============================

    if (recording) {

        size_t requestedSamples = 1024;

        int count = mic_read(
            micBuffer,
            requestedSamples
        );

        if (count > 0) {

            int16_t minPCM = 32767;
            int16_t maxPCM = -32768;

            for (int i = 0; i < count; i++) {

                int16_t pcm =
                    (int16_t)(micBuffer[i] >> 16);

                pcmBuffer[i] = pcm;

                if (pcm < minPCM)
                    minPCM = pcm;

                if (pcm > maxPCM)
                    maxPCM = pcm;
            }

            Serial.printf(
                "PCM RANGE: min=%d max=%d samples=%d\n",
                minPCM,
                maxPCM,
                count
            );

            sendAudioBIN(
                (uint8_t*)pcmBuffer,
                count * sizeof(int16_t)
            );
        }
    }


    // ==============================
    // BUTTON RELEASE
    // ==============================

    if (event == BUTTON_RELEASED) {

        Serial.println("Button released.");

        if (recording) {

            Serial.println("Recording stopped.");

            stopRecording();
        }
    }
}