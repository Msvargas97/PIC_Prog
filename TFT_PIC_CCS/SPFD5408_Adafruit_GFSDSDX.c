
/*
This is the core graphics library FOR all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library FOR each display device we carry
(to handle the lower-level functions).
Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
Copyright (c) 2013 Adafruit Industries.  All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and / or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
Libreria TFTimportada de Arduino By MICHAEL VARGAS para su uso inicial
con la TFT Shield Arduino de  2.4' 320x2 
 Thanks to:
 Modified FOR SPFD5408 by Joao Lopes to work with SPFD5408
*/
#define tft_putc  write
#include <math.h>
#include <stdlib.h> // FOR atoi32
#include "glcdfont.c"
#include "pin_magic.h"
#include "registers.h"
#define pgm_read_byte(addr) (addr)
#define pgm_read_word(addr) ((addr))
#ifndef min
 #define min (a, b) ( (a < b) ? a : b) 
#endif
#define swap(a, b) { INT16 t = a; a = b; b = t; }
#define TFTWIDTH   240
#define TFTHEIGHT  320

// LCD controller chip identifiers
#define ID_932X    0
#define ID_7575    1
#define ID_9341    2
#define ID_HX8357D    3
#define ID_UNKNOWN 0xFF
#define FillScreen(color)  fillRect(0, 0, _width, _height, color)

void Adafruit_GFX(INT16 w, int16 h); // Constructor

  // These MAY be overridden by the subclass to provide device - specific
  // optimized code. Otherwise 'generic' versions are used.
   VOID drawLine (int16 x0, int16 y0, int16 x1, int16 y1, unsigned int16 color);
   VOID drawFastVLine (signed int16 x, signed int16 y, int16 h, unsigned int16 color);
   VOID drawFastHLine (signed int16 x, signed int16 y, int16 w, unsigned int16 color);
   VOID drawRect (int16 x, int16 y, int16 w, int16 h, unsigned int16 color);
   VOID fillRect (signed int16 x1, signed int16 y1, signed int16 w, signed int16 h, UNSIGNED int16 fillcolor);
   VOID invertDisplay (int1 i);
   VOID drawCircle (int16 x0, int16 y0, int16 r, unsigned int16 color);
   VOID drawCircleHelper (int16 x0, int16 y0, int16 r, unsigned charcornername,UNSIGNED int16 color);
   VOID fillCircle (int16 x0, int16 y0, int16 r, unsigned int16 color);
   VOID fillCircleHelper (int16 x0, int16 y0, int16 r, unsigned char cornername, int16 delta, UNSIGNED int16 color);
   VOID drawTriangle (int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, UNSIGNED int16 color);
   VOID fillTriangle (int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, UNSIGNED int16 color);
   VOID drawRoundRect (int16 x0, int16 y0, int16 w, int16 h,int16 radius, UNSIGNED int16 color);
   VOID fillRoundRect (int16 x0, int16 y0, int16 w, int16 h, int16 radius, unsigned INT16 color);
   VOID drawBitmap (int16 x, int16 y, unsigned char * bitmap, int16 w, int16 h,UNSIGNED int16 color);
   VOID drawBitmap (int16 x, int16 y, unsigned char * bitmap, int16 w, int16 h,UNSIGNED int16 color, unsigned int16 bg);
   VOID drawXBitmap (int16 x, int16 y, unsigned char * bitmap, int16 w, int16 h, UNSIGNED int16 color);
   VOID drawChar (signed int16 x, signed int16 y, unsigned char c, unsigned int16 color,   UNSIGNED int16 bg, unsigned char size);
   VOID setCursor (signed int16 x, signed int16 y) ;
   VOID setTextColor (unsigned int16 c);
   VOID setTextColor (unsigned int16 c, unsigned int16 bg);
   VOID setTextSize (unsigned char s);
   VOID setTextWrap (int1 w);
   VOID setRotation (unsigned char r);
   VOID cp437 (int1 x = true) ;
   VOID write (unsigned char);
   VOID begin (unsigned int16 id);
   VOID drawPixel (signed int16 x, signed int16 y, unsigned int16 color); 
 // VOID drawFastVLine (int16 x0, int16 y0, int16 h, unsigned int16 color);
  //VOID fillRect (int16 x, int16 y, int16 w, int16 h, unsigned int16 c); 
  //VOID fillScreen (unsigned int16 color); 
   VOID reset (void);
   VOID setRegisters8 (unsigned char * ptr, unsigned char n);
   VOID setRegisters16 (unsigned int16 * ptr, unsigned char n);

   VOID setAddrWindow (signed int16 x1, signed int16 y1, signed int16 x2, signed INT16 y2);
   VOID pushColors (unsigned int16 * data, unsigned char len, int1 first); 
   UNSIGNED int16 color565 (unsigned char r, unsigned char g, unsigned char b);
   UNSIGNED int16 readPixel (signed int16 x, signed int16 y);
   unsigned int16 readID (VOID);
   UNSIGNED int32 readReg (unsigned char r);
   VOID TFTLCD_init ();
   VOID setLR (void);
   VOID flood (unsigned int16 color, unsigned int32 len);
   VOID writeRegister24 (unsigned char a, unsigned int32 d);
   VOID writeRegister32 (unsigned char a, unsigned int32 d); 
  
  UNSIGNED char driver;
  UNSIGNED int16 _reset;
  UNSIGNED char getRotation ();

  // get current cursor position (get rotation safe maximum values, using: width () FOR x, height () for y)
  int16 getCursorX (VOID);
  int16 getCursorY (VOID);
  const SIGNED int16 WIDTH = TFTWIDTH, HEIGHT = TFTHEIGHT; // This is the 'raw'display w / h - never changes
  SIGNED int16 _width = TFTWIDTH, _height = TFTHEIGHT, // Display w / h as modified by current rotation
    cursor_x = 0, cursor_y = 0;
  UNSIGNED int16
    textcolor, textbgcolor;
  UNSIGNED char
    textsize,
    rotation;

  INT1 wrap, // IF set, 'wrap'text at right edge of display
    _cp437; // IF set, use correct CP437 charset (default is off)

void Adafruit_GFX(INT16 w, int16 h)
{
   // ! WIDTH (w), HEIGHT (h);
   _width = w;
   _height = h;
   rotation = 0;
   cursor_y = cursor_x = 0;
   textsize = 1;
   textcolor = textbgcolor = 0xFFFF;
   wrap = true;
   _cp437 = false;
}

// Draw a circle outline
void drawCircle(INT16 x0, int16 y0, int16 r,UNSIGNED int16 color)
{
       SIGNED int32 f = 1 - r;
       INT16 ddF_x = 1;
       INT16 ddF_y = -2 * r;
       INT16 x = 0;
       INT16 y = r;
       drawPixel (x0, y0 + r, color);
       drawPixel (x0, y0 - r, color);
       drawPixel (x0 + r, y0, color);
       drawPixel (x0 - r, y0, color);

       WHILE (x < y)
       {
          IF (f >= 0)
          {
             y--;
             ddF_y += 2;
             f += ddF_y;
          }

          x++;
          ddF_x += 2;
          f += ddF_x;
          
          drawPixel (x0 + x, y0 + y, color);
          drawPixel (x0 - x, y0 + y, color);
          drawPixel (x0 + x, y0 - y, color);
          drawPixel (x0 - x, y0 - y, color);
          drawPixel (x0 + y, y0 + x, color);
          drawPixel (x0 - y, y0 + x, color);
          drawPixel (x0 + y, y0 - x, color);
          drawPixel (x0 - y, y0 - x, color);
       }
}
VOID drawCircleHelper (int16 x0, int16 y0,
    int16 r, UNSIGNED char cornername, unsigned int16 color)
{
       SIGNED int16 f = 1 - r;
       INT16 ddF_x = 1;
       INT16 ddF_y = -2 * r;
       INT16 x = 0;
       INT16 y = r;

       WHILE (x < y)
       {
          IF (f >= 0)
          {
             y--;
             ddF_y += 2;
             f += ddF_y;
          }

          x++;
          ddF_x += 2;
          f += ddF_x;

          IF (cornername&0x4)
          {
             drawPixel (x0 + x, y0 + y, color);
             drawPixel (x0 + y, y0 + x, color);
          }

          IF (cornername&0x2)
          {
             drawPixel (x0 + x, y0 - y, color);
             drawPixel (x0 + y, y0 - x, color);
          }

          IF (cornername&0x8)
          {
             drawPixel (x0 - y, y0 + x, color);
             drawPixel (x0 - x, y0 + y, color);
          }

          IF (cornername&0x1)
          {
             drawPixel (x0 - y, y0 - x, color);
             drawPixel (x0 - x, y0 - y, color);
          }
       }
    }

VOID fillCircle (int16 x0, int16 y0, int16 r,UNSIGNED int16 color)    {
       drawFastVLine (x0, y0 - r, 2 * r + 1, color) ;
       fillCircleHelper (x0, y0, r, 3, 0, color);
    }

    // Used to DO circles and roundrects
    VOID fillCircleHelper (int16 x0, int16 y0, int16 r,
    UNSIGNED char cornername, int16 delta, unsigned int16 color)
    {
       SIGNED int16 f = 1 - r;
       SIGNED int16 ddF_x = 1;
       SIGNED int16 ddF_y = -2 * r;
       SIGNED int16 x = 0;
       SIGNED int16 y = r;

       WHILE (x < y)
       {
          IF (f >= 0)
          {
             y--;
             ddF_y += 2;
             f += ddF_y;
          }

          x++;
          ddF_x += 2;
          f += ddF_x;

          IF (cornername&0x1)
          {
             drawFastVLine (x0 + x, y0 - y, 2 * y + 1 + delta, color);
             drawFastVLine (x0 + y, y0 - x, 2 * x + 1 + delta, color);
          }

          IF (cornername&0x2)
          {
             drawFastVLine (x0 - x, y0 - y, 2 * y + 1 + delta, color);
             drawFastVLine (x0 - y, y0 - x, 2 * x + 1 + delta, color);
          }
       }
    }

    // Bresenham's algorithm - thx wikpedia
    VOID drawLine (int16 x0, int16 y0,
    INT16 x1, int16 y1,
    UNSIGNED int16 color)
    {
       SIGNED int16 steep = abs (y1 - y0) > abs (x1 - x0) ;

       IF (steep)
       {
          swap (x0, y0);
          swap (x1, y1);
       }

       IF (x0 > x1)
       {
          swap (x0, x1);
          swap (y0, y1);
       }

       SIGNED int16 dx, dy;
       dx = x1 - x0;
       dy = abs (y1 - y0);
       SIGNED int16 err = dx / 2;
       SIGNED int16 ystep;

       IF (y0 < y1)
       {
          ystep = 1;
          } ELSE {
          ystep = -1;
       }

       FOR (; x0 <= x1; x0++)
       {
          IF (steep)
          {
             drawPixel (y0, x0, color);
             } ELSE {
             drawPixel (x0, y0, color);
          }

          err -= dy;

          IF (err < 0)
          {
             y0 += ystep;
             err += dx;
          }
       }
    }

    // Draw a rectangle
    VOID drawRect (int16 x, int16 y,
    INT16 w, int16 h,
    UNSIGNED int16 color)
    {
       drawFastHLine (x, y, w, color);
       drawFastHLine (x, y + h - 1, w, color);
       drawFastVLine (x, y, h, color);
       drawFastVLine (x + w - 1, y, h, color);
    }

    // ! VOID drawFastVLine (signed int16 x, signed int16 y,
    // ! int16 h, UNSIGNED int16 color) {
       // ! // Update in subclasses IF desired !
       // ! drawLine (x, y, x, y + h - 1, color);

       //!}

    //!
    // ! VOID drawFastHLine (signed int16 x, signed int16 y,
    // ! int16 w, UNSIGNED int16 color) {
       // ! // Update in subclasses IF desired !
       // ! drawLine (x, y, x + w - 1, y, color);

       //!}

    // ! VOID fillRect (int16 x, int16 y, int16 w, int16 h,
    // ! UNSIGNED int16 color){
       // ! // Update in subclasses IF desired !
       // ! for (INT16 i = x; i < x + w; i++){
          // ! drawFastVLine (i, y, h, color);

          
// !}

       //!}

    // ! VOID fillScreen (unsigned int16 color)
   // {
       // ! fillRect (0, 0, _width, _height, color);

       //!
    //}

    //!
    // Draw a rounded rectangle
    VOID drawRoundRect (int16 x, int16 y, int16 w,
    int16 h, int16 r, UNSIGNED int16 color)
    {
       // smarter version
       drawFastHLine (x + r, y, w - 2 * r, color) ; // Top
       drawFastHLine (x + r, y + h - 1, w - 2 * r, color); // Bottom
       drawFastVLine (x, y + r, h - 2 * r, color) ; // Left
       drawFastVLine (x + w - 1, y + r , h - 2 * r, color) ; // Right
       // draw four corners
       drawCircleHelper (x + r, y + r , r, 1, color);
       drawCircleHelper (x + w - r - 1, y + r , r, 2, color) ;
       drawCircleHelper (x + w - r - 1, y + h - r-1, r, 4, color);
       drawCircleHelper (x + r, y + h - r - 1, r, 8, color);
    }

    // Fill a rounded rectangle
    VOID fillRoundRect (int16 x, int16 y, int16 w,
    int16 h, int16 r, UNSIGNED int16 color)
    {
       // smarter version
       fillRect (x + r, y, w - 2 * r, h, color);

       // draw four corners
       fillCircleHelper (x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
       fillCircleHelper (x + r, y + r, r, 2, h-2 * r - 1, color) ;
    }

    // Draw a triangle
    VOID drawTriangle (int16 x0, int16 y0,
    INT16 x1, int16 y1,
    int16 x2, int16 y2, UNSIGNED int16 color)
    {
       drawLine (x0, y0, x1, y1, color);
       drawLine (x1, y1, x2, y2, color);
       drawLine (x2, y2, x0, y0, color);
    }

    // Fill a triangle
    VOID fillTriangle (int16 x0, int16 y0,
    INT16 x1, int16 y1,
    int16 x2, int16 y2, UNSIGNED int16 color)
    {
       INT16 a, b, y, last;

       // Sort coordinates by Y order (y2 >= y1 >= y0)
       IF (y0 > y1)
       {
          swap (y0, y1); swap (x0, x1);
       }

       IF (y1 > y2)
       {
          swap (y2, y1); swap (x2, x1);
       }

       IF (y0 > y1)
       {
          swap (y0, y1); swap (x0, x1);
       }

       IF (y0 == y2)
       {
          // Handle awkward all - on - same - line CASE as its ownthing
          a = b = x0;
          IF (x1 < a) a = x1;
          else IF (x1 > b) b = x1;
          IF (x2 < a) a = x2;
          else IF (x2 > b) b = x2;
          drawFastHLine (a, y0, b - a + 1, color);
          RETURN;
       }

       INT16
       dx01 = x1 - x0,
       dy01 = y1 - y0,
       dx02 = x2 - x0,
       dy02 = y2 - y0,
       dx12 = x2 - x1,
       dy12 = y2 - y1;

       INT32
       sa = 0,
       sb = 0;

       // FOR upper part of triangle, find scanline crossings for segments
       // 0 - 1 and 0 - 2. IF y1 = y2 (flat - bottomed triangle), the scanline y1
       // is included here (and second loop will be skipped, avoiding a / 0
       // error there), otherwise scanline y1 is skipped here and handled
       // in the second loop...which also avoids a / 0 error here IF y0 = y1
       // (flat - topped triangle) .
       IF (y1 == y2) last = y1; // Include y1 scanline
       ELSE last = y1 - 1; // Skip it
       FOR (y = y0; y <= last; y++)
       {
          a = x0 + sa / dy01;
          b = x0 + sb / dy02;
          sa += dx01;
          sb += dx02;

          /* longhand:
          a = x0 + (x1 - x0) * (y - y0) / (y1 - y0) ;
          b = x0 + (x2 - x0) * (y - y0) / (y2 - y0) ;

          */
          IF (a > b) swap (a, b) ;
          drawFastHLine (a, y, b - a + 1, color);
       }

       // FOR lower part of triangle, find scanline crossings for segments
       // 0 - 2 and 1 - 2. This loop is skipped IF y1 = y2.
       sa = dx12 * (y - y1);
       sb = dx02 * (y - y0);
       FOR (; y <= y2; y++)
       {
          a = x1 + sa / dy12;
          b = x0 + sb / dy02;
          sa += dx12;
          sb += dx02;

          /* longhand:
          a = x1 + (x2 - x1) * (y - y1) / (y2 - y1) ;
          b = x0 + (x2 - x0) * (y - y0) / (y2 - y0) ;

          */
          IF (a > b) swap (a, b) ;
          drawFastHLine (a, y, b - a + 1, color);
       }
    }

    VOID drawBitmap (int16 x, int16 y,
    UNSIGNED char * bitmap, int16 w, int16 h,
    UNSIGNED int16 color)
    {
       INT16 i, j, byteWidth = (w + 7) / 8;
       FOR (j = 0; j < h; j++)
       {
          FOR (i = 0; i < w; i++)
          {
             IF (pgm_read_byte (bitmap + j * byteWidth + i / 8)& (128 >> (i&
             7)))
             {
                drawPixel (x + i, y + j, color);
             }
          }
       }
    }

    // Draw a 1 - bit color bitmap at the specified x, y position from the
    // provided bitmap buffer (must be PROGMEM memory) using color as the
    // foreground color and bg as the background color.
    VOID drawBitmap (int16 x, int16 y,
    UNSIGNED char * bitmap, int16 w, int16 h,
    UNSIGNED int16 color, unsigned int16 bg)
    {
       INT16 i, j, byteWidth = (w + 7) / 8;
       
       FOR (j = 0; j < h; j++)
       {
          FOR (i = 0; i < w; i++)
          {
             IF (pgm_read_byte (bitmap + j * byteWidth + i / 8)& (128 >> (i&
             7)))
             {
                drawPixel (x + i, y + j, color);
             }

             ELSE
             {
                drawPixel (x + i, y + j, bg) ;
             }
          }
       }
    }

    //Draw XBitMap Files (*.xbm), exported from GIMP,
    //Usage: Export from GIMP to * .xbm, rename * .xbm to * .c and open in editor.
    //C Array can be directly used with this function
    VOID drawXBitmap (int16 x, int16 y,
    UNSIGNED char * bitmap, int16 w, int16 h,
    UNSIGNED int16 color)
    {
       
       INT16 i, j, byteWidth = (w + 7) / 8;
       
       FOR (j = 0; j < h; j++)
       {
          FOR (i = 0; i < w; i++)
          {
             IF (pgm_read_byte (bitmap + j * byteWidth + i / 8)& (1 << (i % 8)
             ))
             {
                drawPixel (x + i, y + j, color);
             }
          }
       }
    }

    VOID write (unsigned char c)
    {
       IF (c == '\f')
       {
          fillScreen (textbgcolor);
          }else IF (c == '\n') {
          cursor_y += textsize * 8;
          } else IF (c == '\r') {
          cursor_x = 0;
          }else IF (c == '\1'){
          textsize = 1;
          }else IF (c == '\2'){
          textsize = 2;
          }else IF (c == '\3'){
          textsize = 3;
          }else IF (c == '\4'){
          textsize = 4;
          }else IF (c == '\5'){
          textsize = 5;
          }else IF (c == '\6'){
          textsize = 6;
          }ELSE{
          drawChar (cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
          cursor_x += textsize * 6;

          IF (wrap&& (cursor_x > (_width - textsize * 6)) )
          {
             cursor_y += textsize * 8;
             cursor_x = 0;
          }
       }
    }

    // Draw a character
    VOID drawChar (signed int16 x, signed int16 y, unsigned char c,
    UNSIGNED int16 color, unsigned int16 bg, unsigned char size)
    {
       IF ( (x >= _width) ||// Clip right
       (y >= _height) ||// Clip bottom
       ( (x + 6 * size - 1) < 0) ||// Clip left
       ( (y + 8 * size - 1) < 0) ) // Clip top
       RETURN;
       IF ( ! _cp437&& (c >= 176)) c++; // Handle 'classic' charset behavior
       for (INT8 i = 0; i < 6; i++)
       {
          UNSIGNED char line;

          IF (i == 5)
             line = 0x0;

          ELSE
          line = font[c][i];
          for (INT8 j = 0; j < 8; j++)
          {
             IF (line&0x1)
             {
                IF (size == 1) // default size
                   drawPixel (x + i, y + j, color);

                ELSE
                {
                   // big size
                   fillRect (x + (i * size), y + (j*size), size, size, color);
                }

                } else IF (bg != color) {
                IF (size == 1) // default size
                   drawPixel (x + i, y + j, bg) ;

                ELSE
                {
                   // big size
                   fillRect (x + i * size, y + j * size, size, size, bg);
                }
             }

             line >>= 1;
          }
       }
    }

    VOID setCursor (signed int16 x, signed int16 y)
    {
       cursor_x = x;
       cursor_y = y;
    }

    int16 getCursorX (VOID)
    {
       RETURN cursor_x;
    }

    int16 getCursorY (VOID)
    {
       RETURN cursor_y;
    }

    VOID setTextSize (unsigned char s)
    {
       textsize = (s > 0) ? s : 1;
    }

    VOID setTextColor (unsigned int16 c)
    {
       // FOR 'transparent' background, we'll set the bg
       // to the same as fg instead of using a flag
       textcolor = textbgcolor = c;
    }

    VOID setTextColor (unsigned int16 c, unsigned int16 b)
    {
       textcolor = c;
       textbgcolor = b;
    }

    VOID setTextWrap (int1 w)
    {
       wrap = w;
    }

    unsigned char getRotation (VOID)
    {
       RETURN rotation;
    }

    VOID setRotation (unsigned char x)
    {
       rotation = (x&3);

       SWITCH (rotation)
       {
          CASE 0:
          CASE 2:
          _width = WIDTH;
          _height = HEIGHT;
          BREAK;

          CASE 1:
          CASE 3:
          _width = HEIGHT;
          _height = WIDTH;
          BREAK;
       }

       CS_ACTIVE;

       IF (driver == ID_9341)
       {
          // MEME, HX8357D uses same registers as 9341 but different values
          UNSIGNED int16 t;

          SWITCH (rotation)
          {
             CASE 2:
             t = ILI9341_MADCTL_MX|ILI9341_MADCTL_BGR;
             BREAK;

             CASE 3:
             t = ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR;
             BREAK;

             CASE 0:
             t = ILI9341_MADCTL_MY|ILI9341_MADCTL_BGR;
             BREAK;

             CASE 1:
             t = ILI9341_MADCTL_MX|ILI9341_MADCTL_MY|ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR;
             BREAK;
          }

          writeRegister8 (ILI9341_MADCTL, t); // MADCTL
          // FOR 9341, init default full - screen address window:
          setAddrWindow (0, 0, _width - 1, _height - 1) ; // CS_IDLE happens he
         
       }
    }

    // Enable (or disable) Code Page 437 - compatible charset.
    // There was an error in glcdfont.c FOR the longest time -- one character
    // (#176, the 'light shade' block) was missing -- this threw off the index
    // of every character that followed it. But a TON of code has been written
    // with the erroneous character indices. By DEFAULT, the library uses the
    // original 'wrong' behavior and old sketches will still work. Pass 'true'
    // to this function to use correct CP437 character values in your code.
    VOID cp437 (int1 x)
    {
       _cp437 = x;
    }

    // RETURN the size of the display (per current rotation)
    int16 width (VOID)
    {
       RETURN _width;
    }

    int16 height (VOID)
    {
       RETURN _height;
    }

    VOID invertDisplay (int1 i)
    {
       // Do nothing, must be subclassed IF supported
    }

    VOID TFTLCD_init (void)
    {
       _reset = RST_PIN;
       output_high (_reset);
       
       setWriteDir (); // Set up LCD data port (s) FOR WRITE operations
       rotation = 0;
       cursor_y = cursor_x = 0;
       textsize = 1;
       textcolor = BLACK;
       _width = TFTWIDTH;
       _height =TFTHEIGHT;
       
       begin (0x9341);
       setRotation (0);
       fillScreen (BLACK);
    }

    #define TFTLCD_DELAY 0xFF
    VOID begin (unsigned int16 id)
    {
       reset ();

       IF (id == 0x9341)
       {
          
          driver = ID_9341;
          CS_ACTIVE;
          writeRegister8 (ILI9341_SOFTRESET, 0);
          writeRegister8 (ILI9341_DISPLAYOFF, 0);
          writeRegister8 (ILI9341_POWERCONTROL1, 0x23);
          writeRegister8 (ILI9341_POWERCONTROL2, 0x10);
          writeRegister16 (ILI9341_VCOMCONTROL1, 0x2B2B); 
          writeRegister8(ILI9341_VCOMCONTROL2, 0xC0); 
          writeRegister8 (ILI9341_MEMCONTROL, ILI9341_MADCTL_MY|ILI9341_MADCTL_BGR);
          writeRegister8 (ILI9341_PIXELFORMAT, 0x55);
          writeRegister16 (ILI9341_FRAMECONTROL, 0x001B);
  
          writeRegister8 (ILI9341_ENTRYMODE, 0x07);
          /* writeRegister32 (ILI9341_DISPLAYFUNC, 0x0A822700); */
          writeRegister8 (ILI9341_SLEEPOUT, 0);
          writeRegister8 (ILI9341_DISPLAYON, 0);
          writeRegister8 (ILI9341_INVERTON, 0x00);
          setAddrWindow (0, 0, TFTWIDTH - 1, TFTHEIGHT - 1) ;
       }
    }

    VOID reset (void)
    {
       CS_IDLE;
       // CD_DATA;
       WR_IDLE;
       OUTPUT_LOW (_reset);
       delay_ms (2);
       output_high(_reset);

       // Data transfer sync
       CS_ACTIVE;
       CD_COMMAND;
       write8 (0x00);
       for (UNSIGNED char i = 0; i < 3; i++) WR_STROBE; // Three extra 0x00s
       CS_IDLE;
    }

    VOID setAddrWindow (signed int16 x1, signed int16 y1, signed int16 x2, signed INT16 y2)
    {
       CS_ACTIVE;

       IF ((driver == ID_9341) || (driver == ID_HX8357D))
       {
          UNSIGNED int32 t;
          t = x1;
          t <<= 16;
          t |= x2;
          writeRegister32 (ILI9341_COLADDRSET, t); // HX8357D uses same registe
         
          t = y1;
          t <<= 16;
          t|= y2;
          writeRegister32 (ILI9341_PAGEADDRSET, t); // HX8357D uses same regist
   
       }

       CS_IDLE;
    }

    VOID setLR (void)
    {
       CS_ACTIVE;

       // ! writeRegisterPair (HX8347G_COLADDREND_HI, HX8347G_COLADDREND_LO, _width - 1);

       // ! writeRegisterPair (HX8347G_ROWADDREND_HI, HX8347G_ROWADDREND_LO, _height - 1);
       CS_IDLE;
    }

    VOID flood (unsigned int16 color, unsigned int32 len)
    {
       UNSIGNED int16 blocks;

       UNSIGNED char i, hi = color >> 8,
       lo = color;
       CS_ACTIVE;
       CD_COMMAND;

       IF (driver == ID_9341)
       {
          write8 (0x2C);
          } ELSE {
          write8 (0x22); // Write data to GRAM
       }

       // Write first pixel normally, decrement counter by 1
       CD_DATA;
       write8 (hi);
       write8 (lo);
       len--;
       blocks = (UNSIGNED int16) (len / 64); // 64 pixels / block
       IF (hi == lo)
       {
          // High and low bytes are identical. Leave prior data
          // on the port (s) and just toggle the write strobe.
          WHILE (blocks--)
          {
             i = 16; // 64 pixels / block / 4 pixels / pass
             DO
             {
                WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // 2 bytes / pixel
                WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // x 4 pixels
             } WHILE (--i);
          }

          // Fill any remaininpixels (1 to 64)
          for (i = (UNSIGNED char) len&63; i--; )
          {
             WR_STROBE;
             WR_STROBE;
          }

          } ELSE {
          WHILE (blocks--)
          {
             i = 16; // 64 pixels / block / 4 pixels / pass
             DO
             {
                write8 (hi); write8 (lo); write8 (hi); write8 (lo) ;
                write8 (hi); write8 (lo); write8 (hi); write8 (lo) ;
             } WHILE (--i);
          }

          for (i = (UNSIGNED char) len&63; i--; )
          {
             write8 (hi);
             write8 (lo);
          }
       }

       CS_IDLE;
    }

    VOID drawFastHLine (signed int16 x, signed int16 y, int16 length, unsigned
    INT16 color)
    {
       SIGNED int16 x2 = (x + length - 1);

       // Initial off - screen clipping
       IF ( (length <= 0 )|| (y < 0) || (y >= _height) || (x >= _width)|| (
       x2 < 0)) RETURN;

       IF (x < 0)
       {
          // Clip left
          length += x;
          x = 0;
       }

       IF (x2 >= _width)
       {
          // Clip right
          x2 = _width - 1;
          length = x2 - x + 1;
       }

       setAddrWindow (x, y, x2, y);
       flood (color, length);
       IF (driver == ID_932X) setAddrWindow (0, 0, _width - 1, _height - 1);
       ELSE setLR ();
    }

    VOID drawFastVLine (signed int16 x, signed int16 y, int16 length,
    UNSIGNED int16 color)
    {
       SIGNED int16 y2;

       //#ignore_warnings 201, 203, 204, 205
       // Initial off - screen clipping
       y2 = (y + length - 1);

       IF ( (length <= 0)|| (x < 0) || (x >= _width)|| (y >= _height)|| (y2
       < 0)) RETURN;

       //#ignore_warnings none
       IF (y < 0)
       {
          // Clip top
          length += y;
          y = 0;
       }

       IF (y2 >= _height)
       {
          // Clip bottom
          y2 = _height - 1;
          length = y2 - y + 1;
       }

       setAddrWindow (x, y, x, y2);
       flood (color, length);
       IF (driver == ID_932X) setAddrWindow (0, 0, _width - 1, _height - 1);
       ELSE setLR ();
    }

    VOID fillRect (signed int16 x1, signed int16 y1, signed int16 w, signed int16 h,
    UNSIGNED int16 fillcolor)
    {
       SIGNED int16 x2, y2;
       x2 = x1 + w - 1;
       y2 = y1 + h - 1;

       // Initial off - screen clipping
       IF ((w <= 0 )|| (h <= 0)|| (x1 >= _width)|| (y1 >= _height) || (x2 <
       0)|| (y2 < 0)) RETURN;

       IF (x1 < 0)
       {
          // Clip left
          w += x1;
          x1 = 0;
       }

       IF (y1 < 0)
       {
          // Clip top
          h += y1;
          y1 = 0;
       }

       IF (x2 >= _width)
       {
          // Clip right
          x2 = _width - 1;
          w = x2 - x1 + 1;
       }

       IF (y2 >= _height)
       {
          // Clip bottom
          y2 = _height - 1;
          h = y2 - y1 + 1;
       }

       setAddrWindow (x1, y1, x2, y2);
       flood (fillcolor, (UNSIGNED int32) w * (unsigned int32) h);
    }

   
    VOID drawPixel (signed int16 x, signed int16 y, unsigned int16 color)
    {
       // Clip
       if ( (x < 0) || (y < 0) || (x >= _width) || (y >= _height) ) RETURN;
       CS_ACTIVE;

       IF ((driver == ID_9341) || (driver == ID_HX8357D))
       {
          setAddrWindow (x, y, _width - 1, _height - 1) ;
          CS_ACTIVE;
          CD_COMMAND;
          write8 (0x2C);
          CD_DATA;
          write8 (color >> 8); write8 (color);
       }

       CS_IDLE;
    }

    VOID pushColors (unsigned int16 * data, unsigned char len, int1 first)
    {
       UNSIGNED int16 color;
       UNSIGNED char hi, lo;
       CS_ACTIVE;

       IF (first == true)
       {
          // Issue GRAM write command only on first call
          CD_COMMAND;

          IF ((driver == ID_9341) || (driver == ID_HX8357D))
          {
             write8 (0x2C);
             } ELSE{
             write8 (0x22);
          }
       }

       CD_DATA;

       WHILE (len--)
       {
          color = *data++;
          hi = color >> 8; // Don't simplify or merge these
          lo = color; // lines, there's macro shenanigans
          write8 (hi); // going on.
          write8 (lo);
       }

       CS_IDLE;
    }

    #ifndef read8
    unsigned char read8fn (VOID);

    #define read8isFunctionalized
    #endif
    #ifdef read8isFunctionalized
    #define read8 (x) x = read8fn ()
    #endif
    UNSIGNED int16 readPixel (signed int16 x, signed int16 y)
    {
       if ( (x < 0)|| (y < 0) || (x >= _width) || (y >= _height) ) RETURN 0;
       CS_ACTIVE;
    }

    unsigned int16 readID (VOID)
    {
       // *** SPFD5408 change -- Begin
       RETURN 0x9341;
    }

    UNSIGNED int32 readReg (unsigned char r)
    {
       UNSIGNED int32 id;
       UNSIGNED char x;

       // try reading REGISTER #4
       CS_ACTIVE;
       CD_COMMAND;
       write8 (r);
       setReadDir (); // Set up LCD data port (s) FOR READ operations
       CD_DATA;
       delay_us (50);
       read8 (x);
       id = x; // DO not merge or otherwise simplify
       id <<= 8; // these lines. It's an unfortunate
       read8 (x);
       id |= x; // shenanigans that are going on.
       id <<= 8; // these lines. It's an unfortunate
       read8 (x);
       id |= x; // shenanigans that are going on.
       id <<= 8; // these lines. It's an unfortunate
       read8 (x);
       id |= x; // shenanigans that are going on.
       CS_IDLE;
       setWriteDir (); // Restore LCD data port (s) to WRITE configuration
       RETURN id;
    }

    // Pass 8 - bit (each) R,G, B, get back 16 - bit packed color
    UNSIGNED int16 color565 (unsigned char r, unsigned char g, unsigned char b)
    {
       RETURN ((r&0xF8) << 8)| ((g&0xFC) << 3)| (b >> 3);
    }

    #ifndef write8
    VOID write8 (unsigned char value)
    {
       write8inline (value);
    }

    #endif
    #ifdef read8isFunctionalized
    unsigned char read8fn (VOID)
    {
       UNSIGNED char result;
       read8inline (result);
       RETURN result;
    }

    #endif
    #ifndef setWriteDir
    VOID setWriteDir (void)
    {
       setWriteDirInline ();
    }

    #endif
    #ifndef setReadDir
    VOID setReadDir (void)
    {
       setReadDirInline ();
    }

    #endif
    #ifndef writeRegister8
    VOID writeRegister8 (unsigned char a, unsigned char d)
    {
       writeRegister8inline (a, d);
    }

    #endif
    #ifndef writeRegister16
    VOID writeRegister16 (unsigned int16 a, unsigned int16 d)
    {
       writeRegister16inline (a, d);
    }

    #endif
    #ifndef writeRegisterPair
    VOID writeRegisterPair (unsigned char aH, unsigned char aL, unsigned int16 d
    )
    {
       writeRegisterPairInline (aH, aL, d);
    }

    #endif
    VOID writeRegister24 (unsigned char r, unsigned int32 d)
    {
       CS_ACTIVE;
       CD_COMMAND;
       write8 (r);
       CD_DATA;
       delay_us (10);
       write8 (d >> 16);
       delay_us (10);
       write8 (d >> 8);
       delay_us (10);
       write8 (d);
       CS_IDLE;
    }

    VOID writeRegister32 (unsigned char r, unsigned int32 d)
    {
       CS_ACTIVE;
       CD_COMMAND;
       write8 (r);
       CD_DATA;
       delay_us (10);
       write8 (d >> 24);
       delay_us (10);
       write8 (d >> 16);
       delay_us (10);
       write8 (d >> 8);
       delay_us (10);
       write8 (d);
       CS_IDLE;
    }

    //#ignore_warnings none
