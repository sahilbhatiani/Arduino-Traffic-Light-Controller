#include <avr/io.h>
#include <avr/interrupt.h>

/*Count variable is used to keep track of which LED needs to be 
  lit next. 1 = "RED" ,  2 = "GREEN" , "3 = YELLOW" */
volatile uint8_t count = 1;

void setup()
{
  cli(); // All interrupts are disabled globally.
  
  //IO pins setup. Pins 13,12 and 11 are set as OUTPUT pins.
  DDRB = (1<<DDB5)|(1<<DDB4)|(1<<DDB3);
  
  //All the above pins are set to LOW.
  PORTB = (0<<PORTB5)|(0<<PORTB4)|(0<<PORTB3);
  
  //Timer/counter 1 is set to normal mode.
  TCCR1A = 0x00;
  TCCR1B = (1<<CS12) | (1<<CS10); //Prescaler: 1024
  TIMSK1 |= (1<<TOIE1); //Overflow Interrupt is enables
  TCNT1 = 49910; 
  /*TCNT1 is set to this value so that the overflow interrupt 
    service routine is called in 1 second. */
  
  sei(); // All interrupts are disabled globally.
  
 
}

void loop()
{
  
 
}
ISR(TIMER1_OVF_vect){
  if(count == 1){
  	PORTB = (1<<PORTB5); //Only Red LED is lit
    count++; //Count variable set to 2(->Lit "GREEN" LED next)
  }
  else if(count == 2){
  	PORTB = (1<<PORTB3); //Only Green LED is lit
    count++; //Count variable set to 3(->Lit "YELLOW" LED next)
  }
  
  else if(count == 3){
  	PORTB = (1<<PORTB4); //Only Yellow LED is lit.
    count = 1; // Count variable set to 1(->Lit "RED" LED next)
  } 
  TCNT1 = 49910; 
  /* Timer/Counter 1 set to this value again so that overflow 
  interrupt service routine continues to be called every 1 second.*/
  
}