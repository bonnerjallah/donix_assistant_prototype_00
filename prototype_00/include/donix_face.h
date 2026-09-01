#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>



constexpr int DONIX_FACE_WIDTH = 128;
constexpr int DONIX_FACE_HEIGHT = 64;


enum DonixExpression {
    DONIX_NORMAL,
    DONIX_HAPPY,
    DONIX_SAD,
    DONIX_ANGRY,
    DONIX_SURPRISED,
    DONIX_SLEEPY,
    DONIX_THINKING,
};

enum DonixState {
    DONIX_IDLE,
    DONIX_LISTENING,
    DONIX_THINKING_STATE,
    DONIX_SPEAKING,
    DONIX_BLINKING
};

class DonixFace {
    public: 
        DonixFace(Adafruit_SH1106G& display);
        bool begin();
        void update();
        void setExpression(DonixExpression expression);
        void setState(DonixState state);
        void setBlinking();
        void setTalk();
        void setStopTalking();
        void setListen();
        void setThinking();
        void setHappy();
        void setSad();
        void setAngry();
        void setSurprised();
        void setSleepy();
        void setNormal();

    private:
        Adafruit_SH1106G& display_;
        DonixExpression currentExpression_;
        DonixState currentState_;

        bool _talking;

        unsigned long _lastBlinkTime;
        unsigned long _blinkStart;

        unsigned long _lastTalkTime;
        unsigned long _talkStart;

        bool _blinkActive;
        uint8_t _blinkFrame = 0;
        uint8_t _talkFrame = 0;



        void drawFace();
        void drawEyes();
        void drawEyeBrows();
        void drawMouth();
        void drawNormalEyes();
        void drawHappyEyes();
        void drawSadEyes();
        void drawAngryEyes();
        void drawSurprisedEyes();
        void drawSleepyEyes();
        void drawThinkingEyes();

        void drawNormalBrows();
        void drawHappyBrows();
        void drawSadBrows();
        void drawAngryBrows();
        void drawSurprisedBrows();
        void drawSleepyBrows();
        void drawThinkingBrows();

        void drawClosedMouth();
        void drawSmile();
        void drawSadMouth();
        void drawOpenMouth();
        void drawWideMouth();
        void drawTalkingMouth();
        void drawThinkingMouth();
        void drawBlink();
        void clearFace();
};