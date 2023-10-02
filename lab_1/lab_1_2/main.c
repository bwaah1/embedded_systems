#include <avr/io.h> 
#include <util/delay.h> 

#define LED_PIN PB0
#define DOT_DELAY 200  
#define DASH_DELAY 600 
#define SPACE_DELAY 200 
#define WORD_DELAY 600 
#define VOID_DELAY 1000


void dot()
{
    PORTB &= ~(1 << LED_PIN);
    _delay_ms(DOT_DELAY);   
    PORTB |= (1 << LED_PIN); 
    _delay_ms(SPACE_DELAY);   
}

void dash()
{
    PORTB &= ~(1 << LED_PIN); 
    _delay_ms(DASH_DELAY);   
    PORTB |= (1 << LED_PIN); 
    _delay_ms(SPACE_DELAY);   
}

void blink_message(char *msg)
{
    for (uint8_t i=0; i<strlen(msg); i++) {
        switch (msg[i]) { 
            case '.':
                dot();
                break;
            case '-':
                dash();
                break;
            case ' ':
                _delay_ms(VOID_DELAY);
        }
    }
}

int main(void) 
{
    DDRB |= (1 << LED_PIN);
    while (1) {
        blink_message("...---... "); 
    }
    return 0;
}
