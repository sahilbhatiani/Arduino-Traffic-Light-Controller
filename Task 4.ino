#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 1; //Similar to Task 1, used to change lit lights
volatile bool is_Red = false; //Used to check if red light is Lit
volatile uint8_t pulse_count = 0; // This is used to light the Blue Led twice in a second.
volatile uint8_t car_count = 0; //This keeps track of the number of cars that have breached the red light

//This ISR is triggered when the button is pressed.
ISR(INT0_vect) {  
  if(is_Red == true){
    TIMSK1 |= (1<<OCIE1A); //Enables CompA interrupt for blue Led Pulsing
    car_count++; //Updating car count
    if(car_count >100){car_count = 100;} 
    OCR1B = int(car_count*65535/100.0); //Updating the duty cycle of the PWM.
    
  }
  /*The code below increments OCR1A by TCNT1 + 1000, so that
  the compA interrupt is called after 1000 counts or 0.015s*/
  if(TCNT1+1000 > 65535){
  	OCR1A = TCNT1 + 1000 - 65536; //Accounting for TCNT1 overflow and resetting
  }
  else{OCR1A = TCNT1 + 1000;}
}

//This ISR will pulse the LED twice in 1 second (50% duty cycle)
ISR(TIMER1_COMPA_vect) { 
  if(pulse_count < 4){
  	PORTC ^= (1<<PC4);
    if(TCNT1+16384 > 65535){
      OCR1A = TCNT1 + 16384 - 65535; //Increment of 16384 corrosponds to 0.25s
  }
  	else{OCR1A = TCNT1 + 16384;}
    pulse_count++; //Keeping track of the number of pulses. 
  } 
  //Disabling CompA after blue LED is pulsed twice in a second.
  else{TIMSK1 ^= (1<<OCIE1A);pulse_count = 0;}
  
}

//This ISR sets Pin 13 to low after the duty cycle count is over.
ISR(TIMER1_COMPB_vect) { 
  if(car_count!= 100 && car_count!=0){
    PORTB &= ~(1<<PB5);
  }
}

void setup()
{
  Serial.begin(9600);
  cli();
  //Setting output pins
  DDRB = (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB5);
  DDRC = (1<<PC4);
  
  //Setting inital state of above pins.
  PORTB = (0<<PB0)|(0<<PB1)|(0<<PB2)|(0<<PB5);
  PORTC = (0<<PC4);
  
  // Initialize counter and control register(s).  
  TCCR1B = 0;  
  TCCR1A = 0;  
  TCNT1 = 0;  
  // set mode = normal, prescaler = 256  
  OCR1A = (5000); //This value is random  
  OCR1B = 8000;  //This value is also random.
  TCCR1B |= (1<<CS12);   // prescale = 256 => period = 1.05s  
  TIMSK1 |= (0<<OCIE1A) | (1<<OCIE1B)|(1<<TOIE1); // enable B and overflow interrupt.
  
  EIMSK |= (1<<INT0); // enable external int. on PORTD2 
  EICRA |= (1<<ISC01) | (1<<ISC00); // sense a rising edge
  
  sei();

}

void loop()
{
  asm volatile("nop");
}

//This ISR is kind of similar to traffic light system in task 1.
//There are 2 additions. 1) Setting Pin 13 to HIGH 2) Updating the is_Red Variable.
ISR(TIMER1_OVF_vect){
  if(car_count !=0){PORTB |= (1<<PB5);}
  if(count == 1){
    PORTB |= (1<<PB2);
    PORTB &= ~(1<<PB1);
    is_Red = true;
    count++;
   
  }
  else if(count == 2){
    PORTB |= (1<<PB0);
    PORTB &= ~(1<<PB2);
    is_Red = false;
    count++;
  }
  
  else if(count == 3){
    PORTB |= (1<<PB1);
    PORTB &= ~(1<<PB0);
    count = 1;
    is_Red = false;
  } 
}