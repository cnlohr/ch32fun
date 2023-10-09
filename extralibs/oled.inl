//
// SSD1306 OLED display library
// written by Larry Bank
// bitbank@pobox.com
// Copyright (c) 2023 BitBank Software, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <stdint.h>
#include <string.h>
#include "oled.h"
#include "ch32v_hal.h"

static int cursor_x, cursor_y;
static uint8_t oledAddr;
static uint8_t ucTemp[40], u8Cache[130];

const unsigned char oled64_initbuf[]={0x00,0xae,0xa8,0x3f,0xd3,0x00,0x40,0xa1,0xc8,
      0xda,0x12,0x81,0xff,0xa4,0xa6,0xd5,0x80,0x8d,0x14,
      0xaf,0x20,0x02};

const uint8_t ucFont[] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x5f,0x5f,0x06,0x00,
  0x00,0x07,0x07,0x00,0x07,0x07,0x00,0x14,0x7f,0x7f,0x14,0x7f,0x7f,0x14,
  0x24,0x2e,0x2a,0x6b,0x6b,0x3a,0x12,0x46,0x66,0x30,0x18,0x0c,0x66,0x62,
  0x30,0x7a,0x4f,0x5d,0x37,0x7a,0x48,0x00,0x04,0x07,0x03,0x00,0x00,0x00,
  0x00,0x1c,0x3e,0x63,0x41,0x00,0x00,0x00,0x41,0x63,0x3e,0x1c,0x00,0x00,
  0x08,0x2a,0x3e,0x1c,0x3e,0x2a,0x08,0x00,0x08,0x08,0x3e,0x3e,0x08,0x08,
  0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,
  0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x03,0x01,
  0x3e,0x7f,0x59,0x4d,0x47,0x7f,0x3e,0x40,0x42,0x7f,0x7f,0x40,0x40,0x00,
  0x62,0x73,0x59,0x49,0x6f,0x66,0x00,0x22,0x63,0x49,0x49,0x7f,0x36,0x00,
  0x18,0x1c,0x16,0x53,0x7f,0x7f,0x50,0x27,0x67,0x45,0x45,0x7d,0x39,0x00,
  0x3c,0x7e,0x4b,0x49,0x79,0x30,0x00,0x03,0x03,0x71,0x79,0x0f,0x07,0x00,
  0x36,0x7f,0x49,0x49,0x7f,0x36,0x00,0x06,0x4f,0x49,0x69,0x3f,0x1e,0x00,
  0x00,0x00,0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x80,0xe6,0x66,0x00,0x00,
  0x08,0x1c,0x36,0x63,0x41,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x14,0x14,
  0x00,0x41,0x63,0x36,0x1c,0x08,0x00,0x00,0x02,0x03,0x59,0x5d,0x07,0x02,
  0x3e,0x7f,0x41,0x5d,0x5d,0x5f,0x0e,0x7c,0x7e,0x13,0x13,0x7e,0x7c,0x00,
  0x41,0x7f,0x7f,0x49,0x49,0x7f,0x36,0x1c,0x3e,0x63,0x41,0x41,0x63,0x22,
  0x41,0x7f,0x7f,0x41,0x63,0x3e,0x1c,0x41,0x7f,0x7f,0x49,0x5d,0x41,0x63,
  0x41,0x7f,0x7f,0x49,0x1d,0x01,0x03,0x1c,0x3e,0x63,0x41,0x51,0x33,0x72,
  0x7f,0x7f,0x08,0x08,0x7f,0x7f,0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00,
  0x30,0x70,0x40,0x41,0x7f,0x3f,0x01,0x41,0x7f,0x7f,0x08,0x1c,0x77,0x63,
  0x41,0x7f,0x7f,0x41,0x40,0x60,0x70,0x7f,0x7f,0x0e,0x1c,0x0e,0x7f,0x7f,
  0x7f,0x7f,0x06,0x0c,0x18,0x7f,0x7f,0x1c,0x3e,0x63,0x41,0x63,0x3e,0x1c,
  0x41,0x7f,0x7f,0x49,0x09,0x0f,0x06,0x1e,0x3f,0x21,0x31,0x61,0x7f,0x5e,
  0x41,0x7f,0x7f,0x09,0x19,0x7f,0x66,0x26,0x6f,0x4d,0x49,0x59,0x73,0x32,
  0x03,0x41,0x7f,0x7f,0x41,0x03,0x00,0x7f,0x7f,0x40,0x40,0x7f,0x7f,0x00,
  0x1f,0x3f,0x60,0x60,0x3f,0x1f,0x00,0x3f,0x7f,0x60,0x30,0x60,0x7f,0x3f,
  0x63,0x77,0x1c,0x08,0x1c,0x77,0x63,0x07,0x4f,0x78,0x78,0x4f,0x07,0x00,
  0x47,0x63,0x71,0x59,0x4d,0x67,0x73,0x00,0x7f,0x7f,0x41,0x41,0x00,0x00,
  0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0x00,0x41,0x41,0x7f,0x7f,0x00,0x00,
  0x08,0x0c,0x06,0x03,0x06,0x0c,0x08,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x00,0x00,0x03,0x07,0x04,0x00,0x00,0x20,0x74,0x54,0x54,0x3c,0x78,0x40,
  0x41,0x7f,0x3f,0x48,0x48,0x78,0x30,0x38,0x7c,0x44,0x44,0x6c,0x28,0x00,
  0x30,0x78,0x48,0x49,0x3f,0x7f,0x40,0x38,0x7c,0x54,0x54,0x5c,0x18,0x00,
  0x48,0x7e,0x7f,0x49,0x03,0x06,0x00,0x98,0xbc,0xa4,0xa4,0xf8,0x7c,0x04,
  0x41,0x7f,0x7f,0x08,0x04,0x7c,0x78,0x00,0x44,0x7d,0x7d,0x40,0x00,0x00,
  0x60,0xe0,0x80,0x84,0xfd,0x7d,0x00,0x41,0x7f,0x7f,0x10,0x38,0x6c,0x44,
  0x00,0x41,0x7f,0x7f,0x40,0x00,0x00,0x7c,0x7c,0x18,0x78,0x1c,0x7c,0x78,
  0x7c,0x78,0x04,0x04,0x7c,0x78,0x00,0x38,0x7c,0x44,0x44,0x7c,0x38,0x00,
  0x84,0xfc,0xf8,0xa4,0x24,0x3c,0x18,0x18,0x3c,0x24,0xa4,0xf8,0xfc,0x84,
  0x44,0x7c,0x78,0x4c,0x04,0x0c,0x18,0x48,0x5c,0x54,0x74,0x64,0x24,0x00,
  0x04,0x04,0x3e,0x7f,0x44,0x24,0x00,0x3c,0x7c,0x40,0x40,0x3c,0x7c,0x40,
  0x1c,0x3c,0x60,0x60,0x3c,0x1c,0x00,0x3c,0x7c,0x60,0x30,0x60,0x7c,0x3c,
  0x44,0x6c,0x38,0x10,0x38,0x6c,0x44,0x9c,0xbc,0xa0,0xa0,0xfc,0x7c,0x00,
  0x4c,0x64,0x74,0x5c,0x4c,0x64,0x00,0x08,0x08,0x3e,0x77,0x41,0x41,0x00,
  0x00,0x00,0x00,0x77,0x77,0x00,0x00,0x41,0x41,0x77,0x3e,0x08,0x08,0x00,
  0x02,0x03,0x01,0x03,0x02,0x03,0x01,0x70,0x78,0x4c,0x46,0x4c,0x78,0x70};
// 5x7 font (in 6x8 cell)
const uint8_t ucSmallFont[] = {
0x00,0x00,0x00,0x00,0x00,
0x00,0x06,0x5f,0x06,0x00,
0x07,0x03,0x00,0x07,0x03,
0x24,0x7e,0x24,0x7e,0x24,
0x24,0x2b,0x6a,0x12,0x00,
0x63,0x13,0x08,0x64,0x63,
0x36,0x49,0x56,0x20,0x50,
0x00,0x07,0x03,0x00,0x00,
0x00,0x3e,0x41,0x00,0x00,
0x00,0x41,0x3e,0x00,0x00,
0x08,0x3e,0x1c,0x3e,0x08,
0x08,0x08,0x3e,0x08,0x08,
0x00,0xe0,0x60,0x00,0x00,
0x08,0x08,0x08,0x08,0x08,
0x00,0x60,0x60,0x00,0x00,
0x20,0x10,0x08,0x04,0x02,
0x3e,0x51,0x49,0x45,0x3e,
0x00,0x42,0x7f,0x40,0x00,
0x62,0x51,0x49,0x49,0x46,
0x22,0x49,0x49,0x49,0x36,
0x18,0x14,0x12,0x7f,0x10,
0x2f,0x49,0x49,0x49,0x31,
0x3c,0x4a,0x49,0x49,0x30,
0x01,0x71,0x09,0x05,0x03,
0x36,0x49,0x49,0x49,0x36,
0x06,0x49,0x49,0x29,0x1e,
0x00,0x6c,0x6c,0x00,0x00,
0x00,0xec,0x6c,0x00,0x00,
0x08,0x14,0x22,0x41,0x00,
0x24,0x24,0x24,0x24,0x24,
0x00,0x41,0x22,0x14,0x08,
0x02,0x01,0x59,0x09,0x06,
0x3e,0x41,0x5d,0x55,0x1e,
0x7e,0x11,0x11,0x11,0x7e,
0x7f,0x49,0x49,0x49,0x36,
0x3e,0x41,0x41,0x41,0x22,
0x7f,0x41,0x41,0x41,0x3e,
0x7f,0x49,0x49,0x49,0x41,
0x7f,0x09,0x09,0x09,0x01,
0x3e,0x41,0x49,0x49,0x7a,
0x7f,0x08,0x08,0x08,0x7f,
0x00,0x41,0x7f,0x41,0x00,
0x30,0x40,0x40,0x40,0x3f,
0x7f,0x08,0x14,0x22,0x41,
0x7f,0x40,0x40,0x40,0x40,
0x7f,0x02,0x04,0x02,0x7f,
0x7f,0x02,0x04,0x08,0x7f,
0x3e,0x41,0x41,0x41,0x3e,
0x7f,0x09,0x09,0x09,0x06,
0x3e,0x41,0x51,0x21,0x5e,
0x7f,0x09,0x09,0x19,0x66,
0x26,0x49,0x49,0x49,0x32,
0x01,0x01,0x7f,0x01,0x01,
0x3f,0x40,0x40,0x40,0x3f,
0x1f,0x20,0x40,0x20,0x1f,
0x3f,0x40,0x3c,0x40,0x3f,
0x63,0x14,0x08,0x14,0x63,
0x07,0x08,0x70,0x08,0x07,
0x71,0x49,0x45,0x43,0x00,
0x00,0x7f,0x41,0x41,0x00,
0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x7f,0x00,
0x04,0x02,0x01,0x02,0x04,
0x80,0x80,0x80,0x80,0x80,
0x00,0x03,0x07,0x00,0x00,
0x20,0x54,0x54,0x54,0x78,
0x7f,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x28,
0x38,0x44,0x44,0x44,0x7f,
0x38,0x54,0x54,0x54,0x08,
0x08,0x7e,0x09,0x09,0x00,
0x18,0xa4,0xa4,0xa4,0x7c,
0x7f,0x04,0x04,0x78,0x00,
0x00,0x00,0x7d,0x40,0x00,
0x40,0x80,0x84,0x7d,0x00,
0x7f,0x10,0x28,0x44,0x00,
0x00,0x00,0x7f,0x40,0x00,
0x7c,0x04,0x18,0x04,0x78,
0x7c,0x04,0x04,0x78,0x00,
0x38,0x44,0x44,0x44,0x38,
0xfc,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0xfc,
0x44,0x78,0x44,0x04,0x08,
0x08,0x54,0x54,0x54,0x20,
0x04,0x3e,0x44,0x24,0x00,
0x3c,0x40,0x20,0x7c,0x00,
0x1c,0x20,0x40,0x20,0x1c,
0x3c,0x60,0x30,0x60,0x3c,
0x6c,0x10,0x10,0x6c,0x00,
0x9c,0xa0,0x60,0x3c,0x00,
0x64,0x54,0x54,0x4c,0x00,
0x08,0x3e,0x41,0x41,0x00,
0x00,0x00,0x77,0x00,0x00,
0x00,0x41,0x41,0x3e,0x08,
0x02,0x01,0x02,0x01,0x00,
0x3c,0x26,0x23,0x26,0x3c};

void oledInit(uint8_t u8Addr, int iSpeed)
{
	   I2CInit(0,0,iSpeed);
	   oledAddr = u8Addr;
	   I2CWrite(oledAddr, (uint8_t *)oled64_initbuf, sizeof(oled64_initbuf));
} /* oledInit() */

void oledSetPosition(int x, int y)
{
uint8_t buf[4];

  y >>= 3; // divide by 8 for byte row
  buf[0] = 0x00; // command introducer
  buf[1] = 0xb0 | y; // set page to Y
  buf[2] = x & 0xf; // lower column address
  buf[3] = 0x10 | (x >> 4); // upper column addr
  I2CWrite(oledAddr, buf, 4);
} /* oledSetPosition() */

void oledDrawSprite(int x, int y, int cx, int cy, uint8_t *pSprite, int iPitch, int bInvert)
{
    int tx, ty, dx, dy, iStartX;
    uint8_t *s, *d, pix, ucSrcMask, ucDstMask, ucFill;

    if (x+cx < 0 || y+cy < 0 || x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return; // out of bounds
    ucFill = (bInvert) ? 0xff : 0x00;
    dy = y; // destination y
    if (y < 0) // skip the invisible parts
    {
        cy += y;
        y = -y;
        pSprite += (y * iPitch);
        dy = 0;
    }
    if (y + cy > 64)
        cy = OLED_HEIGHT - y;
    iStartX = 0;
    dx = x;
    if (x < 0)
    {
        cx += x;
        x = -x;
        iStartX = x;
        dx = 0;
    }
    if (x + cx > OLED_WIDTH)
        cx = OLED_WIDTH - x;
    u8Cache[0] = 0x40; // data block
    memset(&u8Cache[1], ucFill, sizeof(u8Cache)-1); // start with black
    for (ty=0; ty<cy; ty++)
    {
        s = &pSprite[(iStartX >> 3)];
        d = &u8Cache[1];
        ucSrcMask = 0x80 >> (iStartX & 7);
        pix = *s++;
        ucDstMask = 1 << (dy & 7);
		  for (tx=0; tx<cx; tx++)
		  {
			if (pix & ucSrcMask) { // set pixel in source, set it in dest
				if (bInvert)
					d[0] &= ~ucDstMask;
				else
					d[0] |= ucDstMask;
			}
			d++; // next pixel column
			ucSrcMask >>= 1;
			if (ucSrcMask == 0) // read next byte
			{
				ucSrcMask = 0x80;
				pix = *s++;
			}
		  } // for tx
        if (ucDstMask == 0x80) { // last row of byte, time to write to the display
        	oledSetPosition(dx, dy);
        	I2CWrite(oledAddr, u8Cache, cx+1);
        	memset(&u8Cache[1], ucFill, sizeof(u8Cache)-1);
        }
        dy++;
        pSprite += iPitch;
    } // for ty
} /* oledDrawSprite() */

//
// Turn OLED main power on or off
// I2C is still responsive when power is off
//
void oledPower(int bOn)
{
	ucTemp[0] = 0; // CMD
	ucTemp[1] = 0xae | (bOn != 0); // power on/off (LSB)
	I2CWrite(oledAddr, ucTemp, 2);
} /* oledPower() */

int oledGetCursorX(void)
{
	return cursor_x;
}

int oledGetCursorY(void)
{
	return cursor_y;
}

void oledFill(uint8_t ucData)
{
uint8_t x, y;
uint8_t iLines, iCols;
unsigned char temp[20];

  iLines = 8; //pOLED->oled_y >> 3;
  iCols = 8; //pOLED->oled_x >> 4;
  temp[0] = 0x40; // data introducer
  memset(&temp[1], ucData, 16);
  //pOLED->iCursorX = pOLED->iCursorY = 0;

  for (y=0; y<iLines; y++)
  {
    oledSetPosition(0,y*8); // set to (0,Y)
    for (x=0; x<iCols; x++) // wiring library has a 32-byte buffer, so send 16 bytes so that the data prefix (0x40) can fit
    {
      I2CWrite(oledAddr, temp, 17);
    } // for x
  } // for y
  cursor_x = cursor_y = 0;
} /* oledFill() */
//
// Invert font data
//
void InvertBytes(uint8_t *pData, uint8_t bLen)
{
uint8_t i;
   for (i=0; i<bLen; i++)
   {
      *pData = ~(*pData);
      pData++;
   }
} /* InvertBytes() */

void oledContrast(uint8_t cont)
{
	ucTemp[0] = 0; // CMD
	ucTemp[1] = 0x81; // contrast
	ucTemp[2] = cont; // value
	I2CWrite(oledAddr, ucTemp, 3);
} /* oledContrast() */
//
// Draw a string of normal (8x8), small (6x8) or large (16x32) characters
// At the given col+row
//
int oledWriteString(int x, int y, const char *szMsg, int iSize, int bInvert)
{
int i, iFontOff, iLen;
unsigned char c, *s;

    if (x >= 128 || y >= 64)
       return -1; // can't draw off the display

    if (x == -1)
    	x = cursor_x;
    if (y == -1)
    	y = cursor_y;
    oledSetPosition(x, y);
    if (iSize == FONT_8x8) // 8x8 font
    {
       i = 0;
       while (x < 128 && szMsg[i] != 0 && y < 64)
       {
             c = (unsigned char)szMsg[i];
             iFontOff = (int)(c-32) * 7;
             // we can't directly use the pointer to FLASH memory, so copy to a local buffer
             ucTemp[0] = 0x40; // data introducer
             ucTemp[1] = 0; // space
             memcpy(&ucTemp[2], &ucFont[iFontOff], 7);
             if (bInvert) InvertBytes(&ucTemp[1], 8);
             iLen = 8;
             if (x + iLen > 128) // clip right edge
                 iLen = 128 - x;
             I2CWrite(oledAddr, ucTemp, iLen+1); // write character pattern
             x += iLen;
             if (x >= 128-7) // word wrap enabled?
             {
               x = 0; // start at the beginning of the next line
               y += 8;
               oledSetPosition(x, y);
             }
         i++;
       } // while
       cursor_x = x;
       cursor_y = y;
       return 0;
    } // 8x8
    else if (iSize == FONT_12x16) // 6x8 stretched to 12x16
    {
      i = 0;
      while (x < 128 && y < 64 && szMsg[i] != 0)
      {
// stretch the 'normal' font instead of using the big font
              int tx, ty;
              uint8_t ucTemp2[16];

              ucTemp2[0] = 0x40; // data introducer
              c = szMsg[i] - 32;
              unsigned char uc1, uc2, ucMask, *pDest;
              s = (unsigned char *)&ucSmallFont[(int)c*5];
              ucTemp[0] = 0; // first column is blank
              memcpy(&ucTemp[1], s, 5);
              if (bInvert)
                  InvertBytes(ucTemp, 6);
              // Stretch the font to double width + double height
              memset(&ucTemp[6], 0, 24); // write 24 new bytes
              for (tx=0; tx<6; tx++)
              {
                  ucMask = 3;
                  pDest = &ucTemp[6+tx*2];
                  uc1 = uc2 = 0;
                  c = ucTemp[tx];
                  for (ty=0; ty<4; ty++)
                  {
                      if (c & (1 << ty)) // a bit is set
                          uc1 |= ucMask;
                      if (c & (1 << (ty + 4)))
                          uc2 |= ucMask;
                      ucMask <<= 2;
                  }
                  pDest[0] = uc1;
                  pDest[1] = uc1; // double width
                  pDest[12] = uc2;
                  pDest[13] = uc2;
              }
              // smooth the diagonal lines
              for (tx=0; tx<5; tx++)
              {
                  uint8_t c0, c1, ucMask2;
                  c0 = ucTemp[tx];
                  c1 = ucTemp[tx+1];
                  pDest = &ucTemp[6+tx*2];
                  ucMask = 1;
                  ucMask2 = 2;
                  for (ty=0; ty<7; ty++)
                  {
                      if (((c0 & ucMask) && !(c1 & ucMask) && !(c0 & ucMask2) && (c1 & ucMask2)) || (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2)))
                      {
                          if (ty < 3) // top half
                          {
                              pDest[1] |= (1 << ((ty * 2)+1));
                              pDest[2] |= (1 << ((ty * 2)+1));
                              pDest[1] |= (1 << ((ty+1) * 2));
                              pDest[2] |= (1 << ((ty+1) * 2));
                          }
                          else if (ty == 3) // on the border
                          {
                              pDest[1] |= 0x80; pDest[2] |= 0x80;
                              pDest[13] |= 1; pDest[14] |= 1;
                          }
                          else // bottom half
                          {
                              pDest[13] |= (1 << (2*(ty-4)+1));
                              pDest[14] |= (1 << (2*(ty-4)+1));
                              pDest[13] |= (1 << ((ty-3) * 2));
                              pDest[14] |= (1 << ((ty-3) * 2));
                          }
                      }
                      else if (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2))
                      {
                          if (ty < 4) // top half
                          {
                              pDest[1] |= (1 << ((ty * 2)+1));
                              pDest[2] |= (1 << ((ty+1) * 2));
                          }
                          else
                          {
                              pDest[13] |= (1 << (2*(ty-4)+1));
                              pDest[14] |= (1 << ((ty-3) * 2));
                          }
                      }
                      ucMask <<= 1; ucMask2 <<= 1;
                  }
              }
              iLen = 12;
              if (x + iLen > 128) // clip right edge
                  iLen = 128 - x;
              oledSetPosition(x, y);
              memcpy(&ucTemp2[1], &ucTemp[6], iLen);
              I2CWrite(oledAddr, ucTemp2, iLen+1);
              oledSetPosition(x, y+8);
              memcpy(&ucTemp2[1], &ucTemp[18], iLen);
              I2CWrite(oledAddr, ucTemp2, iLen+1);
              x += iLen;
              if (x >= 128-11) // word wrap enabled?
              {
                  x = 0; // start at the beginning of the next line
                  y += 16;
                  oledSetPosition(x, y);
              }
          i++;
      } // while
      cursor_x = x;
      cursor_y = y;
      return 0;
    } // 12x16
    else if (iSize == FONT_6x8) // 6x8 font
    {
       i = 0;
       while (x < 128 && y < 64 && szMsg[i] != 0)
       {
               c = szMsg[i] - 32;
               // we can't directly use the pointer to FLASH memory, so copy to a local buffer
               ucTemp[0] = 0x40;
               ucTemp[1] = 0;
               memcpy(&ucTemp[2], &ucSmallFont[(int)c*5], 5);
               if (bInvert) InvertBytes(&ucTemp[1], 6);
               iLen = 6;
               if (x + iLen > 128) // clip right edge
                   iLen = 128 - x;
               I2CWrite(oledAddr, ucTemp, iLen+1); // write character pattern
               x += iLen;
               if (x >= 128-5) // word wrap enabled?
               {
                 x = 0; // start at the beginning of the next line
                 y += 8;
                 oledSetPosition(x, y);
               }
         i++;
       }
       cursor_x = x;
       cursor_y = y;
      return 0;
    } // 6x8
  return -1; // invalid size
} /* oledWriteString() */

void oledClearLine(int y)
{
   u8Cache[0] = 0x40; // start of data
   memset(&u8Cache[1], 0, OLED_WIDTH);
   oledSetPosition(0, y);
   I2CWrite(oledAddr, u8Cache, OLED_WIDTH + 1);
} /* oledClearLine() */

//
// Draw a string of characters in a custom font
// A back buffer must be defined
//
void oledWriteStringCustom(const GFXfont *pFont, int x, int y, const char *szMsg, uint8_t ucColor)
{
int i, end_y, dx, dy, tx, ty, iBitOff;
unsigned int c;
uint8_t *s, *d, bits, ucFill=0, ucMask, uc;
GFXfont font;
GFXglyph glyph, *pGlyph;

   u8Cache[0] = 0x40; // start of data
    if (x == 1)
        x = cursor_x;
    if (y == -1)
        y = cursor_y;
   // in case of running on Harvard architecture, get copy of data from FLASH
   memcpy_P(&font, pFont, sizeof(font));
   pGlyph = &glyph;

   i = 0;
   while (szMsg[i] && x < OLED_WIDTH)
   {
      c = szMsg[i++];
      if (c < font.first || c > font.last) // undefined character
         continue; // skip it
      c -= font.first; // first char of font defined
      memcpy_P(&glyph, &font.glyph[c], sizeof(glyph));
      dx = x; // + pGlyph->xOffset; // offset from character UL to start drawing
      dy = y + pGlyph->yOffset;
      s = font.bitmap + pGlyph->bitmapOffset; // start of bitmap data
      // Bitmap drawing loop. Image is MSB first and each pixel is packed next
      // to the next (continuing on to the next character line)
      iBitOff = 0; // bitmap offset (in bits)
      bits = uc = 0; // bits left in this font byte
      end_y = dy + pGlyph->height;
      if (dy < 0) { // skip these lines
          iBitOff += (pGlyph->width * (-dy));
          dy = 0;
      }
      memset(&u8Cache[1], ucFill, sizeof(u8Cache)-1);
      for (ty=dy; ty<end_y && ty < OLED_HEIGHT; ty++) {
          ucMask = 1<<(ty & 7); // destination bit number for this line
          d = &u8Cache[1+pGlyph->xOffset]; // no backing ram; buffer 8 lines at a time
         for (tx=0; tx<pGlyph->width; tx++) {
            if (bits == 0) { // need to read more font data
               uc = pgm_read_byte(&s[iBitOff>>3]); // get more font bitmap data
               bits = 8 - (iBitOff & 7); // we might not be on a byte boundary
               iBitOff += bits; // because of a clipped line
               uc <<= (8-bits);
            } // if we ran out of bits
            if ((dx+tx) < OLED_WIDTH) { // foreground pixel
                if (uc & 0x80) {
                   if (ucColor == 1)
                      d[tx] |= ucMask;
                   else
                      d[tx] &= ~ucMask;
                } else {
                    if (ucColor == 1)
                       d[tx] &= ~ucMask;
                    else
                       d[tx] |= ucMask;
                }
            }
            bits--; // next bit
            uc <<= 1;
         } // for x
          if ((ucMask == 0x80 || ty == end_y-1)) { // dump this line
              oledSetPosition(dx, (ty & 0xfff8));
              I2CWrite(oledAddr, u8Cache, pGlyph->xAdvance+1);
              memset(&u8Cache[1], ucFill, sizeof(u8Cache)-1); // NB: assume no DMA
          }
      } // for y
      x += pGlyph->xAdvance; // width of this character
   } // while drawing characters
   cursor_x = x;
   cursor_y = y;
} /* oledWriteStringCustom() */
