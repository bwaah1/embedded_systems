#include "max7219.h"
#include <avr/pgmspace.h>

const char message[] PROGMEM = "Ura URA ura ";

int main(void)
{
    initSPI();
    initMatrix();
    clearMatrix();
    initBuffer();

    const char *messagePointer = &message[0];

    uint16_t messageSize = sizeof(message);

    while (1)
    {
        displayMessage(messagePointer, messageSize);
    }
    return (0);
}