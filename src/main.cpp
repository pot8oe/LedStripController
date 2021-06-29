/* *
 * Teensy LED Strip Control Interface
 *
 * Copyright (C) 2021 Thomas G. Kenny Jr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * */

#include <Arduino.h>            // Arduino Framework
#define FASTLED_INTERNAL        // Suppress FastLED build banner
#include <FastLED.h>            // FastLED Library
#include "ledgfx.h"             // LED "Graphics" helpers from DavePL
#include "bounce.h"             // Boouncing call effect
#include "comet.h"              // Comet effect
#include "fire.h"               // Fire effect
#include "firewithcolor.h"      // Fire with color palette options
#include "marquee.h"            // Marquee effect
#include "protocol.h"           // Simple ASCII command protocol library
#include "twinkle.h"            // Twinkle effect



/* *
 * Firmware version code
 * Name_Boardversion_FirmwareVersion
 * */
const char* VERSION_CODE = "LEDSC_TEENSY_001";




#define NUM_LEDS                    300                     // FastLED definitions
#define LED_PIN                     7                       // FastLED Data Pin

#define MAX_BRIGHTNESS              255                     // Max brightness value
#define MIN_BRIGHTNESS              0                       // Min brightness value
#define MAX_INPUT_BUFFER_LEN        MAX_PROTO_PACKET_LEN    // Input buffer max length






/* *
 * Command print version
 * */
#define CMD_PRINT_VERSION               "CPV\0"

/* *
 * Command full reset - Reset the board - Not implemented
 * */
#define  CMD_FULL_RESET                 "CFR\0"

/* *
 * Command enter bootloader - Not implemented
 * */
#define  CMD_ENTER_BOOTLOADER           "CEB\0"

/* *
 * Command set debugging
 * params
 * - debugging enabled in HEX:
 *       0x00 - off
 *       0x01 - on
 * */
#define  CMD_SET_DEBUGGING              "CSD\0"

/* *
 * Command Set Effect - Sets the active LED strip effect
 * params
 * - effect code in HEX:
 *      0x00 - Solid Color
 *      0x01 - Rainbow Cycle
 *      0x02 - Comet
 *      0x03 - Comet Rainbow
 *      0x04 - Fire
 *      0x05 - Fire with color
 *      0x06 - Solid Color Pulse
 *      0x07 - Bouncing Ball
 *      0x08 - Twinkle
 * */
#define  CMD_SET_EFFECT                 "CSE\0"

/* *
 * Command Set Color - Sets the base color for effects that use an input color
 * params
 * - Color code 24bit RGB in HEX
 * */
#define CMD_SET_COLOR                   "CSC\0"

/* *
 * Command Set Brightness - Sets the brightness
 * params
 * - Brightness 0-255 in HEX
 * */
#define CMD_SET_BRIGHTNESS              "CSB\0"


/* *
 * LED Strip effects
 * */
typedef enum AvailableEffects
{
    OFF = 0x00,             // Off
    SOLID_COLOR,            // Solid color
    RAINBOW_CYCLE,          // Rainbow cycle
    COMET,                  // Comet with static color
    COMET_RAINBOW,          // Comet rainbow
    FIRE,                   // Classic fire effect
    FIRE_COLOR,             // Fire with color effect
    SOLID_PULSE,            // Solid color pulse
    BOUNCING_BALL,          // Bouncing ball
    TWINKLE,                // Twinkle
    MAX_EFFECT,             // Easy reference to the number of effects
} Effect_t;



CRGB leds[NUM_LEDS] = {0};                                  // Frame buffer for FastLED
CRGB color(175,91,7);                                       // Base color for effects that require an input color
uint8_t brightness = 0x44;                                  // 0-255 LED brightness
int brightnessDelta = -10;                                  // Brightness delta
int fps = 0;                                                // FastLED draw Frames per second
byte hue = HUE_RED;                                         // Current hue for effects that use a base hue
BouncingBallEffect bouncingBall(NUM_LEDS);                  // Bouncing ball effect object
Comet comet(hue);                                           // Comet effect object
FireEffect fire(NUM_LEDS, 15, 100, 15, 4, true, true);      // Fire effect object
FireWithColor fireColor(NUM_LEDS);                          // Fire with color object
Effect_t active_effect = AvailableEffects::OFF;             // Active LED Strip effect
bool debugging = false;                                     // Enable debugging output
uint16_t cib_len=0;                                         // Current input buffer length
char ich = 0;                                               // Current input buffer character index
char char_in_buffer[MAX_PROTO_PACKET_LEN];                  // Input character buffer
proto_pkt_t pkt_receive;                                    // Command protocol receive packet
proto_pkt_t pkt_response;                                   // Command protocol response packet



/* *
 * Fade all LEDS
 * */
void fadeall() {
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i].nscale8(250);
    }
}

/**
 * @brief proc_print_error
 * @param pkt
 */
void proc_print_error(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response, int16_t errorcode) {
    proto_init_response_pkt(pkt_response, pkt_received);
    proto_set_response_pkt_error_code(pkt_response, errorcode);
    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_print_version
 * @param pkt
 */
void proc_print_version(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {
    proto_init_response_pkt(pkt_response, pkt_received);
    proto_append_response_pkt_param(pkt_response, VERSION_CODE);
    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_set_debugging
 * @param pkt
 */
void proc_set_debugging(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {

    if(pkt_received->param_count > 0)
        debugging = atoi(pkt_received->params[0]) != 0;

    proto_init_response_pkt(pkt_response, pkt_received);
    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_set_active_effect
 * @param pkt
 */
void proc_set_active_effect(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {

    proto_init_response_pkt(pkt_response, pkt_received);

    if(pkt_received->param_count <= 0) {
        proto_set_response_pkt_error_code(pkt_response, ERR_PROTO_CP_MISSING_PARAMS);
        proto_print_response_pkt(pkt_response);
        return;
    }

    if(pkt_received->param_count > 0) {
        long effectin = strtol(pkt_received->params[0], NULL, 16);

        if(effectin >= 0 && effectin < AvailableEffects::MAX_EFFECT) {
            active_effect = (AvailableEffects)effectin;
        }
    }


    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_set_color
 * @param pkt
 */
void proc_set_color(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {

    proto_init_response_pkt(pkt_response, pkt_received);

    if(pkt_received->param_count <= 0) {
        proto_set_response_pkt_error_code(pkt_response, ERR_PROTO_CP_MISSING_PARAMS);
        proto_print_response_pkt(pkt_response);
        return;
    }

    if(pkt_received->param_count > 0) {
        uint32_t colorin = strtol(pkt_received->params[0], NULL, 16);
        color.setColorCode(colorin);
    }


    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_set_brightness
 * @param pkt
 */
void proc_set_brightness(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {

    proto_init_response_pkt(pkt_response, pkt_received);

    if(pkt_received->param_count <= 0) {
        proto_set_response_pkt_error_code(pkt_response, ERR_PROTO_CP_MISSING_PARAMS);
        proto_print_response_pkt(pkt_response);
        return;
    }

    if(pkt_received->param_count > 0) {
        brightness = strtol(pkt_received->params[0], NULL, 16);
        FastLED.setBrightness(brightness);
    }

    proto_print_response_pkt(pkt_response);
}

/**
 * @brief proc_input Processes incoming serial data and writes it to pkt_receive
 * @return Packet length when end of packet detected. 0 When packet has been cleared due to no data.
 */
uint8_t proc_input(proto_pkt_t* pkt_received) {

    if(!Serial) return 0;

    while(Serial.available() > 0) {

        ich = (char)Serial.read();

        //ignore \r and \n
        if(cib_len < MAX_INPUT_BUFFER_LEN && ich != PROTO_CR && ich != PROTO_NL)
            char_in_buffer[cib_len++] = ich;

        if(ich == PROTO_CR) {
            int16_t error_code = proto_parse_pkt_buffer(char_in_buffer, cib_len, pkt_received);
            memset(char_in_buffer, 0, MAX_INPUT_BUFFER_LEN);
            cib_len = 0;

            if(error_code >= 0) {
                return error_code;
            } else {
                proto_init_response_pkt(&pkt_response, pkt_received);
                proto_set_response_pkt_error_code(&pkt_response, error_code);
                proto_print_response_pkt(&pkt_response);
            }
        }
    }

    //reset do to buffer overflow
    if(cib_len >= MAX_INPUT_BUFFER_LEN) {
        proto_init_response_pkt(&pkt_response, pkt_received);
        proto_set_response_pkt_error_code(&pkt_response, ERR_PROTO_CP_CMD_OVERFLOW);
        proto_print_response_pkt(&pkt_response);
        memset(char_in_buffer, 0, MAX_INPUT_BUFFER_LEN);
        cib_len = 0;
    }

    return 0;
}

/**
 *
 * @param pkt_received
 * @param pkt_response
 */
void proc_cmd(proto_pkt_t* pkt_received, proto_pkt_t* pkt_response) {

    if(strcmp(pkt_received->cmd, CMD_PRINT_VERSION) == 0) {
        proc_print_version(pkt_received, pkt_response);
    } else if(strcmp(pkt_received->cmd, CMD_FULL_RESET) == 0) {
        proc_print_error(pkt_received, pkt_response, ERR_PROTO_CP_CMD_NOT_IMP);
    } else if(strcmp(pkt_received->cmd, CMD_ENTER_BOOTLOADER) == 0) {
        proc_print_error(pkt_received, pkt_response, ERR_PROTO_CP_CMD_NOT_IMP);
    } else if(strcmp(pkt_received->cmd, CMD_SET_DEBUGGING) == 0) {
        proc_set_debugging(pkt_received, pkt_response);
    } else if(strcmp(pkt_received->cmd, CMD_SET_EFFECT) == 0) {
        proc_set_active_effect(pkt_received, pkt_response);
    } else if(strcmp(pkt_received->cmd, CMD_SET_COLOR) == 0) {
        proc_set_color(pkt_received, pkt_response);
    } else if(strcmp(pkt_received->cmd, CMD_SET_BRIGHTNESS) == 0) {
        proc_set_brightness(pkt_received, pkt_response);
    } else {
        proc_print_error(pkt_received, pkt_response, ERR_PROTO_CP_CMD_UNKNOWN);
    }

    //reset packet
    proto_clear_pkt(pkt_received);
}

/**
 * @brief setup - Arduino application initialization
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Teensy Startup");

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);               // Add our LED strip to the FastLED library

  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);
}

/**
 * @brief loop - Arduino application loop
 */
void loop()
{

    while(true) {

        //read any pending input and process if a full cmd has been read into buffer
        if(proc_input(&pkt_receive) > 0) {
            proc_cmd(&pkt_receive, &pkt_response);
        } else {

            switch(active_effect)
            {

            case AvailableEffects::SOLID_COLOR:
                EVERY_N_MILLISECONDS(1000)
                {
                    for(int i=0; i<NUM_LEDS; i++) leds[i] = color;
                    FastLED.show();
                }
                break;

            case AvailableEffects::RAINBOW_CYCLE:
                EVERY_N_MILLISECONDS(100)
                {
                    hue += 1;
                    for(int i=0; i<NUM_LEDS; i++) {
                        leds[i].setHue(hue);
                        //FastLED.show();
                    }
                    FastLED.show();
                }
                break;

            case AvailableEffects::COMET:
                EVERY_N_MILLISECONDS(16)
                {
                    comet.setHue(HUE_YELLOW);
                    comet.DrawComet();
                    FastLED.show();
                }
                break;

            case AvailableEffects::COMET_RAINBOW:
                EVERY_N_MILLISECONDS(16)
                {
                    comet.setHue(comet.hue()+4);
                    comet.DrawComet();
                    FastLED.show();
                }
                break;

            case AvailableEffects::FIRE:
                EVERY_N_MILLISECONDS(33)
                {
                    FastLED.clear();
                    fire.DrawFire();
                    FastLED.show();
                }
                break;

            case AvailableEffects::FIRE_COLOR:
                EVERY_N_MILLISECONDS(10)
                {
                    FastLED.clear();
                    fireColor.DrawFire();
                    FastLED.show();
                }
                break;

            case AvailableEffects::SOLID_PULSE:
                EVERY_N_MILLISECONDS(33)
                {
                    for(int i=0; i<NUM_LEDS; i++) {
                        leds[i] = color;
                    }
                    FastLED.show();

                    brightness += brightnessDelta;

                    const uint8_t min_pulse_brightness = 50;
                    const uint8_t max_pulse_brightness = 175;

                    if(brightness <= min_pulse_brightness) {

                        brightness = min_pulse_brightness;
                        brightnessDelta = 1;

                    } else if(brightness >= max_pulse_brightness) {

                        brightness = max_pulse_brightness;
                        brightnessDelta = -1;
                    }

                    FastLED.setBrightness(brightness);

                }
                break;

            case AvailableEffects::BOUNCING_BALL:
                EVERY_N_MILLISECONDS(16)
                {
                    FastLED.clear();
                    bouncingBall.Draw();
                    FastLED.show();
                }
                break;

            case AvailableEffects::TWINKLE:
                EVERY_N_MILLISECONDS(16)
                {
                    DrawTwinkle();
                    FastLED.show();
                }
                break;

            default:
            case AvailableEffects::MAX_EFFECT:
            case AvailableEffects::OFF:
                FastLED.clear(true);
                break;

            };



            if(debugging) {

                fps = FastLED.getFPS();

                EVERY_N_SECONDS(1)
                {
                    Serial.println(fps, DEC);
                }

            }


        }

    }

}



