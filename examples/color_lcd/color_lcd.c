//
// color LCD demo
// written by Larry Bank
// bitbank@pobox.com
//
// Copyright 2023 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "ch32v_hal.inl"
#include "spi_lcd.inl" 
#include <stdlib.h>

// Pin definitions for the LCD project protoboard
//#define BL_PIN 0xd2
//#define CS_PIN 0xff
#define BL_PIN 0xd5
#define CS_PIN 0xd6
#define DC_PIN 0xd2
#define RST_PIN 0xd4

static uint16_t usPal[8] = {COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_GREEN,
						    COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_YELLOW};

/* White Noise Generator State */
#define NOISE_BITS 8
#define NOISE_MASK ((1<<NOISE_BITS)-1)
#define NOISE_POLY_TAP0 31
#define NOISE_POLY_TAP1 21
#define NOISE_POLY_TAP2 1
#define NOISE_POLY_TAP3 0
uint32_t lfsr = 1;

/*
 * random byte generator
 */
uint8_t rand8(void)
{
        uint8_t bit;
        uint32_t new_data;

        for(bit=0;bit<NOISE_BITS;bit++)
        {
                new_data = ((lfsr>>NOISE_POLY_TAP0) ^
                                        (lfsr>>NOISE_POLY_TAP1) ^
                                        (lfsr>>NOISE_POLY_TAP2) ^
                                        (lfsr>>NOISE_POLY_TAP3));
                lfsr = (lfsr<<1) | (new_data&1);
        }

        return lfsr&NOISE_MASK;
}

int main(void)
{
int i, dx, iColor;

    SystemInit();

    lcdInit(LCD_ST7735_80x160, 24000000, CS_PIN, DC_PIN, RST_PIN, BL_PIN);
    lcdFill(COLOR_GREEN);
    i = 1; dx = 1;
    while (1) {
    	lcdRectangle(rand8() & 127, rand8() & 63, rand8() & 63, rand8() & 31, usPal[(iColor+1)&7], 1);
    	lcdEllipse(rand8() & 127, rand8() & 63, rand8() & 63, rand8() & 31, usPal[iColor & 7], 1);
    	iColor++;
    	lcdWriteString(0,i,"CH32V003 is fast", COLOR_BLUE, COLOR_GREEN, FONT_8x8);
    	lcdWriteString(0,i+8,"enough for me!", COLOR_RED, COLOR_GREEN, FONT_8x8);
    	lcdWriteString(0,i+16,"Large Font!", COLOR_WHITE, COLOR_MAGENTA, FONT_12x16);
    	i += dx;
    	if (i >= 39 || i == 1) dx = -dx;
    }
}
