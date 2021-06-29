//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        
//
// Description:
//
//   
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------
#ifndef TWINKLE_H
#define TWINKLE_H

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "ledgfx.h"

static const CRGB TwinkleColors [] = 
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow
};

void DrawTwinkle()
{
    static int passCount = 0;
    if (passCount++ == FastLED.size()/4)
    {
        passCount = 0;
        FastLED.clear(false);
    }
    FastLED.leds()[random(FastLED.size())] = TwinkleColors[random(0, ARRAYSIZE(TwinkleColors))];
    delay(200);       
}

#endif
