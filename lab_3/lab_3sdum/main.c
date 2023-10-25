#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

//*((volatile uint8_t *)(0x05) + 0x20) = 54;
uint8_t dl = 0, state = 1;
uint8_t data = 0;
uint8_t mbuf[64];

const uint8_t font[] = {
    0xC6,  /*  %%...%%.  */
    0xEE,  /*  %%%.%%%.  */
    0xFE,  /*  %%%%%%%.  */
    0xD6,  /*  %%.%.%%.  */
    0xC6,  /*  %%...%%.  */
    0xC6,  /*  %%...%%.  */
    0xC6,  /*  %%...%%.  */
    0x00,  /*  ........  */

    0x7C,  /*  .%%%%%..  */
    0xC6,  /*  %%...%%.  */
    0xC6,  /*  %%...%%.  */
    0xFE,  /*  %%%%%%%.  */
    0xC6,  /*  %%...%%.  */
    0xC6,  /*  %%...%%.  */
    0xC6,  /*  %%...%%.  */
    0x00,   /*  ........  */

    0xC6,  /*  %%...%%.  */
    0x6C,  /*  .%%.%%..  */
    0x38,  /*  ..%%%...  */
    0x38,  /*  ..%%%...  */
    0x38,  /*  ..%%%...  */
    0x6C,  /*  .%%.%%..  */
    0xC6,  /*  %%...%%.  */
    0x00,  /*  ........  */

0x7E,
0x60,
0x30,
0x18,
0x0C,
0x06,
0x06,
0x00,
0x3E,
0x63,
0x60,
0x30,
0x0C,
0x06,
0x7F,
0x00,
0x18,
0x1C,
0x1E,
0x18,
0x18,
0x18,
0x3C,
0x00,
0x3E,
0x63,
0x63,
0x7E,
0x60,
0x63,
0x3E,
0x00,

    // 0xFE,  /*  %%%%%%%.  */
    // 0x06,  /*  .....%%.  */
    // 0x0C,  /*  ....%%..  */
    // 0x18,  /*  ...%%...  */
    // 0x30,  /*  ..%%....  */
    // 0x60,  /*  .%%.....  */
    // 0x60,  /*  .%%.....  */
    // 0x00,  /*  ........  */

    // 0x7C,  /*  .%%%%%..  */
    // 0xC6,  /*  %%...%%.  */
    // 0x06,  /*  .....%%.  */
    // 0x0C,  /*  ....%%..  */
    // 0x30,  /*  ..%%....  */
    // 0x60,  /*  .%%.....  */
    // 0xFE,  /*  %%%%%%%.  */
    // 0x00,  /*  ........  */

    // 0x18,  /*  ...%%...  */
    // 0x38,  /*  ..%%%...  */
    // 0x78,  /*  .%%%%...  */
    // 0x18,  /*  ...%%...  */
    // 0x18,  /*  ...%%...  */
    // 0x18,  /*  ...%%...  */
    // 0x3C,  /*  ..%%%%..  */
    // 0x00,  /*  ........  */

    // 0x7C,  /*  .%%%%%..  */
    // 0xC6,  /*  %%...%%.  */
    // 0xC6,  /*  %%...%%.  */
    // 0x7E,  /*  .%%%%%%.  */
    // 0x06,  /*  .....%%.  */
    // 0xC6,  /*  %%...%%.  */
    // 0x7C,  /*  .%%%%%..  */
    // 0x00,  /*  ........  */

    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00,  /*  ........  */
    0x00   /*  ........  */
};

const uint8_t disp_cnt = 2;

uint8_t SPI_Transfer(uint8_t cData);

//-----------------------------------------------------------------------------
void SPI_MasterInit(void)
{
  // Налаштувати піни MOSI (PB3) і SCK (PB5) на вивід
  DDRB |= (1<<PB3) | (1<<PB5) | (1 << PB2);
  // Включити SPI, задати режим Master, 
  // встановити швидкість fck/16
  SPCR = (1<<SPE) | (1<<MSTR) | (0<<SPR0);
}

//-----------------------------------------------------------------------------
uint8_t SPI_Transfer(uint8_t cData)
{
  // Початок передачі
  SPDR = cData;
  // Чекати завершення передачі
  while(!(SPSR & (1<<SPIF)));
  return SPDR;
}

//-----------------------------------------------------------------------------
void max7219_sendData(uint8_t addr, uint8_t data) 
{
  // CS = 0 
  //PORTB &= ~(1 << PB2);
  // Почати передачу адресу
  SPDR = addr;
  // Чекати завершення передачі
  while(!(SPSR & (1<<SPIF)));
  // Почати передачу даних 
  SPDR = data;
  // Чекати завершення передачі
  while(!(SPSR & (1<<SPIF)));
  // CS = 1
  //PORTB |= (1 << PB2);
}

//-----------------------------------------------------------------------------
void max7219_Init(void)
{
  const uint16_t init_seq[] = {
      0x0F00,  // normal mode
      0x0900,  // no decode
      0x0B07,  
      0x0A0B,  // intensity
      0x0C01   // power on
  };

  SPI_MasterInit();
  PORTB |= (1 << PB2);

for (uint8_t i = 0; i < 5; i++) {
    PORTB &= ~(1 << PB2);  // CS = 0 
    for (uint8_t j = 0; j < disp_cnt; j++) {
      max7219_sendData(init_seq[i] >> 8, init_seq[i] & 0xFF);
    }
    PORTB |= (1 << PB2);  // CS = 1
  }
   
  // очистка дисплея
  for (uint8_t i = 0; i < 8; i++) { 
    PORTB &= ~(1 << PB2);  // CS = 0 
    for (uint8_t j = 0; j < disp_cnt; j++) {
      max7219_sendData(i + 1, 0);
    }
    PORTB |= (1 << PB2);  // CS = 1
  }
}

//-----------------------------------------------------------------------------
void max7219_setPixel(uint8_t x, uint8_t y, uint8_t p)
{
  x &= 15;
  y &= 7;
  uint8_t xx = x >> 3; // x / 8

  if (p == 0) {
    mbuf[y + xx * 8] &= ~(1 << (x & 7)); // clear
  } else {
    mbuf[y + xx * 8] |= 1 << (x & 7); //set
  }

  for (uint8_t i = 0; i < 8; i++) {  
    PORTB &= ~(1 << PB2);  // CS = 0 
    //for (uint8_t j = 1; j >= 0; --j) {
    //  max7219_sendData(i + 1, mbuf[j * 8 + i]);
    // }
    max7219_sendData(1 + i, mbuf[8 + i]);
    max7219_sendData(1 + i, mbuf[i]);

    PORTB |= (1 << PB2);  // CS = 1
  }
}

//-----------------------------------------------------------------------------
void max7219_update(void)
{
  for(uint8_t i = 0; i < 8; i++) {
    PORTB &= ~(1 << PB2);  // CS = 0 
    max7219_sendData(1 + i, mbuf[8 + i]);
    max7219_sendData(1 + i, mbuf[i]);
    PORTB |= (1 << PB2);  // CS = 1
  }
}

void max7219_shift(void)
{
  uint8_t i, j, a;
  
  for (i = 0; i < 8; i++) {
    a = mbuf[i];
    for (j = 0; j < 7; j++) {
      mbuf[i + j * 8] >>= 1;
      if (mbuf[i + (j+1) * 8] & 1) {
        mbuf[i + j * 8] |= 128;
      }
    }
    mbuf[i + (j) * 8] >>= 1;
    if (a & 1) {
      mbuf[i + (j) * 8] |= 128; 
    } 
  }
}

//-----------------------------------------------------------------------------
void key_handler(uint8_t st)
{
  switch(st) {
    case 1: //short press
   
    break;
    case 2: //long press
     ;
  }
}

//-----------------------------------------------------------------------------
void scan_key(void)
{
  static uint8_t shreg = 0xFF;
  static uint8_t lcnt, dbc_cnt;

  shreg <<= 1;
  if((PINB & (1<<PB1)) != 0) {
    shreg |= 1;
  }
  if((shreg & 0x07) == 0x04) {  //11111 100 00000
    key_handler(0); //short press down "100"
   // LOG_DEBUG("MAIN", "Short press down");
    lcnt=0;
  }
  if((shreg & 0x0F) == 0x03) {
    if(dbc_cnt <= 32 && dbc_cnt > 5) {
      //db click
      //LOG_DEBUG("MAIN", "Double-click: dbc_cnt= %d\n", dbc_cnt);
    } else {
      if(lcnt != 101 && dbc_cnt >= 34) {
        //LOG_DEBUG("MAIN", "Short press up");
        key_handler(1); //short press up //0011
      }
    }
    if(dbc_cnt >= 34) {
      dbc_cnt = 0;
    } 
  }
  if(dbc_cnt < 35) {
     dbc_cnt++; //200ms
  } 
  // long press
  if((shreg & 15) == 0) {
    if(lcnt < 100) {
      lcnt++; //200ms
    } else if (lcnt == 100) {
      lcnt++; 
      //LOG_DEBUG("MAIN", "Long press");
      key_handler(2);
    }
  // printf("[D] dbc_cnt= %d\n", dbc_cnt);
  }
}

//-----------------------------------------------------------------------------
float GetTemp(void)
{
  uint8_t wADC;
  float t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<MUX3);
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); 
  _delay_ms(20);  // wait for voltages to become stable.
  
  ADCSRA |= (1<<ADSC);  // Start the ADC
  // Detect end-of-conversion
  while (ADCSRA & (1<<ADSC));
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW; //  (ADCL | (ADCH<<8));
  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;

  // The returned temperature is in degrees Celcius.
  return t;
}

#define U_PIN PB0
void swu_put_char(uint8_t c )
{
  c = ~c;
  //  output_low( PORTB, SND );
  PORTB &= ~(1 << U_PIN);
  for (uint8_t i = 10; i; i-- ) {      // 10 bits
    _delay_us( 1000000 / 9600 );    // bit duration / (8MHz, 9600, 104 us) Delay 832 cycles
    if ( c & 1 ) {
      PORTB &= ~(1 << U_PIN);  //output_low(PORTB, SND); 
    } else {
      PORTB |= (1 << U_PIN); //output_high(PORTB, SND);   // data bit 0 / data bit 1 or stop bit
    }  
    c >>= 1;
  }
} 

//=============================================================================
int main(void)
{
  //DDRD = 0xFF;
 
  max7219_Init();

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      mbuf[j + i * 8] = font[i * 8 + (7-j)];
    }
  }
  max7219_update();

  // mail loop =============
  for (;;) 
  {

    //--------------------------
    // thread2 (100 ms)
    if(++dl > 40) {
      dl = 0;

      max7219_shift();
      max7219_update();
    }

    //------------------------
    // thread1 10 ms
    scan_key();
    _delay_ms(10);
  }

  return 0;
}