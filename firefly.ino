
/*
 * Firefly 'learn to solder' badge
 * Originally developed for Homeschool Association of California Convention 2017, Maker Space
 * 
 * {link to hardware design}
 * http://github.com/chriscook8/firefly
 * 
 * 1) when photocell detects dark, start binking
 * 2) if photocell detects other binking, speed up delay in attempt to synchronize with other blinking
 * 3) detect wing touch to enter other modes
 * 3a) blink during the day (when photocell detects light)
 * 3b) ? set different blink pattern
 * 
 * 
 * narative:
 * battery added, setup function begins, 
 * hello blink,take ref readings, random pause before normal opperations
 * 
 * 
 * modes
 * if night, blind, firefly blink, blind, sense (light and touch) for period until next blink; 
 *    light sense advances next blink period
 * if day, no blink, low power mode, less frequent sensing to tell it it's become night, or if captouch
 * 
 * touch signals:
 *  - both wings held
 *  -- special bink on detection (hello blink)
 *  -- set allways blink mode toggle
 * 
 */

// put ledPin on PWM pin
// put touch pins on Qtouch pins

  #define ledPin 1
  #define photoPin A1
  #define touch1Pin A3
  #define touch2Pin A2

//Configuration variables
  #define mainloopdelay 100 // sample every x ms
  #define day 90 //photocell day vs night value
  #define percentblink 1.1 // x * average to trigger blinkdetect
  #define flashadvance 200 //skip forward x on blink detect
  #define goblind 500 // pause after bink detect
  #define blinkfreq 8000 // Blink every x
//
  #define tt_off 0
  #define tt_on 1
  #define tt_push 2
  #define tt_release 3
  #define tt_timeout 4

  #define numReadings 20 // x readings for average light

  uint8_t photoVal = 0;
  uint8_t photoReadings[numReadings];
  uint8_t photoCounter = 0;
  uint16_t photoTotal = 0;
  uint8_t photoAvg = 0;
    
  boolean wingOverride = false;
  unsigned long lastMillis = 0;
  unsigned long currentMillis = 0;


void init_ADC() {

  ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (0 << ADPS2) |     // set prescaler to 8, bit 2 
            (1 << ADPS1) |     // set prescaler to 8, bit 1 
            (1 << ADPS0);      // set prescaler to 8, bit 0  
  
}

uint8_t get_photo() {
  ADMUX =
            (1 << ADLAR) |     // left shift result
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC1 for input (PB2), MUX bit 3
            (0 << MUX2)  |     // use ADC1 for input (PB2), MUX bit 2
            (0 << MUX1)  |     // use ADC1 for input (PB2), MUX bit 1
            (1 << MUX0);       // use ADC1 for input (PB2), MUX bit 0
  
  ADCSRA |= (1 << ADSC);  // Start A2D Conversions
  while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 
  return ADCH;
}


void init_blink(uint8_t a){
  for (uint8_t cnt = 0; cnt < a; cnt++ ){
    analogWrite(ledPin, 255);
    delay(500);
    analogWrite(ledPin, 0);
    delay(200);
  }
}

void firefly_blink(){
  if (currentMillis - lastMillis > blinkfreq){
    //blink with nice sinlog fade effect
      float in, out;
      // < 10.995 = one wave, < 17.278 = two
      for (in = 4.712; in < 17.278; in = in + 0.01)
      {
        out = sin(in) * 127.5 + 127.5;
        analogWrite(ledPin,out);
        delay(1);
      }
      digitalWrite(ledPin, LOW);
    lastMillis = millis();
  }
}

void setup() {  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  for (uint8_t cnt = 0; cnt < numReadings; cnt++) {
    photoReadings[cnt] = 0;
  }

  init_ADC();
  tinytouch_init();

  init_blink(4);
  
  randomSeed(analogRead(5));
  delay(random(300,3000));  //random delay
}

void loop() {

//set time
  currentMillis = millis();
  if (currentMillis < lastMillis) { lastMillis = currentMillis; currentMillis++; } // rollover protection

// get current vals  and do logic
  delay(mainloopdelay); // don't sample more than x a second
  photoVal = get_photo();
  
// make photoAvg
  photoTotal = photoTotal - photoReadings[photoCounter];
  photoReadings[photoCounter] = photoVal;
  photoTotal = photoTotal + photoReadings[photoCounter];
  photoCounter = photoCounter + 1;
  if (photoCounter >= numReadings) {
    photoCounter = 0;
  }
  photoAvg = photoTotal / numReadings;
  
  if (tinytouch_sense()==tt_push) { init_blink(2); wingOverride = !wingOverride; }

  if (photoAvg < day ) { // we think it's nighttime
    if (photoVal > photoAvg*percentblink) {
      lastMillis = lastMillis - flashadvance;
      delay(goblind);
    }
    firefly_blink();    
  } else if ( wingOverride) {
      firefly_blink();
  }

}
