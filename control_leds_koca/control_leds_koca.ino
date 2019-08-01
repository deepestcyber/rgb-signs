
// Designed for Arduino Nano

#include "hsv2rgb.h"
#include "FastLED.h"
#include "elapsedMillis.h"

#define DEBUG_MODE 0

#define LEDS_PIN 18 // 14 // = A0
#define POTI_FONT_MODE A0
#define POTI_CHAR_MODE A1 
#define POTI_BRIGHT A2
#define PHOTO_RST_PIN A3
#define MODEL_PIXELS WS2812 // WS2811, WS2812b // Fastled
#define MODEL_COLORS GRB //Fastled
#define NUM_LEDS 131  
#define NUM_LEDS_FONT 94  
#define NUM_LEDS_MEGAMAN 7 
#define NUM_LEDS_KONG 9 
#define NUM_LEDS_BLINKY 3 
#define NUM_LEDS_LAKITU 10 
#define NUM_LEDS_BUB 8 
#define FIRST_LED 0 //75 // 47
#define INPUT_WAIT 50 // time (ms) to wait for another buttoncheck
#define REFRESH_WAIT 50 // time (ms) to wait for another buttoncheck
#define COOLING  20 //55           // defines the level at which the lighting effect fades before a new "flame" generates
#define SPARKING 80 //120          // defines the rate of flicker which we will see from the flame animation

CRGB leds[NUM_LEDS];
int ledsFont[NUM_LEDS_FONT] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112};
int ledsMegaman[NUM_LEDS_MEGAMAN] = {0, 1, 2, 3, 4, 5, 6};
int ledsKong[NUM_LEDS_KONG] = {33, 34, 35, 36, 37, 38, 39, 40, 41};
int ledsBlinky[NUM_LEDS_BLINKY] = {60, 61, 62};
int ledsLakitu[NUM_LEDS_LAKITU] = {113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
int ledsBub[NUM_LEDS_BUB] = {123, 124, 125, 126, 127, 128, 129, 130};
CRGBPalette16 firePalette = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Red, CRGB::Yellow);
CRGBPalette16 waterPalette = CRGBPalette16(CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Green, CRGB::Yellow, CRGB::Yellow, CRGB::Green);

// font_font_modes: 0 = light patterns, 1 = image stream (24bit), 2 = music patterns, 3 = NES video stream
uint8_t font_mode = 0;
uint8_t font_modeMax = 255;
uint8_t font_modePrev = 0;

uint8_t char_mode = 0;
uint8_t char_modeMax = 255;
uint8_t char_modePrev = 0;

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
  // TODO: distribute the LEDS over different containers
  FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(ledsMegaman, NUM_LEDS_MEGAMAN).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(leds, 26).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(ledsKong, NUM_LEDS_KONG).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(leds, 18).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(ledsBlinky, NUM_LEDS_BLINKY).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(leds, 60).setCorrection( TypicalSMD5050 );
 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.setBrightness(255);
  FastLED.show();

  pinMode(POTI_FONT_MODE, INPUT);
  pinMode(POTI_BRIGHT, INPUT);
  pinMode(PHOTO_RST_PIN, INPUT);
//  Serial.begin(9600);      // open the serial port at 9600 bps:

  delay(100);
}

void loop1() {
  elapsedTime = 0;
  for (int i = 0; i < NUM_LEDS_FONT; i++) {
    leds[ledsFont[i]] = CHSV( 0, 255, 255 );
  }
  for (int i = 0; i < NUM_LEDS_MEGAMAN; i++) {
    leds[ledsMegaman[i]] = CHSV( 184, 2, 128 );
  }
  for (int i = 0; i < NUM_LEDS_KONG; i++) {
    leds[ledsKong[i]] = CHSV( 64, 2, 128 );
  }
  for (int i = 0; i < NUM_LEDS_BLINKY; i++) {
    leds[ledsBlinky[i]] = CHSV( 96, 255, 128 );
  }
  for (int i = 0; i < NUM_LEDS_LAKITU; i++) {
    leds[ledsLakitu[i]] = CHSV( 220, 255, 128 );
  }
  for (int i = 0; i < NUM_LEDS_BUB; i++) {
    leds[ledsBub[i]] = CHSV( 128, 255, 128 );
  }
  waitingTime = REFRESH_WAIT;
  
  FastLED.show();
  timedDelay(waitingTime);
}

void loop() {

  elapsedTime = 0;
  
  // Change the font: Konsolen Café
  // font_mode - TODO
  if (font_mode >= 224 && font_mode <=255) {
    state=(state+1)%NUM_LEDS;
    fill_palette(leds, NUM_LEDS, state, 6, waterPalette, 128, LINEARBLEND);
    waitingTime = 200;
  }

  // font_mode - TODO
  else if (font_mode >= 200 && font_mode <=223) {
    random16_add_entropy( random());  // Add entropy to random number generator; we use a lot of it.
    setFontFireWithPalette();
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 176 && font_mode <=199) {
    state=(state+1)%NUM_LEDS_FONT;
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( int(255.0/NUM_LEDS_FONT*(NUM_LEDS_FONT-1-i+state))%256, 128, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 152 && font_mode <=175) {
    state=(state+1)%NUM_LEDS_FONT;
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( int(255.0/NUM_LEDS_FONT*(NUM_LEDS_FONT-1-i+state))%256, 255, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 88 && font_mode <=151) {
    state=(state+1)%NUM_LEDS_FONT;
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      uint8_t k = (NUM_LEDS_FONT-1-i+state+FIRST_LED)%NUM_LEDS_FONT;
      leds[ledsFont[i]] = CHSV( (font_mode-88)*4, (255-k), 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 80 && font_mode <=87) {
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 125, 15);
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 72 && font_mode <=79) {
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 165, 110);
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 64 && font_mode <=71) {
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 220, 180);
    }
    waitingTime = REFRESH_WAIT;
  }

  // font_mode - TODO
  else if (font_mode >= 0 && font_mode <=63) {
    for (int i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( font_mode*4, 255, 255 );
    }
    waitingTime = REFRESH_WAIT;
  }

  // Change the characters: Megaman, Kong, Blinky, Lakito, Bub
  // char_mode - TODO
  if (char_mode >= 64 && char_mode <=255) {
    setCharacters( 0, 0 );
    waitingTime = REFRESH_WAIT;
  }

  // char_mode - TODO
  else if (char_mode >= 0 && char_mode <=63) {
    setCharacters( char_mode*4, 255 );
    waitingTime = REFRESH_WAIT;
  }

  if (char_mode >= 0 && char_mode <=255) {
    setCharacters( 0, 255 );
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

void setCharacters(int hue, int sat)
{
  setMegaman( hue, sat );
  setKong( hue, sat );
  setBlinky( hue, sat );
  setLakitu( hue, sat );
  setBub( hue, sat );  
}

void setMegaman(int hue, int sat)
{
  //default col1: 31,235,205 (HSL)
  //default col2: 125,255,116 (HSL)
  //default col3: 150,255,118 (HSL)

  for (int i = 0; i < 2; i++) {
    leds[ledsMegaman[i]] = CHSV( 31, 235, 205 ); 
  }
  for (int j = 2; j < 4; j++) {
    leds[ledsMegaman[j]] = CHSV( 125, 255, 116 );
  }
  for (int k = 4; k < 7; k++) {
    leds[ledsMegaman[k]] = CHSV( 150, 255, 118 );
  }
}

void setKong(int hue, int sat)
{
  //default col1: 23,255,124 (HSL)
  //default col2: 4,235,102 (HSL)
  //default col3: 0,0,254 (HSL)

  for (int i = 0; i < 3; i++) {
    leds[ledsKong[i]] = CHSV( 23,255,124 ); 
  }
  for (int j = 3; j < 8; j++) {
    leds[ledsKong[j]] = CHSV( 4,235,102 );
  }
  for (int k = 8; k < 9; k++) {
    leds[ledsKong[k]] = CHSV( 0,0,254 );
  }
}

void setBlinky(int hue, int sat)
{
  //default col1: 0,255,127 (HSL)
  //default col2: 42,255,198 (HSL)

  for (int i = 0; i < 2; i++) {
    leds[ledsBlinky[i]] = CHSV( 0,255,127 ); 
  }
  for (int j = 2; j < 3; j++) {
    leds[ledsBlinky[j]] = CHSV( 42,255,198 );
  }
}

void setLakitu(int hue, int sat)
{
  //default col1: 0,0,248 (HSL)
  //default col2: 23,255,124 (HSL)

  for (int i = 0; i < 4; i++) {
    leds[ledsLakitu[i]] = CHSV( 0,0,248 );
  }
  for (int k = 4; k < 6; k++) {
    leds[ledsLakitu[k]] = CHSV( 23,255,124 );
  }
  for (int k = 6; k < 10; k++) {
    leds[ledsLakitu[k]] = CHSV( 0,0,248 );
  }
}

void setBub(int hue, int sat)
{
  //default col1: 75,197,140 (HSL)
  //default col2: 4,241,183 (HSL)
  //default col3: 85,85,253 (HSL)

  for (int i = 0; i < 2; i++) {
    leds[ledsBub[i]] = CHSV( 4,241,183 ); 
  }
  for (int j = 2; j < 4; j++) {
    leds[ledsBub[j]] = CHSV( 75,197,140 );
  }
  for (int k = 4; k < 6; k++) {
    leds[ledsBub[k]] = CHSV( 85,85,253 );
  }
  for (int j = 6; j < 8; j++) {
    leds[ledsBub[j]] = CHSV( 75,197,140 );
  }
}

void setFontFireWithPalette()   // defines a new function
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS_FONT];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS_FONT; i++) {
    heat[i] = qsub8( heat[i], random8(0, ((COOLING * 10) / NUM_LEDS_FONT) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int j = NUM_LEDS_FONT - 1; j >= 2; j--) {
    heat[j] = (heat[j - 1] + heat[j - 2] + heat[j - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int l = 0; l < NUM_LEDS_FONT; l++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[NUM_LEDS_FONT-l-1], 248);
    uint8_t k = (NUM_LEDS_FONT-1-l+48)%NUM_LEDS_FONT;
    leds[ledsFont[k]] = ColorFromPalette( firePalette, colorindex);
  }
}

void checkInputs() {

  font_modePrev = font_mode;
  font_mode = (int)(round(analogRead(POTI_FONT_MODE) / 1023. * font_modeMax)*0.2)+(font_modePrev*0.8);

  char_modePrev = font_mode;
  char_mode = (int)(round(analogRead(POTI_CHAR_MODE) / 1023. * font_modeMax)*0.2)+(font_modePrev*0.8);

  //photoRSTState = (int)round(photoLeakeRate * photoRSTState + (1.-photoLeakeRate) * (analogRead(PHOTO_RST_PIN) / 1023 * 255 * 0.75 + 63));
  photoRSTState = 255; // photo sensor not used

  brightnessPrev = brightness;
  //brightness = (int)round(analogRead(POTI_BRIGHT) / 1023 * 255 / 255. * photoRSTState); // linear
  //brightness = (int)round(powf(0.+analogRead(POTI_BRIGHT),2) / powf_1023 * 255 / 255. * photoRSTState); // quadratic
  brightness = (int)(round(powf(0.+analogRead(POTI_BRIGHT),2) / powf_1023 * photoRSTState)*0.2)+(brightnessPrev*0.8);
//	
//  Serial.print("Check inputs: ");       
//  Serial.print(" font_mode: ");
//  Serial.print(font_mode);       
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
