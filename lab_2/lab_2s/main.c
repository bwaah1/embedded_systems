#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

uint8_t delayCounter = 0, displayData = 1, currentEffect = 0;
uint16_t timePressed = 0;
uint8_t hexPattern = 0x00;
uint8_t maskPattern = 0x01;
bool ascending = true;
bool isKeyPressed = false;
bool isEffectStopped = false;

void changeEffect(uint8_t keyPressed) {
    if (keyPressed == 0) {
        currentEffect += 1;
        if (currentEffect > 4) {
            currentEffect = 0;
        }
        delayCounter = 0;
        displayData = 0;
        hexPattern = 0x00;
        if (currentEffect == 3) {
            hexPattern = 0x0F;
        }
        ascending = true;
    }
}

void stopEffect(uint8_t keyPressed) {
    if (keyPressed == 0) {
        if (isEffectStopped == false) {
            isEffectStopped = true;
        } else {
            isEffectStopped = false;
        }
    }
}

void runningFire(void) {
    if (++delayCounter > 20) {
        delayCounter = 0;
        PORTD = displayData;
        displayData <<= 1;

        if (displayData == 0) {
            displayData = 1;
        }
    }
}

void runningFireReverse(void) {
    if (++delayCounter > 20) {
        delayCounter = 0;
        PORTD = displayData;
        displayData >>= 1;

        if (displayData == 0) {
            displayData = 128;
        }
    }
}

void fillingLine(void) {
    if (++delayCounter > 20) {
        if (ascending == true) {
            delayCounter = 0;
            PORTD = hexPattern;
            hexPattern |= maskPattern;
            maskPattern <<= 1;
        } else {
            delayCounter = 0;
            PORTD = hexPattern;
            hexPattern &= maskPattern;
            maskPattern = (maskPattern << 1) | 0x01;
        }
        if (hexPattern == 0xFF) {
            ascending = false;
        } else if (hexPattern == 0x00) {
            maskPattern = 0x01;
            ascending = true;
        }
    }
}

void johnsonCounter(void) {
    if (++delayCounter > 20) {
        if (ascending == true) {
            delayCounter = 0;
            PORTD = hexPattern;
            hexPattern |= maskPattern;
            maskPattern <<= 1;
        } else {
            delayCounter = 0;
            PORTD = hexPattern;
            hexPattern &= ~maskPattern;
            maskPattern = (maskPattern << 1) | 0x01;
        }
        if (hexPattern == 0xFF) {
            ascending = false;
        } else if (hexPattern == 0x00) {
            maskPattern = 0x01;
            ascending = true;
        }
    }
}

void goByTwo(void) {
    if (++delayCounter > 20) {
        delayCounter = 0;
        PORTD = hexPattern;

        hexPattern = ((hexPattern << 1) & 0xFE) | ((hexPattern >> 7) & 0x01);
    }
}

void scanKey(void) {
    static uint8_t shiftRegister;
    shiftRegister <<= 1;
    if ((PINB & (1 << PB1)) != 0) {
        shiftRegister |= 1;
    }
    if ((shiftRegister & 0x07) == 0x04) {
        isKeyPressed = true;
    }
    if ((shiftRegister & 0x0F) == 0x03) {
        isKeyPressed = false;
        if (timePressed > 1600) {
            if (isEffectStopped == true) {
                changeEffect(0);
                stopEffect(0);
            }
        } else {
            stopEffect(0);
        }
        timePressed = 0;
    }
}

int main(void) {
    UCSR0B = 0;
    DDRD = 0xFF;
    DDRB = 0;
    for (;;) {
        if (isEffectStopped == false) {
            if (currentEffect == 0) {
                runningFire();
            }
            if (currentEffect == 1) {
                runningFireReverse();
            }
            if (currentEffect == 2) {
                johnsonCounter();
            }
            if (currentEffect == 3) {
                goByTwo();
            }
            if (currentEffect == 4) {
                fillingLine();
            }
        }

        scanKey();
        if (isKeyPressed == true) {
            timePressed += 10;
        }
        _delay_ms(10);
    }
    return 0;
}
