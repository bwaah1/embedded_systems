#include <avr/io.h>
#include <util/delay.h>

/*
Board digital I/O pin to atmega328 registers for LEDS
| d2  | d3  | d4  | d5  | d6  | d7  | d8  | d9  |
| pd2 | pd3 | pd4 | pd5 | pd6 | pd7 | pb0 | pb1 |

   B1  |  B2  |  B3  
| d10  | d11  | d12  |
| pb2  | pb3  | pb4  |
*/

void initializePorts()
{
    DDRD = 0b11111100;
    PORTD = 0b00000000;

    DDRB = 0b00000011;
    PORTB = 0b00000000;
}

void incrementCount(uint8_t *count)
{
    if (*count < 63) {
        PORTD = PORTD + 0b00000100;
        (*count)++;
    }
    else if (*count == 63) {
        *count = 0;
        PORTD = 0b00000000;
        PORTB = PORTB + 0b00000001;
    }
}

void decrementCount(uint8_t *count)
{
    if (*count > 0) {
        PORTD = PORTD - 0b00000100;
        (*count)--;
    } else if (*count == 0) {
        *count = 63;
        PORTD = 0b11111100;
        PORTB = PORTB - 0b00000001;
    }
}

void clearCounter(uint8_t *count)
{
    *count = 0;
    PORTD &= ~0b11111100;
    PORTB &= ~0b00000011;
}

int main()
{
    uint8_t x = 0;

    initializePorts();

    for (;;) {
        if ((PINB & 0b00000100) == 0) {
            incrementCount(&x);
            _delay_ms(200);
        }
        else if ((PINB & 0b00001000) == 0) {
            clearCounter(&x);
        }
        else if ((PINB & 0b00010000) == 0) {
            decrementCount(&x);
            _delay_ms(200);
        }
    }

    return 0;
}