#pragma once

void setupWebSocket();
void loopWebSocket();

void sendWebSocketText(const char* message);
void sendAudioBIN(const uint8_t* data, size_t length);



