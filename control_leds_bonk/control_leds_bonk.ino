
// Designed for Arduino Nano

#include "hsv2rgb.h"
#include "FastLED.h"
#include "elapsedMillis.h"

#define DEBUG_MODE 0

#define LEDS_PIN_0 14 // = A0
#define LEDS_PIN_1 15 // = A1
#define POTI_MODE A2 
#define POTI_BRIGHT A3
#define PHOTO_RST_PIN A4
#define MODEL_PIXELS WS2812 // WS2811, WS2812b // Fastled
#define MODEL_COLORS GRB //Fastled
#define NUM_LEDS 122 // 300
#define FIRST_LED 47 //75 // 300
#define INPUT_WAIT 50 // time (ms) to wait for another buttoncheck
#define REFRESH_WAIT 50 // time (ms) to wait for another buttoncheck
#define COOLING  20 //55           // defines the level at which the lighting effect fades before a new "flame" generates
#define SPARKING 80 //120          // defines the rate of flicker which we will see from the flame animation

CRGB leds0[NUM_LEDS];
//CRGBPalette16 currentPalette = HeatColors_p;
CRGBPalette16 firePalette = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Red, CRGB::Yellow);
//CRGBPalette16 waterPalette = CRGBPalette16(CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Green, CRGB::Yellow, CRGB::Yellow, CRGB::Green);
CRGBPalette16 waterPalette = CRGBPalette16(CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Green, CRGB::Yellow, CRGB::Yellow, CRGB::Green);

// modes: 0 = light patterns, 1 = image stream (24bit), 2 = music patterns, 3 = NES video stream
uint8_t mode = 0;
uint8_t modeMax = 255;
uint8_t modePrev = 0;

int waitingTime = INPUT_WAIT;

int photoRSTState = 128;      // photo resistor for regulating brightness
float photoLeakeRate = 0.9; // for smoothing the photo resistor [0,1]
const float powf_1023 = powf(1023,2);

int brightness = 128;
int brightnessPrev = 128;

elapsedMillis elapsedTime;

uint8_t state = 30;

void setup() {

  // Set up LEDS
  if (NUM_LEDS > 0) FastLED.addLeds<MODEL_PIXELS, LEDS_PIN_0, MODEL_COLORS>(leds0, NUM_LEDS).setCorrection( TypicalSMD5050 );
 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds0[i] = CRGB::Black;
  }
  FastLED.setBrightness(255);
  FastLED.show();

  pinMode(POTI_MODE, INPUT);
  pinMode(POTI_BRIGHT, INPUT);
  pinMode(PHOTO_RST_PIN, INPUT);
//  Serial.begin(9600);      // open the serial port at 9600 bps:

  delay(100);
}

void loop() {

  elapsedTime = 0;

  // mode - TODO
  if (mode >= 224 && mode <=255) {
    state=(state+1)%NUM_LEDS;
    fill_palette(leds0, NUM_LEDS, state, 6, waterPalette, 128, LINEARBLEND);
    waitingTime = 200;
  }

  // mode - TODO
  else if (mode >= 200 && mode <=223) {
    random16_add_entropy( random());  // Add entropy to random number generator; we use a lot of it.
    Fire2012WithPalette();
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 176 && mode <=199) {
    state=(state+1)%NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CHSV( int(255.0/NUM_LEDS*(NUM_LEDS-1-i+state))%256, 128, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 152 && mode <=175) {
    state=(state+1)%NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CHSV( int(255.0/NUM_LEDS*(NUM_LEDS-1-i+state))%256, 255, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 88 && mode <=151) {
    state=(state+1)%NUM_LEDS;
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t k = (NUM_LEDS-1-i+state+FIRST_LED)%NUM_LEDS;
      leds0[i] = CHSV( (mode-88)*4, (255-k), 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 80 && mode <=87) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CRGB( 255, 125, 15);
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 72 && mode <=79) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CRGB( 255, 165, 110);
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 64 && mode <=71) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CRGB( 255, 220, 180);
    }
    waitingTime = REFRESH_WAIT;
  }

  // mode - TODO
  else if (mode >= 0 && mode <=63) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds0[i] = CHSV( mode*4, 255, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  FastLED.show();
  timedDelay(waitingTime);
}

void delayAwake(int time) {

  int start = (int)elapsedTime;
  while (true) {
    if ((int)elapsedTime-start >= time) break;
  }
  return;
}

void checkInputs() {

  modePrev = mode;
  mode = (int)(round(analogRead(POTI_MODE) / 1023. * modeMax)*0.2)+(modePrev*0.8);

  //photoRSTState = (int)round(photoLeakeRate * photoRSTState + (1.-photoLeakeRate) * (analogRead(PHOTO_RST_PIN) / 1023 * 255 * 0.75 + 63));
  photoRSTState = 255; // photo sensor not used

  brightnessPrev = brightness;
  //brightness = (int)round(analogRead(POTI_BRIGHT) / 1023 * 255 / 255. * photoRSTState); // linear
  //brightness = (int)round(powf(0.+analogRead(POTI_BRIGHT),2) / powf_1023 * 255 / 255. * photoRSTState); // quadratic
  brightness = (int)(round(powf(0.+analogRead(POTI_BRIGHT),2) / powf_1023 * photoRSTState)*0.2)+(brightnessPrev*0.8);
//	
//  Serial.print("Check inputs: ");       
//  Serial.print(" mode: ");
//  Serial.print(mode);       
//  Serial.print(" photoRSTState: ");       
//  Serial.print(photoRSTState);       
//  Serial.print(" brightness: ");       
//  Serial.println(brightness);           

  FastLED.setBrightness(brightness);
}

void timedDelay(int waitTime) {
  checkInputs();
  while ((waitTime - (int)elapsedTime) > INPUT_WAIT) {
    delayAwake(INPUT_WAIT);
    checkInputs();
  }
  delayAwake(max(waitTime - (int)elapsedTime, 0));
  waitingTime = 0; // for safety - TODO remove
}

void Fire2012WithPalette()   // defines a new function
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int j = NUM_LEDS - 1; j >= 2; j--) {
    heat[j] = (heat[j - 1] + heat[j - 2] + heat[j - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }
//
//  Serial.print("Heat: ");       
//  Serial.print(heat[0]);       
//  Serial.print(" ");       
//  Serial.print(heat[1]);       
//  Serial.print(" ");       
//  Serial.print(heat[2]);       
//  Serial.print(" ");       
//  Serial.print(heat[4]);       
//  Serial.print(" ");       
//  Serial.print(heat[8]);       
//  Serial.print(" ");       
//  Serial.print(heat[16]);       
//  Serial.print(" ");       
//  Serial.print(heat[32]);       
//  Serial.print(" ");       
//  Serial.println(brightness);           

  // Step 4.  Map from heat cells to LED colors
  for ( int l = 0; l < NUM_LEDS; l++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[NUM_LEDS-l-1], 248);
    uint8_t k = (NUM_LEDS-1-l+48)%NUM_LEDS;
    leds0[k] = ColorFromPalette( firePalette, colorindex);
  }
}
