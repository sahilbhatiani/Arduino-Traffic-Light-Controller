#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t resultConversion = 0;
volatile int mode = 1; //Mode is set to 1 by default
volatile uint16_t timer;
volatile byte button = 0;
volatile byte count = 1;
volatile uint16_t diff = 0; //diff is used to account for resetting of counter after it overflows.
volatile uint16_t increment = 0;
volatile int blue_pulse_count = 0;


void setup()
{
  
  cli(); //Disabling Global I bit. 
  
  //LED pins
  //Setting pins 8,9,10 and A1 as OUTPUT.
  DDRB = (1<<DDB2)|(1<<DDB1)|(1<<DDB0); //Pins 8,9,10.
  DDRC = (1<<DDC1); //Pins A1 = BLUE LED.
  
  //Setting LED pin State
  PORTB = (0<<PB2)|(0<<PB1)|(0<<PB0); //pins 8,9 and 10 are set to LOW.
  PORTC = (1<<PC1); //pins A1 is set to HIGH.
  
  
  //ADC stuff
  ADMUX = 0x0;
  ADMUX |= (0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0); // pin ADC0
  ADMUX |= (0<<REFS1)|(1<<REFS0); // V_REF = AV_cc
  ADCSRA = 0x0;
  ADCSRA |= (1<<ADEN)|(1<<ADIE);
  
  //Timers Setup
  TCCR1A = 0x00;
  TCCR1B = (1<<CS12) | (1<<CS10); //Prescaler: 1024
  OCR1A = 15625; 
  TIMSK1 |= (1<<OCIE1A); // enable compare A
  TCNT1 = 0;
  
  //Button Setup
  EIMSK |= (1<<INT0); // enable external int. on PORTD2  
  EICRA |= (1<<ISC01) | (1<<ISC00); // sense a rising edge
  
  Serial.begin(9600);
  
  sei(); //Sets the I bit.
  
}

void loop()
{
   
}
/*Blue pulse function pulses the Led mode number of times 
per second.*/
void bluePulse(){
  //this part pulses the LED mode number of times per second.
  if(blue_pulse_count < 2*mode){
  	PORTC ^= (1<<PORTC1);
    diff = 65535 - TCNT1;
    increment = 15625/(2*mode);
    if(diff<increment){
    	OCR1A = increment - diff;
    }
    else{OCR1A = TCNT1 + increment;}
    blue_pulse_count++;
  }
  //This part switches the LED off for 2 seconds.
  else{
    PORTC = (0<<PORTC1);
    diff = 65535 - TCNT1;
    increment = 15625*2; // Setting the increment to 2 seconds.
    if(diff<increment){ //Accounting for reseting of TCNT1 due to overflow.
    	OCR1A = increment - diff;
      	
    }
    else{OCR1A = TCNT1 + increment;}
    blue_pulse_count = 0;
  }
}
ISR(TIMER1_COMPA_vect){
  
  //Configuration mode => If button is pressed and light = RED
  if(button == 1 && count == 1){ 
    bluePulse();
    PORTB = (1<<PORTB2);
    //Initiates ADC conversion
    ADCSRA |= (1<<ADSC);
    //The code below finds the mode value based on the potentiometer position.
    if(resultConversion>0 && resultConversion<256){mode=1;}
    else if(resultConversion>255 && resultConversion<511){mode=2;}
    else if(resultConversion>510 && resultConversion<766){mode=3;}
	else if(resultConversion>765 && resultConversion<1024){mode=4;}
    Serial.print(mode); //Prints mode value on serial monitor
    
  }
  
  
  
  //LED traffic system 
  else{
    
    PORTC = (1<<PORTC1);
    diff = 65535 - TCNT1;
    increment = 15625*mode; //Increment deoends on mode.
    if(diff<increment){ //Accounting for overflow and resetting.
    	OCR1A = increment - diff;
    }
    else{OCR1A = TCNT1 + (15625*mode);}
    
    //Similar stuff to Task 1
    if(count == 1){
      PORTB = (1<<PORTB2);
      count++;
    }
    else if(count == 2){
      PORTB = (1<<PORTB1);
      count++;
    }
	else if(count == 3){
      PORTB = (1<<PORTB0);
      count = 1;
    } 
  }
}

//Detects button being pressed.
ISR(INT0_vect) {  
  button == 0 ? button = 1 : button = 0;
}

//Records Analogue to digital conversion.
ISR(ADC_vect) { resultConversion = ADC; }

