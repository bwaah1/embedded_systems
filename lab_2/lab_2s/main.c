#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

uint8_t delayCounter = 0, displayData = 1, effectIndex = 0;
uint16_t longPressTime = 0;
uint8_t displayValue = 0x00;
uint8_t displayMask = 0x01;
bool ascendingOrder = true;
bool isButtonPressed = false;
bool isEffectStopped = false;

//-----------------------------------------------------------Effect communication
void changeEffect(uint8_t keyPressed) {
    if (keyPressed == 0) {
        effectIndex++;
        if (effectIndex > 4) {
            effectIndex = 0;
        }
        delayCounter = 0;
        displayData = 0;
        displayValue = 0x00;
        if (effectIndex == 3) {
            displayValue = 0x0F;
        }
        ascendingOrder = true;
    }
}

void stopEffect(uint8_t keyPressed) {
    if (keyPressed == 0) {
        isEffectStopped = !isEffectStopped;
    }
}

//-----------------------------------------------------------Effects
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
        if (ascendingOrder) {
            delayCounter = 0;
            PORTD = displayValue;
            displayValue |= displayMask;
            displayMask <<= 1;
        } else {
            delayCounter = 0;
            PORTD = displayValue;
            displayValue &= displayMask;
            displayMask = (displayMask << 1) | 0x01;
        }
        if (displayValue == 0xFF) {
            ascendingOrder = false;
        } else if (displayValue == 0x00) {
            displayMask = 0x01;
            ascendingOrder = true;
        }
    }
}

void johnsonCounter(void) {
    if (++delayCounter > 20) {
        if (ascendingOrder) {
            delayCounter = 0;
            PORTD = displayValue;
            displayValue |= displayMask;
            displayMask <<= 1;
        } else {
            delayCounter = 0;
            PORTD = displayValue;
            displayValue &= ~displayMask;
            displayMask = (displayMask << 1) | 0x01;
        }
        if (displayValue == 0xFF) {
            ascendingOrder = false;
        } else if (displayValue == 0x00) {
            displayMask = 0x01;
            ascendingOrder = true;
        }
    }
}

void goByTwo(void) {
    if (++delayCounter > 20) {
        delayCounter = 0;
        PORTD = displayValue;
        displayValue = ((displayValue << 1) & 0xFE) | ((displayValue >> 7) & 0x01);
    }
}

//-----------------------------------------------------------Button press
void scanKey(void) {
    static uint8_t shiftRegister;
    shiftRegister <<= 1;
    if ((PINB & (1 << PB1)) != 0) {
        shiftRegister |= 1;
    }
    if ((shiftRegister & 0x07) == 0x04) {
        isButtonPressed = true;
    }
    if ((shiftRegister & 0x0F) == 0x03) {
        isButtonPressed = false;
        if (longPressTime > 1600) {
            if (isEffectStopped) {
                changeEffect(0);
                stopEffect(0);
            }
        } else {
            stopEffect(0);
        }
        longPressTime = 0;
    }
}

//-----------------------------------------------------------Main
int main(void) {
    UCSR0B = 0;
    DDRD = 0xFF;
    DDRB = 0;

    for (;;) {
        if (!isEffectStopped) {
            switch (effectIndex) {
                case 0:
                    runningFire();
                    break;
                case 1:
                    runningFireReverse();
                    break;
                case 2:
                    johnsonCounter();
                    break;
                case 3:
                    goByTwo();
                    break;
                case 4:
                    fillingLine();
                    break;
                default:
                    break;
            }
        }

        scanKey();
        if (isButtonPressed) {
            longPressTime += 10;
        }
        _delay_ms(10);
    }
    return 0;
}
