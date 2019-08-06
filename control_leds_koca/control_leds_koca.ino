
// Designed for Arduino Nano

#include "hsv2rgb.h"
#include "FastLED.h"
#include "elapsedMillis.h"

#define DEBUG_MODE 0

#define LEDS_PIN 18 // 14 // = A4
#define POTI_FONT_MODE A0
#define POTI_CHAR_MODE A1 
#define POTI_BRIGHT A2
#define POTI_SPEED A3
//#define PHOTO_RST_PIN A3 // not used
#define MODEL_PIXELS WS2812 // WS2811, WS2812b // Fastled
#define MODEL_COLORS GRB //Fastled
#define NUM_LEDS 131  
#define NUM_LEDS_FONT 94  
#define NUM_LEDS_MEGAMAN 7 
#define NUM_LEDS_KONG 9 
#define NUM_LEDS_BLINKY 3 
#define NUM_LEDS_LAKITU 10 
#define NUM_LEDS_BUB 8 
#define FIRST_LED 0         //define on which LED to start an animation
#define INPUT_WAIT 50 // time (ms) to wait for another buttoncheck
#define REFRESH_WAIT 50 // time (ms) to wait for another buttoncheck
#define COOLING  20 //55    // defines the level at which the lighting effect fades before a new "flame" generates
#define SPARKING 80 //120   // defines the rate of flicker which we will see from the flame animation
#define POTI_CORRECTION 1.02 //(correct for the actual limits of the potis


CRGB leds[NUM_LEDS]; //this variable contains the addresses of all LEDs
// the following dictionaries map the specific leds for the font/a character to the addresses:
uint8_t ledsFont[NUM_LEDS_FONT] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112};
uint8_t ledsMegaman[NUM_LEDS_MEGAMAN] = {0, 1, 2, 3, 4, 5, 6};
uint8_t ledsKong[NUM_LEDS_KONG] = {33, 34, 35, 36, 37, 38, 39, 40, 41};
uint8_t ledsBlinky[NUM_LEDS_BLINKY] = {60, 61, 62};
uint8_t ledsLakitu[NUM_LEDS_LAKITU] = {113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
uint8_t ledsBub[NUM_LEDS_BUB] = {123, 124, 125, 126, 127, 128, 129, 130};
CRGBPalette16 firePalette = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Red, CRGB::Yellow);
CRGBPalette16 waterPalette = CRGBPalette16(CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Yellow, CRGB::Green, CRGB::Green, CRGB::Blue, CRGB::Green, CRGB::Green, CRGB::Yellow, CRGB::Yellow, CRGB::Green);

uint8_t fontMode = 0;
uint8_t fontModeMax = 255;
uint8_t fontModePrev = 0;

uint8_t charMode = 0;
uint8_t charModeMax = 255;
uint8_t charModePrev = 0;

uint16_t speed = 0;
uint16_t speedPrev = 0;

uint16_t refreshTime = REFRESH_WAIT;
uint16_t waitingTime = INPUT_WAIT;

uint8_t photoRSTState = 128;    // photo resistor for regulating brightness
//float photoLeakeRate = 0.9; // for smoothing the photo resistor [0,1] //not used currently
const float powf_1023 = powf(1023,2);

uint8_t brightness = 128;
uint8_t brightnessPrev = 128;

elapsedMillis elapsedTime;

uint8_t font_state = 30;
uint8_t char_state = 30;

void setup() {

  // Set up LEDS
  FastLED.addLeds<MODEL_PIXELS, LEDS_PIN, MODEL_COLORS>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.setBrightness(255);
  FastLED.show();

  pinMode(POTI_FONT_MODE, INPUT);
  pinMode(POTI_CHAR_MODE, INPUT);
  pinMode(POTI_BRIGHT, INPUT);
  pinMode(POTI_SPEED, INPUT);
  //pinMode(PHOTO_RST_PIN, INPUT); not used
//  Serial.begin(9600);      // open the serial port at 9600 bps:

  delay(100);
}


void loop_debug() {
  elapsedTime = 0;
  for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
    leds[ledsFont[i]] = CHSV( 0, 255, 255 );
  }
  for (uint8_t i = 0; i < NUM_LEDS_MEGAMAN; i++) {
    leds[ledsMegaman[i]] = CHSV( 184, 2, 128 );
  }
  for (uint8_t i = 0; i < NUM_LEDS_KONG; i++) {
    leds[ledsKong[i]] = CHSV( 64, 2, 128 );
  }
  for (uint8_t i = 0; i < NUM_LEDS_BLINKY; i++) {
    leds[ledsBlinky[i]] = CHSV( 96, 255, 128 );
  }
  for (uint8_t i = 0; i < NUM_LEDS_LAKITU; i++) {
    leds[ledsLakitu[i]] = CHSV( 220, 255, 128 );
  }
  for (uint8_t i = 0; i < NUM_LEDS_BUB; i++) {
    leds[ledsBub[i]] = CHSV( 128, 255, 128 );
  }
  waitingTime = refreshTime;
  
  FastLED.show();
  timedDelay(waitingTime);
}


void loop() {

  elapsedTime = 0;


  // Change the font: Konsolen Café
  // font - water animation
  if (fontMode >= 224 && fontMode <=255) {
    font_state=(font_state+1)%NUM_LEDS;
    fill_palette(leds, NUM_LEDS, font_state, 6, waterPalette, 128, LINEARBLEND);
    waitingTime = 200;
  }

  // font - water animation
  else if (fontMode >= 200 && fontMode <=223) {
    random16_add_entropy( random());  // Add entropy to random number generator; we use a lot of it.
    setFontFireWithPalette();
    waitingTime = refreshTime;
  }

  // font - rainbow animation with reduced colour saturation
  else if (fontMode >= 176 && fontMode <=199) {
    font_state=(font_state+1)%NUM_LEDS_FONT;
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( int(255.0/NUM_LEDS_FONT*(NUM_LEDS_FONT-1-i+font_state))%256, 128, 255 );
    }
    waitingTime = refreshTime;
  }

  // font - rainbow animation
  else if (fontMode >= 152 && fontMode <=175) {
    font_state=(font_state+1)%NUM_LEDS_FONT;
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( int(255.0/NUM_LEDS_FONT*(NUM_LEDS_FONT-1-i+font_state))%256, 255, 255 );
    }
    waitingTime = refreshTime;
  }

  // font - animation with specific colour, fading into white
  else if (fontMode >= 88 && fontMode <=151) {
    font_state=(font_state+1)%NUM_LEDS_FONT;
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      uint8_t k = (NUM_LEDS_FONT-1-i+font_state+FIRST_LED)%NUM_LEDS_FONT;
      leds[ledsFont[i]] = CHSV( (fontMode-88)*4, (255-k), 255 );
    }
    waitingTime = refreshTime;
  }

  // font - fixed colour: warm white
  else if (fontMode >= 80 && fontMode <=87) {
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 125, 15);
    }
    waitingTime = refreshTime;
  }

  // font - fixed colour: normal white
  else if (fontMode >= 72 && fontMode <=79) {
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 165, 110);
    }
    waitingTime = refreshTime;
  }

  // font - fixed colour: sun white
  else if (fontMode >= 64 && fontMode <=71) {
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CRGB( 255, 220, 180);
    }
    waitingTime = refreshTime;
  }

  // font - specific colour with maximal saturation
  else if (fontMode >= 0 && fontMode <=63) {
    for (uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
      leds[ledsFont[i]] = CHSV( fontMode*4, 255, 255 );
    }
    waitingTime = refreshTime;
  }


  // Change the characters: Megaman, Kong, Blinky, Lakito, Bub

  // char - specific colour shift with changing brightness (blinking) - fast
  if (charMode >= 224 && charMode <=255) {
    char_state=(char_state+1)%64;
    setCharacters( (charMode-224)*8, 0, abs(char_state-32)*5-128 );
    waitingTime = refreshTime;
  }

  // char - specific colour shift with changing brightness (blinking) - coupled with font rainbow
  else if (charMode >= 192 && charMode <=223) {
    char_state=(char_state+1)%NUM_LEDS_FONT;
    setCharacters( (charMode-192)*8, 0, abs(char_state-47)*3-128 );
    waitingTime = refreshTime;
  }

  // char - rainbow animation - fast
  else if (charMode >= 160 && charMode <=191) {
    char_state=(char_state+1)%64;
    setCharacters( int(255.0/64*(64-1+char_state))%256, 0, 0 );
    waitingTime = refreshTime;
  }

  // char - rainbow animation - coupled with font rainbow
  else if (charMode >= 128 && charMode <=159) {
    char_state=(char_state+1)%NUM_LEDS_FONT;
    setCharacters( int(255.0/NUM_LEDS_FONT*(NUM_LEDS_FONT-1+char_state))%256, 0, 0 );
    waitingTime = refreshTime;
  }

  // char - specific colour shift with increased saturation
  else if (charMode >= 96 && charMode <=127) {
    setCharacters( (charMode-96)*8, 63, 0 );
    waitingTime = refreshTime;
  }

  // char - specific colour shift with reduced saturation
  else if (charMode >= 64 && charMode <=95) {
    setCharacters( (charMode-64)*8, -64, 0 );
    waitingTime = refreshTime;
  }

  // char - specific colour shift with unmodified saturation and brightness
  else if (charMode >= 0 && charMode <=63) {
    setCharacters( charMode*4, 0, 0 );
    waitingTime = refreshTime;
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


void setCharacters(uint8_t hue, int8_t sat, int8_t val)
{
  //hue: modifikator to shift ALL colours in the H spectrum
  //sat: modifikator to add to or substract from ALL saturation values
  //val: modifikator to add to or substract from ALL brightness values

  setMegaman( hue, sat, val );
  setKong( hue, sat, val );
  setBlinky( hue, sat, val );
  setLakitu( hue, sat, val );
  setBub( hue, sat, val );
}


void setMegaman(uint8_t hue, int8_t sat, int8_t val)
{
  //default col1: 31,235,205 (HSL)
  //default col2: 125,255,116 (HSL)
  //default col3: 150,255,118 (HSL)

  for (uint8_t i = 0; i < 2; i++) {
    leds[ledsMegaman[i]] = CHSV( (39+hue)%256, min(255,max(0,185+sat)), min(255,max(0,205+val)) );
  }
  for (uint8_t j = 2; j < 4; j++) {
    leds[ledsMegaman[j]] = CHSV( (125+hue)%256, min(255,max(0,255+sat)), min(255,max(0,156+val)) );
  }
  for (uint8_t k = 4; k < 7; k++) {
    leds[ledsMegaman[k]] = CHSV( (150+hue)%256, min(255,max(0,255+sat)), min(255,max(0,158+val)) );
  }
}


void setKong(uint8_t hue, int8_t sat, int8_t val)
{
  //default col1: 23,255,124 (HSL)
  //default col2: 4,235,102 (HSL)
  //default col3: 0,0,254 (HSL)

  for (uint8_t i = 0; i < 3; i++) {
    leds[ledsKong[i]] = CHSV( (23+hue)%256, min(255,max(0,255+sat)), min(255,max(0,184+val)) );
  }
  for (uint8_t j = 3; j < 8; j++) {
    leds[ledsKong[j]] = CHSV( (4+hue)%256, min(255,max(0,235+sat)), min(255,max(0,142+val)) );
  }
  for (uint8_t k = 8; k < 9; k++) {
    leds[ledsKong[k]] = CHSV( (0+hue)%256, min(255,max(0,0+sat)), min(255,max(0,254+val)) );
  }
}


void setBlinky(uint8_t hue, int8_t sat, int8_t val)
{
  //default col1: 0,255,127 (HSL)
  //default col2: 42,255,198 (HSL)

  for (uint8_t i = 0; i < 2; i++) {
    leds[ledsBlinky[i]] = CHSV( (0+hue)%256, min(255,max(0,255+sat)), min(255,max(0,167+val)) );
  }
  for (uint8_t j = 2; j < 3; j++) {
    leds[ledsBlinky[j]] = CHSV( (42+hue)%256, min(255,max(0,205+sat)), min(255,max(0,218+val)) );
  }
}


void setLakitu(uint8_t hue, int8_t sat, int8_t val)
{
  //default col1: 0,0,248 (HSL)
  //default col2: 23,255,124 (HSL)

  for (uint8_t i = 0; i < 3; i++) {
    leds[ledsLakitu[i]] = CHSV( (0+hue)%256, min(255,max(0,0+sat)), min(255,max(0,248+val)) );
  }
  for (uint8_t k = 3; k < 6; k++) {
    leds[ledsLakitu[k]] = CHSV( (23+hue)%256, min(255,max(0,255+sat)), min(255,max(0,164+val)) );
  }
  for (uint8_t k = 6; k < 10; k++) {
    leds[ledsLakitu[k]] = CHSV( (0+hue)%256, min(255,max(0,0+sat)), min(255,max(0,248+val)) );
  }
}


void setBub(uint8_t hue, int8_t sat, int8_t val)
{
  //default col1: 75,197,140 (HSL)
  //default col2: 4,241,183 (HSL)
  //default col3: 85,85,253 (HSL)

  for (uint8_t i = 0; i < 2; i++) {
    leds[ledsBub[i]] = CHSV( (10+hue)%256, min(255,max(0,191+sat)), min(255,max(0,193+val)) );
  }
  for (uint8_t j = 2; j < 4; j++) {
    leds[ledsBub[j]] = CHSV( (74+hue)%256, min(255,max(0,237+sat)), min(255,max(0,160+val)) );
  }
  for (uint8_t k = 4; k < 6; k++) {
    leds[ledsBub[k]] = CHSV( (85+hue)%256, min(255,max(0,85+sat)), min(255,max(0,253+val)) );
  }
  for (uint8_t j = 6; j < 8; j++) {
    leds[ledsBub[j]] = CHSV( (75+hue)%256, min(255,max(0,197+sat)), min(255,max(0,160+val)) );
  }
}


void setFontFireWithPalette()   // defines a new function
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS_FONT];

  // Step 1.  Cool down every cell a little
  for ( uint8_t i = 0; i < NUM_LEDS_FONT; i++) {
    heat[i] = qsub8( heat[i], random8(0, ((COOLING * 10) / NUM_LEDS_FONT) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( uint8_t j = NUM_LEDS_FONT - 1; j >= 2; j--) {
    heat[j] = (heat[j - 1] + heat[j - 2] + heat[j - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    uint8_t y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( uint8_t l = 0; l < NUM_LEDS_FONT; l++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[NUM_LEDS_FONT-l-1], 248);
    uint8_t k = (NUM_LEDS_FONT-1-l+48)%NUM_LEDS_FONT;
    leds[ledsFont[k]] = ColorFromPalette( firePalette, colorindex);
  }
}


void checkInputs() {

  //for all potis: with the pow function we transform a linear poti into a quadratic range, with ~Prev*0.8 we smooth out the poti state to avoid flickering
  fontModePrev = fontMode;
  fontMode = (uint8_t)(round(max(min((analogRead(POTI_FONT_MODE)*POTI_CORRECTION) / 1023. * fontModeMax*0.2 + fontModePrev*0.8-0.35, 255.),0.)));

  charModePrev = charMode;
  charMode = (uint8_t)(round(max(min((analogRead(POTI_CHAR_MODE)*POTI_CORRECTION) / 1023. * charModeMax*0.2 + charModePrev*0.8-0.35, 255.),0.)));

  speedPrev = speed;
  speed = (int)(round((1004.-powf(0.+analogRead(POTI_SPEED)*POTI_CORRECTION,2) / powf_1023 * REFRESH_WAIT * 19.24)*0.2 + speedPrev*0.8-0.35));
  refreshTime = speed;
  //refreshTime = REFRESH_WAIT // speed poti is not used

  //photoRSTState = (int)round(photoLeakeRate * photoRSTState + (1.-photoLeakeRate) * (analogRead(PHOTO_RST_PIN) / 1023 * 255 * 0.75 + 63));
  photoRSTState = 255; // photo sensor not used!!

  brightnessPrev = brightness;
  //brightness = (int)round(analogRead(POTI_BRIGHT) / 1023 * 255 / 255. * photoRSTState); // linear
  //brightness = (int)round(powf(0.+analogRead(POTI_BRIGHT),2) / powf_1023 * 255 / 255. * photoRSTState); // quadratic
  brightness = (uint8_t)(round(max(min(powf(0.+analogRead(POTI_BRIGHT)*POTI_CORRECTION,2) / powf_1023 * photoRSTState*0.2 + brightnessPrev*0.8-0.35, 255.),0.)));
//	
//  Serial.print("Check inputs: ");       
//  Serial.print(" fontMode: ");
//  Serial.print(fontMode);       
//  Serial.print(" charMode: ");
//  Serial.print(charMode);
//  Serial.print(" speed: ");
//  Serial.print(speed);
//  Serial.print(" photoRSTState: ");
//  Serial.print(photoRSTState);       
//  Serial.print(" brightness: ");       
//  Serial.println(brightness);           
//  Serial.print(" turnaround: ");       
//  Serial.println(turnaround);           

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
