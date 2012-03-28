#include <mc9s12dp256.h>
#define ENABLE 0x02

/* delay = 2us *NEED TO OPTIMIZE* */
void delayu(unsigned int usec) {

  while (usec--) {
  }
}

/* send cmd to LCD control reg */
void LCDcmd(char c, char DATA) {  

  char lownib = (0x0f & c) <<2; // shift into bits 2:5 *CLEARS DATASEL REGISTER*
  char hinib = (0xf0 & c) >>2;  // shift into bits 2:5 *CLEARS DATASEL REGISTER*
  
  /* send high nibble of data */
  PORTK = hinib;
  PORTK = ENABLE | DATA | hinib;
  PORTK = hinib;
  
  /* send low nibble of data */
  PORTK = lownib;
  PORTK = ENABLE | DATA | lownib;
  PORTK = lownib;
 
  delayu(20+!DATA*1000);  //wait for command completion
  }
  
/* initialize LCD with startup parameters *MAKE SURE INITLENGTH MATCHES* */
void LCDinit(void) {

  const char INITLENGTH = 6;
  char initseq[] = {0x33,0x32,0x28,0x06,0x0c,0x01};
  char i;
  
  DDRK = 0xFF;
  PORTK = 0x00;
  
  for (i=0; i<INITLENGTH; i++) {
    LCDcmd(initseq[i], 0);
    delayu(5000);
  }
}


/* print string to LCD *NO PROVISIONS FOR WRAP* */
void printString(char *c) {

  while(*c){
    LCDcmd(*(c++),1);
  }
}