// Mbed program to test LPD8806-based RGB LED Strips
// (c) 2011 Jelmer Tiete
// This library is ported from the Arduino implementation of Adafruit Industries
// found at: http://github.com/adafruit/LPD8806
// and their strips: http://www.adafruit.com/products/306
// Released under the MIT License: http://mbed.org/license/mit
//
// standard connected to 1st hardware SPI
// LPD8806  <> MBED
// DATA     -> P5
// CLOCK    -> p7
/*****************************************************************************/

#include "LPD8806.h"

LPD8806 strip = LPD8806(5);

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t delay) {
    int i;

    for (i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);  // turn all pixels off
    }

    for (i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        if (i == 0) {
            strip.setPixelColor(strip.numPixels()-1, 0);
        } else {
            strip.setPixelColor(i-1, 0);
        }
        strip.show();
        wait_ms(delay);
    }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t delay) {
    int i;

    for (i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        wait_ms(delay);
    }
}

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos) {
    uint8_t b=0;
    uint8_t g=0;
    uint8_t r = 0;
    switch (WheelPos / 128) {
        case 0:
            r = 127 - WheelPos % 128;   //Red down
            g = WheelPos % 128;      // Green up
            b = 0;                  //blue off
            break;
        case 1:
            g = 127 - WheelPos % 128;  //green down
            b = WheelPos % 128;      //blue up
            r = 0;                  //red off
            break;
        case 2:
            b = 127 - WheelPos % 128;  //blue down
            r = WheelPos % 128;      //red up
            g = 0;                  //green off
            break;
    }
    return(strip.Color(r,g,b));
}

void rainbow(uint8_t delay) {
    int i, j;

    for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
        for (i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel( (i + j) % 384));
        }
        strip.show();   // write all the pixels out
        wait_ms(delay);
    }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t delay) {
    uint16_t i, j;

    for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
        for (i=0; i < strip.numPixels(); i++) {
            // tricky math! we use each pixel as a fraction of the full 384-color wheel
            // (thats the i / strip.numPixels() part)
            // Then add in j which makes the colors go around per pixel
            // the % 384 is to make the wheel cycle around
            strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
        }
        strip.show();   // write all the pixels out
        wait_ms(delay);
    }
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t delay) {
    int i, j, pos, dir;

    pos = 0;
    dir = 1;

    for (i=0; i<((strip.numPixels()-1) * 8); i++) {
        // Draw 5 pixels centered on pos.  setPixelColor() will clip
        // any pixels off the ends of the strip, no worries there.
        // we'll make the colors dimmer at the edges for a nice pulse
        // look
        strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
        strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
        strip.setPixelColor(pos, strip.Color(r, g, b));
        strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
        strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

        strip.show();
        wait_ms(delay);
        // If we wanted to be sneaky we could erase just the tail end
        // pixel, but it's much easier just to erase the whole thing
        // and draw a new one next time.
        for (j=-2; j<= 2; j++)
            strip.setPixelColor(pos+j, strip.Color(0,0,0));
        // Bounce off ends of strip
        pos += dir;
        if (pos < 0) {
            pos = 1;
            dir = -dir;
        } else if (pos >= strip.numPixels()) {
            pos = strip.numPixels() - 2;
            dir = -dir;
        }
    }
}

int main() {
    printf("starting\r\n");    // Start up the LED strip
    strip.begin();

    // Update the strip, to start they are all 'off'
    strip.show();
    while (1) {

        colorChase(strip.Color(127,127,127), 100);

        // Send a simple pixel chase in...
        colorChase(strip.Color(127,0,0), 100);      // full brightness red
        colorChase(strip.Color(127,127,0), 100);    // orange
        colorChase(strip.Color(0,127,0), 100);        // green
        colorChase(strip.Color(0,127,127), 100);    // teal
        colorChase(strip.Color(0,0,127), 100);        // blue
        colorChase(strip.Color(127,0,127), 100);    // violet

        // fill the entire strip with...
        colorWipe(strip.Color(127,0,0), 100);        // red
        colorWipe(strip.Color(0, 127,0), 100);        // green
        colorWipe(strip.Color(0,0,127), 100);        // blue

        rainbow(10);
        rainbowCycle(5);  // make it go through the cycle fairly fast

        // Back-and-forth lights
        scanner(127,0,0, 50);        // red, slow
        scanner(0,0,127, 5);        // blue, fast
    }
}

