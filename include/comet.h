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
// History:     Sep-28-2020     davepl      Created
//
//---------------------------------------------------------------------------

#ifndef COMET_H
#define COMET_H

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>



class Comet
{

private:
    byte m_hue;

public:

    Comet(byte hue = HUE_RED):
        m_hue(hue)
    {

    }

    /**
     * @brief hue - Get hue
     * @return
     */
    byte hue() const
    {
        return m_hue;
    }

    /**
     * @brief setHue - Set Hue
     * @param hue
     */
    void setHue(byte hue)
    {
        if (m_hue == hue)
            return;

        m_hue = hue;
    }

    /**
     * @brief DrawComet - Draw commet
     */
    void DrawComet()
    {
        const byte fadeAmt = 96;
        const int cometSize = 5;

        static int iDirection = 1;
        static int iPos = 0;

        int numLeds = FastLED.size();
        CRGB* leds = FastLED.leds();

        iPos += iDirection;
        if (iPos == (numLeds - cometSize) || iPos == 0)
            iDirection *= -1;

        for (int i = 0; i < cometSize; i++)
            leds[iPos + i].setHue(m_hue);

        // Randomly fade the LEDs
        for (int j = 0; j < numLeds; j++)
            if (random(10) > 5)
                leds[j] = leds[j].fadeToBlackBy(fadeAmt);
    }

};

#endif
