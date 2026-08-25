#include <Arduino.h>
#include "WiFi.h"

#include "wifi_manager.h"
#include "secrets.h"


void setupWiFi() {

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    delay(1000);

    Serial.println("Scaning for available networks");

    int networkFound = WiFi.scanNetworks();

    if(networkFound == 0) {
        Serial.println("No networks found.");
        return;
    } else {
        Serial.print("Networks found: ");
        Serial.println(networkFound);

        for(int i = 0; i < networkFound; i++) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" | RSSI: ");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm");
            Serial.print(" | Encryption: ");
            Serial.println(WiFi.encryptionType(i));
        }
    }

    const char* bestSSID = nullptr;
    const char* knownPassword = nullptr;

    int bestRSSI = -1000;

    for(int i = 0; i < networkFound; i++) {

        String foundSSID = WiFi.SSID(i);
        int32_t foundRSSI = WiFi.RSSI(i);

        Serial.print("Found network: ");
        Serial.print(foundSSID);
        Serial.print(" | RSSI: ");
        Serial.println(foundRSSI);


        for(KnownNetwork network : KnownNetworks) {
            if(foundSSID == network.ssid && foundRSSI > bestRSSI) {
                bestSSID = network.ssid;
                knownPassword = network.password;
                bestRSSI = foundRSSI;
            }
        }

    }

    if(bestSSID == nullptr) {
        Serial.println("No known networks found.");
        return;
    }


    WiFi.begin(bestSSID, knownPassword);

    unsigned long startAttemptTime = millis();

    while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
        Serial.print(".");
        delay(500);
    }

    if(WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi successfully.");
        Serial.print("Connected to SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect to WiFi.");
    }

}

