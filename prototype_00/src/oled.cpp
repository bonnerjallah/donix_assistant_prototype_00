#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET = -1;

Adafruit_SH1106G display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);


void setupOLED() {
    Serial.println("Setting up OLED display...");

    Wire.begin(5, 6); // Initialize I2C with SDA on pin 5 and SCL on pin 6
    delay(250);

    if(!display.begin(0x3C, true)) {
        Serial.println("Failed to initialize OLED display!");
        return;
    }

    Serial.println("OLED display initialized successfully.");

    //----------------
    // START UP SCREEN
    //----------------

    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("DONIX-AI");
    display.display();

    delay(2000); // Display the start-up screen for 2 seconds

    display.clearDisplay(); // Clear the start-up screen after displaying it
}
