#include <avr/interrupt.h>

volatile float N; // This is the duty cycle in decimals
volatile float speed = 0;
volatile float difference;
volatile int count = 0;

// This ISR sets pin 9 to LOW of N!=1
ISR(TIMER1_COMPA_vect) { 
  if(N != 1){PORTB = (0<<PB1);}
  OCR1A = int(N*(65535-3035)); 
  //OCR1A is set to a value that corrosponds to the duty cycle
   
}
//This ISR sets pin 9 to high if N !=0 
ISR(TIMER1_COMPB_vect) { 
  if(N !=0){PORTB = (1<<PB1);}
  OCR1A = int(N*65535-3035);  
  /*OCR1A is set to a value that corrosponds to the duty cycle
  after which the CompA interrupt will be called and set the
  pin 9 to LOW.*/
}

void setup()
{

  Serial.begin(9600);
  cli();
  
  //Setting i/o pins
  DDRB = (1<<DDB1);
  DDRC = (1<<DDC3)|(1<<DDC4);
      
  //Setting intial state of pins
  PORTB = (0<<PB1);
  PORTC = (0<<PC3)|(0<<PC4);
  
  //Buttons setup
  EIMSK |= (1<<INT0)|(1<<INT1); // enable external int. on PORTD2  
  EICRA |= (1<<ISC01) | (1<<ISC00)|(1<<ISC11)|(1<<ISC10); // sense a rising edge

  // Initialize counter and control register(s).
  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;

  // set mode = normal, prescaler = 256
  OCR1A = 2000; //This initial value is just random.
  OCR1B = 0xFFFF;
  TCCR1B |= (1<<CS12);   // prescale = 256 => period = 1.05s
  TIMSK1 |= (1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1); 
  // enable compare A and B and overflow interrupt.

  sei();
 
}

void loop()
{
 asm volatile("nop");


}

//Called when button 1 is pressed
ISR(INT0_vect) { 
  TCNT1 = 3035; //This is done to get a period of exactly 1s
  count = 0;
}


//Called when button 2 is pressed
ISR(INT1_vect) {
  /*Below is the count difference between when button 1 was 
  pressed and when button 2 gets pressed.*/
  difference = TCNT1 + count*(65535-3035) -3035; 
  //Below is the calculted speed in km/hr
  speed = int(20*(65535-3035)*3.6/(difference)); 
  Serial.print("Speed: " + String(speed));
  Serial.println();
  N = speed/100.0; //This is the duty cycle in decimal
  Serial.print("N: " + String(N));
  Serial.println();
  if(N>1){N = 1;} //If speed > 100 => N = 1
  

}


ISR(TIMER1_OVF_vect){
  	/*Count is used to keep track of the number of times the 
    counter has overflowed. This is then included in the 
    difference variable to get the true value of the difference
    in time.*/
	count = count + 1; 
  	TCNT1 = 3035; // This is done to have a period of exactly 1s
  }


