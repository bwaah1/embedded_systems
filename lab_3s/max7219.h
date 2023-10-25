#ifndef _MAX7219_H
#define _MAX7219_H

#include <avr/io.h>

#define SLAVE_SELECT PORTB &= ~(1 << PB2)
#define SLAVE_DESELECT PORTB |= (1 << PB2)

#define NUM_DEVICES 2
#define DEL 60000

void initSPI(void);

void writeByte(uint8_t byte);

void writeWord(uint8_t address, uint8_t data);

void initMatrix(void);

void clearMatrix(void);

void initBuffer(void);

void pushBuffer(uint8_t x);

void pushCharacter(uint8_t c);

void displayMessage(const char *arrayPointer, uint16_t arraySize);

void displayBuffer(void);

#endif