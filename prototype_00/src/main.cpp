#include <Arduino.h>
#include "ws_socket.h"
#include "wifi_manager.h"
#include <WebSocketsClient.h>
#include "oled.h"
#include "donix_face.h"

DonixFace donixFace(display);


void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupWebSocket();
    setupOLED();
    donixFace.begin();
}

void loop() {
    loopWebSocket();
    donixFace.update();
}