#include <Arduino.h>
#include "bttn.h"

const int BUTTON_PIN = 43;

namespace Button {
   constexpr unsigned long DEBOUNCE_DELAY = 50; // milliseconds
   constexpr unsigned long LONG_PRESS_THRESHOLD = 1000; // milliseconds

   bool buttonState = HIGH;
   bool lastButtonState = HIGH;

   unsigned long lastDebounceTime = 0;
   unsigned long pressStartTime = 0;

   bool longPressDetected = false;
}

void setupButton() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Button::buttonState = digitalRead(BUTTON_PIN);
    Button::lastButtonState = Button::buttonState;

    Button::lastDebounceTime = millis();
}

ButtonEvents updateButton() {

    bool currentState = digitalRead(BUTTON_PIN);
    unsigned long currentTime = millis();

    // Detect raw state change
    if (currentState != Button::lastButtonState) {
        Button::lastDebounceTime = currentTime;
    }

    Button::lastButtonState = currentState;

    // Debounce
    if ((currentTime - Button::lastDebounceTime) <= Button::DEBOUNCE_DELAY) {
        return NO_EVENT;
    }

    // Stable state changed
    if (currentState != Button::buttonState) {

        Button::buttonState = currentState;

        // -------------------------
        // BUTTON PRESSED
        // -------------------------
        if (Button::buttonState == LOW) {

            Button::pressStartTime = currentTime;
            Button::longPressDetected = false;

            return NO_EVENT;
        }

        // -------------------------
        // BUTTON RELEASED
        // -------------------------
        else {

            return BUTTON_RELEASED;
        }
    }

    // -------------------------
    // LONG PRESS
    // -------------------------
    if (Button::buttonState == LOW &&
        !Button::longPressDetected &&
        (currentTime - Button::pressStartTime >=
         Button::LONG_PRESS_THRESHOLD)) {

        Button::longPressDetected = true;

        return LONG_PRESS;
    }

    return NO_EVENT;
}

void checkButtonState() {
    ButtonEvents event = updateButton();
    if (event == LONG_PRESS) {
        Serial.println("Button long pressed.");
    } else if (event == BUTTON_RELEASED) {
        Serial.println("Button released.");
    }
}