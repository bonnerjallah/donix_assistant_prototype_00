#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

#include "ws_socket.h"
#include "secrets.h"

WebSocketsClient webSocket;

void sendWebSocketText(const char* message) {
    webSocket.sendTXT(message);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    Serial.print("WS EVENT: ");
    Serial.println((int)type);

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("ESP32 -> WebSocket DISCONNECTED");

            break;

        case WStype_CONNECTED:
            Serial.println("ESP32 -> WebSocket CONNECTED");
            webSocket.sendTXT("Hello from ESP32");

            break;

        case WStype_TEXT:
            Serial.print("ESP32 <- ");
            Serial.println((char*)payload);

            break;

        case WStype_BIN:
            Serial.println("ESP32 <- Binary received");

            break;

        case WStype_ERROR:
            Serial.println("ESP32 <- WebSocket error");

            if (payload != nullptr) {
                Serial.write(payload, length);
                Serial.println();
            }
            
            break;
            
        default:
            Serial.println("ESP32 -> Other WebSocket event");

            break;
    }
}

void setupWebSocket() {

    webSocket.begin(
        NODE_SERVER_IP,
        3001,
        "/ws"
    );

    webSocket.onEvent(webSocketEvent);

    webSocket.setReconnectInterval(5000);

    Serial.println("WebSocket client started");
}

void loopWebSocket() {
    webSocket.loop();
}

void sendAudioBIN(const uint8_t* data, size_t length) {
    webSocket.sendBIN(data, length);
}