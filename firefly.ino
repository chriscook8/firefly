
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
 * {{what if the badge isn't placed down for ref to be taken}}
 * 
 * modes
 * if night, blind, firefly blink, blind, sense (light and touch) for period until next blink; 
 *    light sense advances next blink period { how to sence blink light vs daylight }
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


  #define day 90
  #define flash 20

  #define tt_off 0
  #define tt_on 1
  #define tt_push 2
  #define tt_release 3
  #define tt_timeout 4

  volatile int photoVal = 0;
  volatile boolean wingOverride = false;
  volatile unsigned long lastMillis = 0;
  volatile unsigned long currentMillis = 0;

  volatile int input_number;
  volatile int ones;
  volatile int tens;
  volatile int hundreds;
  volatile int thousands;


void(* resetFunc) (void) = 0; //declare reset function @ address 0

void init_ADC() {

  ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (0 << ADPS2) |     // set prescaler to 8, bit 2 
            (1 << ADPS1) |     // set prescaler to 8, bit 1 
            (1 << ADPS0);      // set prescaler to 8, bit 0  
  
}

int get_photo() {
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


void init_blink(int a){
  for (int i = 0; i < a; i++){
    analogWrite(ledPin, 255);
    delay(500);
    analogWrite(ledPin, 0);
    delay(300);
  }
}

void firefly_blink(){
  //blink with nice sinlog fade effect
  float in, out;
  for (in = 4.712; in < 10.995; in = in + 0.01)
  {
    out = sin(in) * 127.5 + 127.5;
    analogWrite(ledPin,out);
    delay(1);
  }
  digitalWrite(ledPin, LOW);
  delay(100);
  for (in = 4.712; in < 10.995; in = in + 0.01)
  {
    out = sin(in) * 127.5 + 127.5;
    analogWrite(ledPin,out);
    delay(1);
  }
  digitalWrite(ledPin, LOW);
}


void setup() {
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  //pinMode(0, OUTPUT);


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
  photoVal = get_photo();
  if (tinytouch_sense()==tt_push) { init_blink(2); wingOverride = !wingOverride; }

  if (photoVal < day || wingOverride) { // we think it's nighttime, or the wings have been pressed

    // every 8 seconds, blink
    if (currentMillis - lastMillis > 8000){
      firefly_blink();
      lastMillis = millis();
    }
    
  } else {
    //it's day time
    // occasionally look for wing presses
    // check to see if it's night yet   
    
  }

//
//input_number = photoVal;
//
//ones = (input_number%10);
//tens = ((input_number/10)%10);
//hundreds = ((input_number/100)%10);
////thousands = (input_number/1000);
//
//for (int a=0; a < hundreds; a++){
//    analogWrite(ledPin, 255);
//    delay(500);
//    analogWrite(ledPin, 0);
//    delay(500);
//}
//
//    analogWrite(ledPin, 0);
//    delay(2000);
//
//for (int a=0; a < tens; a++){
//    analogWrite(ledPin, 255);
//    delay(500);
//    analogWrite(ledPin, 0);
//    delay(500);
//}
//
//    analogWrite(ledPin, 0);
//    delay(2000);
//
//for (int a=0; a < ones; a++){
//    analogWrite(ledPin, 255);
//    delay(500);
//    analogWrite(ledPin, 0);
//    delay(500);
//}
//
//delay(10000);

//tone(0, photoVal);
//delay(2000);
//noTone(0);

}
