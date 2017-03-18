// Particle Build IDE has an option to automatically include the FastLED library, v3.1.5
// The Particle build is here: https://github.com/focalintent/fastled-sparkcore
// We found that it only compiles if you set the Photon firmware target to 0.6.0,
// some aspect of the library is incompatible with 0.6.1
#include <FastLED.h>

#include "Particle.h"

FASTLED_USING_NAMESPACE;
SYSTEM_MODE(AUTOMATIC);

// LED configuration constants
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
#define PIXEL_PIN D2
#define PIXEL_COUNT 75

// The range of the green animation
#define GREEN_START 80
#define GREEN_END  115

// Current possible animations
enum animation_t { RAINBOW, FIRE, GREEN };

// Global variables
CRGB leds[PIXEL_COUNT];
int color;
int direction;
bool TURNED_ON;
enum animation_t ANIMATION;

// Prototypes for local build, ok to leave in for Build IDE
void rainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);

void setup()
{
  delay(2000);
  color = 0;
  direction = 1;
  TURNED_ON = false;
  ANIMATION = RAINBOW;
   
  Particle.function("alexaOn", alexaOn);
  Particle.function("alexaOff", alexaOff);
  Particle.function("alexaAnim", alexaAnim);
  
  FastLED.addLeds<CHIPSET, PIXEL_PIN, COLOR_ORDER>(leds, PIXEL_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  
  // This ensures that it clears previous animations between turning on/off, when devving
  FastLED.clear();
  FastLED.show();
}

int alexaOn(String none) {
    TURNED_ON = TRUE;
    return 123;
}

int alexaOff(String none) {
    TURNED_ON = FALSE;
    FastLED.clear();
    FastLED.show();
    return 123;
}

int alexaAnim(String animation) {
    // clearing LEDS from previous animation
    FastLED.clear();
    FastLED.show();
    
    if( animation == "fire") {
        ANIMATION = FIRE;
    } else if (animation == "rainbow") {
        ANIMATION = RAINBOW;    
        color = 0;
    } else if (animation == "green") {
        ANIMATION = GREEN;
        color = GREEN_START;
    }
    TURNED_ON = TRUE;
    // We don't need to return anything.
    // Just return an easy to recognize number for testing
    return 123;   
}

void loop()
{
  if (TURNED_ON) {

    switch(ANIMATION) {
        case RAINBOW:
            set_color(color);
            FastLED.show(); // display this frame
            FastLED.delay(1000 / FRAMES_PER_SECOND);
            color++;
            break;
        
        case FIRE:
            random16_add_entropy( random(256));
            
            Fire2012(); // run simulation frame
            
            FastLED.show(); // display this frame
            FastLED.delay(1000 / FRAMES_PER_SECOND);
            break;
        
        case GREEN:
            // 75-140
            set_color(color);
            FastLED.show(); // display this frame
            // todo: func that takes range and automatically slows down based on range
            FastLED.delay(1000 / (FRAMES_PER_SECOND/6));
            
            if (color == GREEN_END) {
                direction = -1;
            } else if (color == GREEN_START) {
                direction = 1;
            }
            color += direction;
            break;
            
    }
  }
  
}

void set_color(int color) {
    uint16_t i;
    
    for(i=0; i< PIXEL_COUNT; i++) {
       leds[i] = CHSV(color, 255, BRIGHTNESS);
    }
}


// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012()
{
// Array of temperature readings at each simulation cell
  const int fire_count = PIXEL_COUNT/2;
  static byte heat[fire_count];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < fire_count; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / fire_count) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k = fire_count - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < fire_count; j++) {
        leds[j] = HeatColor(heat[j]);
    }
}

