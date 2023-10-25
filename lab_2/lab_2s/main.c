#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

uint8_t dl = 0, data = 1, effect = 0;
uint16_t time_pressed = 0;
uint8_t hexValue = 0x00;
uint8_t mask = 0x01;
bool asc = true;
bool is_pressed = false;
bool is_stopped = false;

//-----------------------------------------------------------Effect communication
void change_effect(uint8_t ks) {
    if (ks == 0) {
        effect += 1;
        if (effect > 4) {
            effect = 0;
        }
        dl = 0;
        data = 0;
        hexValue = 0x00;
        if (effect == 3) {
            hexValue = 0x0F;
        }
        asc = true;
    }
}

void stop_effect(uint8_t ks) {
    if (ks == 0) {
        if (is_stopped == false) {
            is_stopped = true;
        } else {
            is_stopped = false;
        }
    }
}
//-----------------------------------------------------------Effects

void running_fire(void) {
    if (++dl > 20) {
        dl = 0;
        PORTD = data;
        
        data <<= 1;

        if (data == 0) {
            data = 1;
        }
    }
}

void running_fire_reverse(void) {
    if (++dl > 20) {
        dl = 0;
        PORTD = data;
        data >>= 1;

        if (data == 0) {
            data = 128;
        }
    }
}

void filling_line(void) {
    if (++dl > 20) {
        if (asc == true){
            dl = 0;
            PORTD = hexValue;
            hexValue |= mask;
            mask <<= 1;
        } else {
            dl = 0;
            PORTD = hexValue;
            hexValue &= mask;
            mask = (mask << 1) | 0x01;
        }
        if (hexValue == 0xFF) {
            asc = false;
        } else if (hexValue == 0x00) {
            mask = 0x01;
            asc = true;
        }
    }
}

void johnson_counter(void) {
    if (++dl > 20) {
        if (asc == true){
            dl = 0;
            PORTD = hexValue;
            hexValue |= mask;
            mask <<= 1;
        } else {
            dl = 0;
            PORTD = hexValue;
            hexValue &= ~mask;
            mask = (mask << 1) | 0x01;
        }
        if (hexValue == 0xFF) {
            asc = false;
        } else if (hexValue == 0x00) {
            mask = 0x01;
            asc = true;
        }
    }
}

void go_by_two(void) {
    if (++dl > 20) {
        dl = 0;
        PORTD = hexValue;
        
        hexValue = ((hexValue << 1) & 0xFE) | ((hexValue >> 7) & 0x01);
    }
}
//-----------------------------------------------------------Button press
void scan_key(void) {
    static uint8_t shreg;
    shreg <<= 1;
    if((PINB & (1 << PB1)) != 0) {
        shreg |= 1;
    }
    if((shreg & 0x07) == 0x04) {
        is_pressed = true;
    }
    if((shreg & 0x0F) == 0x03) {
        is_pressed = false;
        if (time_pressed > 1600) {
            if (is_stopped == true) {
                change_effect(0);
                stop_effect(0);
            }
        } else {
            stop_effect(0);
        }
        time_pressed = 0;
    }
}
//-----------------------------------------------------------Main

int main(void) {
    UCSR0B = 0;
    DDRD = 0xFF;
    DDRB = 0;
    for (;;) {
        if (is_stopped == false) {
            if (effect == 0) {
                running_fire();
            }
            if (effect == 1) {
                running_fire_reverse();
            }
            if (effect == 2) {
                johnson_counter();
            }
            if (effect == 3) {
                go_by_two();
            }
            if (effect == 4) {
                filling_line();
            }
        }
        
        scan_key();
        if (is_pressed == true) {
            time_pressed += 10;
        }
        _delay_ms(10);
    }
    return 0;
}
