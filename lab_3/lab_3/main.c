#include "main.h"


#define NUM_DEVICES     2
#define DEL     80000


uint8_t buffer [NUM_DEVICES*8];    


void initSPI(void) 
{
  DDRB |= (1 << PB2);
  PORTB |= (1 << PB2);

  DDRB |= (1 << PB3);
  DDRB |= (1 << PB5);

  SPCR |= (1 << MSTR);
  SPCR |= (1 << SPE);
}


// Send byte through SPI
void writeByte(uint8_t byte)
{
  SPDR = byte;
  while(!(SPSR & (1 << SPIF)));
}


// Sends word through SPI
void writeWord(uint8_t address, uint8_t data) 
{
  writeByte(address);
  writeByte(data);
}


void initMatrix() 
{
  uint8_t i;

  // display brighness
  SLAVE_SELECT;
  for(i = 0; i < NUM_DEVICES; i++)
  {
    writeByte(0x0A);
    writeByte(0x01); //brightness
  }
  SLAVE_DESELECT;

  
  SLAVE_SELECT;
  for(i = 0; i < NUM_DEVICES; i++)
  {
    writeByte(0x0B);
    writeByte(0x07);
  }
  SLAVE_DESELECT;


  SLAVE_SELECT;
  for(i = 0; i < NUM_DEVICES; i++)
  {
    writeByte(0x0C);
    writeByte(0x01);
  }
  SLAVE_DESELECT;


  SLAVE_SELECT;
  for(i = 0; i < NUM_DEVICES; i++)
  {
    writeByte(0x0F);
    writeByte(0x00);
  }
  SLAVE_DESELECT;
}


void clearMatrix(void)
{
  for(uint8_t x = 1; x < 9; x++)
  {   
        SLAVE_SELECT;
        for(uint8_t i = 0; i < NUM_DEVICES; i++)
    {
      writeByte(x);
      writeByte(0x00);
    }
    SLAVE_DESELECT;
  }
}


void initBuffer(void)
{   
  for(uint8_t i = 0; i < NUM_DEVICES *  8; i++)
    buffer[i] = 0x00;
}       


void pushBuffer(uint8_t x)
{
  for(uint8_t i = 0; i < NUM_DEVICES * 8 - 1; i++)
    buffer[i] = buffer[i + 1];
  
  buffer[NUM_DEVICES*8 - 1] = x;
}

void pushCharacter(uint8_t c)
{
    for(uint8_t i = 0; i < 5; i++)
    {
      pushBuffer(pgm_read_byte(&characters[c][i]));
      displayBuffer();
      _delay_us(DEL);
    }
}

void displayMessage(const char *arrayPointer, uint16_t arraySize)
{
  for(uint16_t i = 0; i < arraySize; i++)
  {
    pushCharacter(pgm_read_byte_near(arrayPointer + i) - 32);
    pushBuffer(0x00);
    displayBuffer();
    _delay_us(DEL);
  }
  
}


// Displays current buffer on the cascaded devices
void displayBuffer()
{   
   for(uint8_t i = 0; i < NUM_DEVICES; i++)
   {
     for(uint8_t j = 1; j < 9; j++)
     {
       SLAVE_SELECT;
       
       for(uint8_t k = 0; k < i; k++)
         writeWord(0x00, 0x00);
       
       writeWord(j, buffer[j + i*8 - 1]);
       
       for(uint8_t k = NUM_DEVICES-1; k > i; k--)
         writeWord(0x00, 0x00);
       
       SLAVE_DESELECT;
     }
   }
}

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