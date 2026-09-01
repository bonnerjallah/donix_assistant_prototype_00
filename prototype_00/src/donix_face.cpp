#include "donix_face.h"

DonixFace::DonixFace( Adafruit_SH1106G& display ) : display_(display) {
    currentExpression_ = DONIX_NORMAL;
    currentState_ = DONIX_IDLE;

    _talking = false;

    _lastBlinkTime = 0;
    _blinkStart = 0;

    _lastTalkTime = 0;
    _talkStart = 0;

    _blinkActive = false;

    _talkFrame = 0;
}


bool DonixFace::begin() {
    clearFace();
    display_.display();
    return true;
}


void DonixFace::clearFace() {
    display_.clearDisplay();
}


void DonixFace::update() {
    unsigned long now = millis();

    /*
     * Automatic blinking
    */

    if ( !_blinkActive && now - _lastBlinkTime > 3500) {
        setBlinking();
    }


    /*
     * Blink animation
    */

    if (_blinkActive) {

    unsigned long elapsed = now - _blinkStart;

    if (elapsed < 60) {
        _blinkFrame = 1;
    }
    else if (elapsed < 120) {
        _blinkFrame = 2;
    }
    else if (elapsed < 180) {
        _blinkFrame = 1;
    }
    else {
        _blinkActive = false;
        _blinkFrame = 0;
        _lastBlinkTime = now;
    }

    if (_blinkActive) {
        display_.clearDisplay();

        drawBlink();

        display_.display();

        return;
    }
}


    /*
     * Talking animation
    */

    if (_talking) {

        if (now - _lastTalkTime > 90) {

            _talkFrame++;

            if (_talkFrame > 3)
                _talkFrame = 0;

            _lastTalkTime = now;
        }
    }


    drawFace();

    display_.display();
}


void DonixFace::drawFace() {
    display_.clearDisplay();
    drawEyes();
    drawEyeBrows();
    drawMouth();
}


/*
 * EXPRESSIONS
*/

void DonixFace::setExpression(DonixExpression expression){
    currentExpression_ = expression;
}


void DonixFace::setNormal() {
    currentExpression_ = DONIX_NORMAL;
}


void DonixFace::setHappy() {
    currentExpression_ = DONIX_HAPPY;
}


void DonixFace::setSad() {
    currentExpression_ = DONIX_SAD;
}


void DonixFace::setAngry() {
    currentExpression_ = DONIX_ANGRY;
}


void DonixFace::setSurprised() {
    currentExpression_ = DONIX_SURPRISED;
}


void DonixFace::setSleepy() {
    currentExpression_ = DONIX_SLEEPY;
}


void DonixFace::setThinking() {
    currentExpression_ = DONIX_THINKING;
    currentState_ = DONIX_THINKING_STATE;
}


void DonixFace::setListen()
{
    currentState_ = DONIX_LISTENING;
}


void DonixFace::setState(DonixState state) {
    currentState_ = state;
}


void DonixFace::setBlinking() {
    _blinkActive = true;
    _blinkStart = millis();
}


void DonixFace::setTalk(){
    _talking = true;
    currentState_ = DONIX_SPEAKING;
}


void DonixFace::setStopTalking() {
    _talking = false;
    currentState_ = DONIX_IDLE;
}


/*
 * EYES
*/

void DonixFace::drawEyes() {
    switch (currentExpression_) {

        case DONIX_HAPPY:
            drawHappyEyes();
            break;

        case DONIX_SAD:
            drawSadEyes();
            break;

        case DONIX_ANGRY:
            drawAngryEyes();
            break;

        case DONIX_SURPRISED:
            drawSurprisedEyes();
            break;

        case DONIX_SLEEPY:
            drawSleepyEyes();
            break;

        case DONIX_THINKING:
            drawThinkingEyes();
            break;
        

        default:
            drawNormalEyes();
            break;
    }
}


void DonixFace::drawNormalEyes() {
    /*
     * Left eye
    */

    display_.fillRoundRect(
        18,
        17,
        30,
        25,
        8,
        SH110X_WHITE
    );

    /*
     * Right eye
    */

    display_.fillRoundRect(
        80,
        17,
        30,
        25,
        8,
        SH110X_WHITE
    );


    /*
     * pupils
    */

    display_.fillCircle(
        33,
        29,
        6,
        SH110X_BLACK
    );

    display_.fillCircle(
        95,
        29,
        6,
        SH110X_BLACK
    );
}


void DonixFace::drawHappyEyes() {
    /*
     * Happy eyes
    */

    display_.drawLine(
        18, 31,
        26, 23,
        SH110X_WHITE
    );

    display_.drawLine(
        26, 23,
        34, 31,
        SH110X_WHITE
    );

    display_.drawLine(
        94, 31,
        102, 23,
        SH110X_WHITE
    );

    display_.drawLine(
        102, 23,
        110, 31,
        SH110X_WHITE
    );
}


void DonixFace::drawSadEyes() {
    display_.fillRoundRect(
        18, 18,
        30, 24,
        7,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        80, 18,
        30, 24,
        7,
        SH110X_WHITE
    );

    display_.fillCircle(
        33, 34,
        5,
        SH110X_BLACK
    );

    display_.fillCircle(
        95, 34,
        5,
        SH110X_BLACK
    );
}


void DonixFace::drawAngryEyes() {
    display_.fillRoundRect(
        18, 20,
        30, 22,
        5,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        80, 20,
        30, 22,
        5,
        SH110X_WHITE
    );

    display_.fillCircle(
        33, 28,
        5,
        SH110X_BLACK
    );

    display_.fillCircle(
        95, 28,
        5,
        SH110X_BLACK
    );
}


void DonixFace::drawSurprisedEyes() {
    display_.fillCircle(
        33,
        29,
        15,
        SH110X_WHITE
    );

    display_.fillCircle(
        95,
        29,
        15,
        SH110X_WHITE
    );

    display_.fillCircle(
        33,
        29,
        6,
        SH110X_BLACK
    );

    display_.fillCircle(
        95,
        29,
        6,
        SH110X_BLACK
    );
}


void DonixFace::drawSleepyEyes() {
    display_.fillRoundRect(
        18,
        24,
        30,
        12,
        6,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        80,
        24,
        30,
        12,
        6,
        SH110X_WHITE
    );
}

void DonixFace::drawThinkingEyes() {

    display_.fillRoundRect(
        18, 18,
        30, 24,
        7,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        80, 18,
        30, 24,
        7,
        SH110X_WHITE
    );

    display_.fillCircle(
        39, 27,
        5,
        SH110X_BLACK
    );

    display_.fillCircle(
        101, 27,
        5,
        SH110X_BLACK
    );
}

/*
 * EYEBROWS
*/

void DonixFace::drawEyeBrows() {
    switch (currentExpression_) {

        case DONIX_HAPPY:
            drawHappyBrows();
            break;

        case DONIX_SAD:
            drawSadBrows();
            break;

        case DONIX_ANGRY:
            drawAngryBrows();
            break;

        case DONIX_SURPRISED:
            drawSurprisedBrows();
            break;

        case DONIX_SLEEPY:
            drawSleepyBrows();
            break;

        case DONIX_THINKING:
            drawThinkingBrows();
            break;

        default:
            drawNormalBrows();
            break;
    }
}


void DonixFace::drawNormalBrows() {
    display_.drawLine(
        20, 12,
        45, 12,
        SH110X_WHITE
    );

    display_.drawLine(
        83, 12,
        108, 12,
        SH110X_WHITE
    );
}


void DonixFace::drawHappyBrows() {
    display_.drawLine(
        20, 14,
        44, 10,
        SH110X_WHITE
    );

    display_.drawLine(
        84, 10,
        108, 14,
        SH110X_WHITE
    );
}


void DonixFace::drawSadBrows() {
    display_.drawLine(
        20, 10,
        44, 14,
        SH110X_WHITE
    );

    display_.drawLine(
        84, 14,
        108, 10,
        SH110X_WHITE
    );
}


void DonixFace::drawAngryBrows() {
    display_.drawLine(
        20, 8,
        45, 17,
        SH110X_WHITE
    );

    display_.drawLine(
        83, 17,
        108, 8,
        SH110X_WHITE
    );
}


void DonixFace::drawSurprisedBrows() {
    display_.drawLine(
        20, 7,
        45, 7,
        SH110X_WHITE
    );

    display_.drawLine(
        83, 7,
        108, 7,
        SH110X_WHITE
    );
}


void DonixFace::drawSleepyBrows() {
    display_.drawLine(
        20, 17,
        45, 17,
        SH110X_WHITE
    );

    display_.drawLine(
        83, 17,
        108, 17,
        SH110X_WHITE
    );
}


void DonixFace::drawThinkingBrows() {
    display_.drawLine(
        20, 10,
        45, 12,
        SH110X_WHITE
    );

    display_.drawLine(
        83, 12,
        108, 10,
        SH110X_WHITE
    );
}


/*
 * MOUTH
*/

void DonixFace::drawMouth() {

    if (_talking) {
        drawTalkingMouth();
        return;
    }


    switch (currentExpression_) {

        case DONIX_HAPPY:
            drawSmile();
            break;

        case DONIX_SAD:
            drawSadMouth();
            break;

        case DONIX_SURPRISED:
            drawWideMouth();
            break;

        case DONIX_THINKING:
            drawThinkingMouth();
            break;

        case DONIX_SLEEPY:
            drawClosedMouth();
            break;

        default:
            drawClosedMouth();
            break;
    }
}


void DonixFace::drawClosedMouth() {
    display_.drawRoundRect(
        51,
        51,
        26,
        5,
        2,
        SH110X_WHITE
    );
}


void DonixFace::drawSmile() {
    display_.drawLine(
        45, 50,
        51, 56,
        SH110X_WHITE
    );

    display_.drawLine(
        51, 56,
        64, 59,
        SH110X_WHITE
    );

    display_.drawLine(
        64, 59,
        77, 56,
        SH110X_WHITE
    );

    display_.drawLine(
        77, 56,
        83, 50,
        SH110X_WHITE
    );
}


void DonixFace::drawSadMouth() {
    display_.drawLine(
        45, 57,
        51, 52,
        SH110X_WHITE
    );

    display_.drawLine(
        51, 52,
        64, 49,
        SH110X_WHITE
    );

    display_.drawLine(
        64, 49,
        77, 52,
        SH110X_WHITE
    );

    display_.drawLine(
        77, 52,
        83, 57,
        SH110X_WHITE
    );
}


void DonixFace::drawOpenMouth() {
    display_.fillRoundRect(
        48,
        48,
        32,
        14,
        5,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        53,
        51,
        22,
        8,
        3,
        SH110X_BLACK
    );
}


void DonixFace::drawWideMouth() {
    display_.fillRoundRect(
        43,
        45,
        42,
        18,
        7,
        SH110X_WHITE
    );

    display_.fillRoundRect(
        49,
        49,
        30,
        11,
        5,
        SH110X_BLACK
    );
}


void DonixFace::drawThinkingMouth() {
    display_.drawCircle(
        64,
        54,
        4,
        SH110X_WHITE
    );
}


void DonixFace::drawTalkingMouth() {
    switch (_talkFrame) {

        case 0:

            drawClosedMouth();

            break;


        case 1:

            drawOpenMouth();

            break;


        case 2:

            display_.fillRoundRect(
                49,
                49,
                30,
                10,
                4,
                SH110X_WHITE
            );

            display_.fillRect(
                54,
                52,
                20,
                4,
                SH110X_BLACK
            );

            break;


        case 3:

            drawWideMouth();

            break;
    }
}


/*
 * BLINK
*/

void DonixFace::drawBlink() {

    switch (_blinkFrame) {

        // Partially closed
        case 1:

            display_.fillRoundRect(
                18, 25,
                30, 10,
                5,
                SH110X_WHITE
            );

            display_.fillRoundRect(
                80, 25,
                30, 10,
                5,
                SH110X_WHITE
            );

            break;


        // Fully closed
        case 2:

            display_.drawLine(
                18, 29,
                48, 29,
                SH110X_WHITE
            );

            display_.drawLine(
                80, 29,
                110, 29,
                SH110X_WHITE
            );

            break;
    }

    // Keep eyebrows visible
    drawEyeBrows();

    // Keep mouth visible
    drawMouth();
}