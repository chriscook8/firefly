#include <util/delay.h>
#include <avr/io.h>

//////////////////////////////////////////////////////////////////////////
//
// User definable settings
//
//////////////////////////////////////////////////////////////////////////

// Define upper and lower threshold for the touch sensing. You may have to
// change these depending on the geometry of your touch button.
// Setting the "on" value lower will make the touch button more sensitive.
// Setting the "off" value higher will make the touch button less likely
// to be "stuck". Too high values can lead to oscillations.

#define touch_threshold_on 177
#define touch_threshold_off 170

// If the touch button is pressed, the bias value is not updated
// to prevent detection of multiple touches. In some cases this may
// lead to a "stuck button" situation. Therefore an update of the bias
// is forced after a certain time unless this function is deactivated.
//
// The value of timeout corresponds to the number of calls to tinytouch_sense().
// The maximum is 255. 
// Setting timeout to 0 will turn the functionality off.

#define touch_timeout 255
//#define touch_timeout 0   // turn off timeout functionality


// Define pins to use for the reference input and the touch button
// The reference pin is used to charge or discharge the internal
// sample&hold capacitor. This pin is used in output mode and should
// not be shorted to VCC or GND externally.
// The sense pin is connected to the touch-button. To improve noise immunity
// a series resistor can be used.

// The pin number must match the corresponding analog input number ADCx. 
// Default port is PORTB. (ATtiny 5/10/13/25/45/85)

#define tt_refpin 5   // Use PB5 as reference pin
#define tt_refadc 0   // Use ADC0 as reference ADC input
#define tt_sensepin 3 // Use PB3 as sense pin
#define tt_senseadc 3 // Use ADC3 as sense ADC input
      
//////////////////////////////////////////////////////////////////////////
//
// Library functions
//
//////////////////////////////////////////////////////////////////////////

// Library initialization
// Call this once to initialize the library functions and the ADC converter
//void  tinytouch_init(void);

// The sense function evaluates the button state and performs internal 
// housekeeping. It should be polled at least 30 times per second to
// update the internal logic. Please note that each call performs 32
// analog to digital conversions with active waiting. This may take 
// several ms.

// Possible return values are:
//    tt_off=0  No touch sensed
//    tt_on   Touch button is active and touch is sensed.
//    tt_push   Touch button is pushed. Use this to initiate one time events.
//    tt_release  Touch button is released. Use this to initiate one time events.
//    tt_timeout  Touch button has been active too long and internal bias was reset. 
//uint8_t tinytouch_sense(void);

// Internal function to read the adc input
//uint8_t tinytouch_adc(void);

uint16_t bias;
uint8_t touch;
#if touch_timeout>0   
uint8_t timer;
#endif
/*
      Capacitive sensing using charge sharing between 
      the S/H capacitor and an external sensing pad     
*/  

void  tinytouch_init(void) {
    
  bias=tinytouch_adc()<<8;
  touch=0;
}

uint8_t tinytouch_sense(void) {
  uint8_t i;
  uint16_t tmp;
  int16_t delta;

  tmp=0;
  for (i=0; i<16; i++) {
    tmp+=tinytouch_adc(); // average 16 samples
    _delay_us(100);
  }

  delta=tmp-(bias>>4);
      
  if (!touch) {
    if (delta>touch_threshold_on) {
      touch=1;
#if touch_timeout>0
      timer=0;          
#endif
      return tt_push;
    }

    // update bias only when touch not active
    bias=(bias-(bias>>6))+(tmp>>2);   // IIR low pass
    return tt_off;    
  } else {
    if (delta<touch_threshold_off) {
      touch=0;
      return tt_release;
    }

#if touch_timeout>0   
    if (timer==255) {
      bias=tinytouch_adc()<<8;      
      return tt_timeout;
    } 
    timer++;
#endif            
    return tt_on;
  }
} 

uint8_t tinytouch_adc(void) { 

  uint8_t dat1,dat2;

  // Precharge Low
  ADMUX =tt_refadc; // connect S/H cap to reference pin 
  PORTB |= _BV(tt_refpin);    // Charge S/H Cap 
  PORTB &=~_BV(tt_sensepin);    // Discharge Pad (0)
  DDRB  |= _BV(tt_refpin)|_BV(tt_sensepin);
    
  _delay_us(32);
  
  DDRB  &=~(_BV(tt_sensepin));  // float pad input, note that pull up is off.

  ADMUX =tt_senseadc|_BV(ADLAR);  // Connect sense input to adc
    
  ADCSRA  |=_BV(ADSC); // Start conversion  
  while (!(ADCSRA&_BV(ADIF)));    
  ADCSRA  |=_BV(ADIF); // Clear ADIF

  dat1=ADCH;

  // Precharge High
  ADMUX  =tt_refadc;  // connect S/H cap to reference pin
  PORTB &=~_BV(tt_refpin);    // Discharge S/H Cap
  PORTB |= _BV(tt_sensepin);    // Charge Pad 
  DDRB  |= _BV(tt_refpin)|_BV(tt_sensepin);
      
  _delay_us(32);

  DDRB  &=~(_BV(tt_sensepin));  // float pad input input
  PORTB &=~_BV(tt_sensepin);    // pull up off

  ADMUX =tt_senseadc|_BV(ADLAR);  // Connect sense input to adc
              
  ADCSRA  |=_BV(ADSC); // Start conversion  
  while (!(ADCSRA&_BV(ADIF)));
  ADCSRA  |=_BV(ADIF); // Clear ADIF

  dat2=ADCH;  

  return dat2-dat1;
}
