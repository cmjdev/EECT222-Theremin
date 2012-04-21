#include <hidef.h>
#include <mc9s12dp256.h>
#include <stdio.h>
#include "222defs.h"


#define volSense ATD0DR2H
#define pitchSense ATD0DR3H

void LCDinit(void);
void delayu(unsigned int usec);
void LCDcmd(char c, char DATA);
void printString(char *c);

void delaym(int x){

  while(--x) {
  delayu(1000);
  }
}

/* ATDinit initializes onboard ADC chips */
void ATDinit(void) {
  
  ATD0CTL2 |= 0x80;
  delayu(10);

  ATD0CTL4 |= 0x80;
  ATD0CTL5 |= 0x30;

}

void RTIinit(void){
  RTICTL = 0x10;
  CRGINT = 0x80;
}

/* init configures LED's for vumeter and initializes LCD / ATD / PWM / RTI*/
void init(void) {
  
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRJ = 0x02;
  PTJ = 0x00;
  DDRT = 0x20;
  
  DDRP = 0xFF;
  PTP = 0xFF;
  
  PWMPRCLK = 0x03;   // 4MHz / 8 = 500kHz
  PWMSCLA = 0x0A;    // 500kHz / (2*10) =25kHz
  PWMCLK = 0x20;     // PWM source channel 5
  PWMPOL = 0x20;     // High then low
  PWMCAE = 0x00;     // Left aligned
  PWMCTL = 0x0F;     // 8-bit, 15 cycle wait
  PWME = 0x20;       // Enable PWM5
  //PWMCNT5 = 0x00;    //Clear PWMCNT05
  
  ATDinit();
  LCDinit();
  RTIinit();
}
  
void welcome() {
  
  printString("   Cool Kids   ");
  LCDcmd(0xC0,0);
  printString("ECT122 Theremin");
  delaym(2000);
}

/* vupdate scales input and displays the result in a 'vumeter' manor */
void vupdate(unsigned char x) {

  const unsigned char lookup[] = {0, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
  
  PORTB = lookup[x/32]; // change when scale is figured    
}

/* display formats and prints data to LCD */
void display(unsigned char vol, unsigned char freq) {

  static char buffer[31];

  if(PTH & 0x02) {
  
    LCDcmd(0x01,0);                                                           
    printString("   Debug Mode  ");  // implement conversion to freq
    (void)sprintf(buffer,"L1=%d    L2=%d", freq, vol);  // duty cycle -> volume %
    LCDcmd(0xC0,0);
    printString(buffer);
    
  } else {
    
    (void)sprintf(buffer,"Frequency..:%d", 25000/pitchSense);  // implement conversion to freq
    LCDcmd(0x01,0);
    printString(buffer);
    (void)sprintf(buffer,"Volume.....:%d",100 * vol / 255);  // duty cycle -> volume %
    LCDcmd(0xC0,0);
    printString(buffer);
  }
  
    vupdate(volSense);
}

/* RTI updates the PWM autonomously */
interrupt void RTI_ISR(void){

  unsigned char y = PTH>>2;
  static int x;

  PWMDTY5 = (PTH & 0x01) ? ((PWMPER5/2)/(25500/(volSense * 100))) : 0x00;
  if(!x) {
    PWMPER5 = pitchSense; 
    x=(y*25)+1;      
    CRGFLG = 0x80;
  }
  x--;
  CRGFLG = 0x80;
}

/* catches uninteded interrupts and re-enables */
interrupt void UnimplementedISR(void) {

  CRGFLG = 0x80;  
}

/* links interrupt flags to function names */
typedef void (*near tIsrFunc)(void);   //tIsrFunc is a pointer to a function
const tIsrFunc _vect[] @0x3E70 = {     //locate table in RAM at 0x3E70
  
  RTI_ISR,
  UnimplementedISR,
  UnimplementedISR,
  UnimplementedISR,
  UnimplementedISR,
  UnimplementedISR,
  UnimplementedISR,
  UnimplementedISR
};

void main(void) {

  /* Setup code */
  int count = 1500;  
  init(); 
  welcome(); 
  int_enable(); 
  
  /* Main loop code */  
  for(;;) {

    if(!(count--)) {
      display(volSense, pitchSense);
      count = 1500;
    } 
  }
}