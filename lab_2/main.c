#include <avr/io.h>
#include <util/delay.h>

void key_handler(uint8_t ks, uint8_t *count);
void incrementCount(uint8_t *count);
void decrementCount(uint8_t *count);
void clearCounter(uint8_t *count);

void initializePorts()
{
    UCSR0B = 0;
    DDRD = 0b11111111;
    PORTD = 0b00000000;
    DDRB = 0;
}

void scan_key(uint8_t pin, uint8_t *count)
{
    static uint8_t shreg[3] = {0}; // Array to hold shift registers for each pin
    shreg[pin] <<= 1;

    if ((PINB & (1 << (PB0 + pin))) != 0) {
        shreg[pin] |= 1;
    }

    if ((shreg[pin] & 0x07) == 0x04) {
        key_handler(pin, count);
    }
}

void key_handler(uint8_t ks, uint8_t *count)
{
    switch(ks) {
        case 0:
            incrementCount(count);
            break;
        case 1:
            decrementCount(count);
            break;
        case 2:
            clearCounter(count);
            break;
    }
}

void incrementCount(uint8_t *count)
{
    PORTD = PORTD + 0b00000001;
    (*count)++;
}

void decrementCount(uint8_t *count)
{
    if (*count > 0) {
        PORTD = PORTD - 0b00000001;
        (*count)--;
    }
    else if (*count == 0) {
        *count = 63;
        PORTD = 0b11111111;
    }
}

void clearCounter(uint8_t *count)
{
    *count = 0;
    PORTD = 0b00000000;
}

int main()
{
    initializePorts();
    uint8_t x = 0;
    for (;;) {
        for (uint8_t i = 0; i < 3; i++) {
            scan_key(i, &x);
            _delay_ms(10);
        }
    }

    return 0;
}