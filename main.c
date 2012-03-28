#include <hidef.h>
#include <mc9s12dp256.h>
#include <stdio.h>

void LCDinit(void);
void delayu(unsigned int usec);
void LCDcmd(char c, char DATA);
void printString(char *c);

/* ATDinit initializes onboard ADC chips */

void ATDinit(void) {
  
  ATD0CTL2 |= 0x80;
  delayu(10);

  ATD0CTL4 |= 0x80;

  ATD0CTL5 |= 0x30;

}

/* DISPinit configures LED's for vumeter and initializes LCD */

void DISPinit(void) {
  
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRJ = 0x02;
  PTJ = 0x00;
    
  LCDinit();
  
}

/* display formats and prints data to LCD */

void display(unsigned char vol, unsigned char freq) {

  static char buffer[32];

  (void)sprintf(buffer,"Volume = %d",vol);  // duty cycle -> volume %
  LCDcmd(1,0);
  printString(buffer);
  (void)sprintf(buffer,"Temp = %d", freq);  // implement conversion to freq
  LCDcmd(0xC0,0);
  printString(buffer);
}

/* vupdate scales input and displays the result in a 'vumeter' manor */
	
void vupdate(unsigned char x) {

  const unsigned char lookup[] = {0, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
  
  PORTB = lookup[x/28]; // change when scale is figured
    
}
void main(void) {

  /* Setup code */
 
  int count = 25000;
  DISPinit();
  ATDinit();
  
  /* Main loop code */
  
  for(;;) {
    
    if(!(--count)) {
      display(ATD0DR2H, ATD0DR3H);
      count = 25000;
    }
    
    vupdate(ATD0DR2H);
      
  }

}