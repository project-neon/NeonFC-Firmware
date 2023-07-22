#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    2

#define LED_COUNT 4

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void ws2812_init() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void colorWipe(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
  }
}


void blink_led() {
  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(0,255,0)); // Green
  delay(1);
  colorWipe(strip.Color(0,0,0));
  
}
