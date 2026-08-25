#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

#include "ws_socket.h"
#include "secrets.h"

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected");
            break;

        case WStype_CONNECTED:
            Serial.println("WebSocket connected");
            webSocket.sendTXT("Hello from ESP32");
            break;

        case WStype_TEXT:
            Serial.println("Message received from server:");
            Serial.println((char*)payload);
            break;

        case WStype_BIN:
            Serial.println("WebSocket binary data received");
            break;

        default:
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