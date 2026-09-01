#pragma once

#include <Arduino.h>

enum ButtonEvents {
    NO_EVENT,
    LONG_PRESS,
    BUTTON_RELEASED
};

void setupButton();

ButtonEvents updateButton();

void checkButtonState();