/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
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
  and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS for A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE for ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN if ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
Libreria TFTimportada de Arduino By MICHAEL VARGAS para su uso inicial
con la TFT Shield Arduino de  2.4' 320x2 
 Thanks to:
 Modified for SPFD5408 by Joao Lopes to work with SPFD5408
*/
#define tft_putc  write
#include <math.h>
#include <stdlib.h> // for atoi32
#include "glcdfont.c"
#include "pin_magic.h"
#include "registers.h"
#define pgm_read_byte(addr) (addr)
#define pgm_read_word(addr) ((addr))
#ifndef min
 #define min(a, b)((a<b)? a : b)
#endif
#define swap(a, b) { int16 t = a; a = b; b = t; }
#define TFTWIDTH   240
#define TFTHEIGHT  320

// LCD controller chip identifiers
#define ID_932X    0
#define ID_7575    1
#define ID_9341    2
#define ID_HX8357D    3
#define ID_UNKNOWN 0xFF
#define FillScreen(color)  fillRect(0, 0, _width, _height, color)

    void Adafruit_GFX(int16 w, int16 h); // Constructor
  //These MAY be overridden by the subclass to provide device-specific
  //optimized code. Otherwise 'generic' versions are used.
    void drawLine(int16 x0, int16 y0, int16 x1, int16 y1, unsigned int16 color);
    void drawFastVLine(signed int16 x, signed int16 y, int16 h, unsigned int16 color);
    void drawFastHLine(signed int16 x, signed int16 y, int16 w, unsigned int16 color);
    void drawRect(signed int16 x, signed int16 y, int16 w, int16 h, unsigned int16 color);
    void fillRect(signed int16 x1, signed int16 y1, signed int16 w, signed int16 h, unsigned int16 fillcolor);
    void invertDisplay(int1 i);

  //These exist only with Adafruit_GFX(no subclass overrides)
   void drawCircle(int16 x0, int16 y0, int16 r, unsigned int16 color);
   void drawCircleHelper(int16 x0, int16 y0, int16 r, unsigned charcornername, unsigned int16 color);
   void fillCircle(int16 x0, int16 y0, int16 r, unsigned int16 color);
   void fillCircleHelper(int16 x0, int16 y0, int16 r, unsigned char cornername, int16 delta, unsigned int16 color);
   void drawTriangle(int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, unsigned int16 color);
   void fillTriangle(int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, unsigned int16 color);

   void drawRoundRect(int16 x0, int16 y0, int16 w, int16 h,
   int16 radius, unsigned int16 color);
   void fillRoundRect(int16 x0, int16 y0, int16 w, int16 h, int16 radius, unsigned int16 color);
   void drawBitmap(signed int16 x, signed int16 y, unsigned char*bitmap, int16 w, int16 h, unsigned int16 color);
   void drawBitmap(signed int16 x, signed int16 y, unsigned char*bitmap, int16 w, int16 h, unsigned int16 color, unsigned int16 bg);
   void drawXBitmap(signed int16 x, signed int16 y, unsigned char*bitmap, int16 w, int16 h, unsigned int16 color);

   void drawChar(signed int16 x, signed int16 y, unsigned char c, unsigned int16 color,
   unsigned int16 bg, unsigned char size);
   void setCursor(int16 x, int16 y);
   void setTextColor(unsigned int16 c);
   void setTextColor(unsigned int16 c, unsigned int16 bg);
   void setTextSize(unsigned char s);
   void setTextWrap(int1 w);
   void setRotation(unsigned char r);
   void cp437(int1 x=true);
   void write(unsigned char);
  void begin(unsigned int16 id);
  void drawPixel(signed int16 x, signed int16 y, unsigned int16 color);
 //void drawFastVLine(int16 x0, int16 y0, int16 h, unsigned int16 color);
  //void fillRect(signed int16 x, signed int16 y, int16 w, int16 h, unsigned int16 c);
  //void fillScreen(unsigned int16 color);
  void reset(void);
  void setRegisters8(unsigned char*ptr, unsigned char n);
  void setRegisters16(unsigned int16*ptr, unsigned char n);
  void setAddrWindow(signed int16 x1, signed int16 y1, signed int16 x2, signed int16 y2);
  void pushColors(unsigned int16*data, unsigned char len, int1 first);
  unsigned int16 color565(unsigned char r, unsigned char g, unsigned char b);
  unsigned int16 readPixel(int16 x, int16 y);
  unsigned int16 readID(void);
  unsigned int32 readReg(unsigned char r);
  void TFTLCD_init();
  void setLR(void);
  void flood(unsigned int16 color, unsigned int32 len);
  void writeRegister24(unsigned char a, unsigned int32 d);
  void writeRegister32(unsigned char a, unsigned int32 d);
  
  unsigned char driver;
  unsigned int16 _reset;
  unsigned char getRotation();

  //get current cursor position(get rotation safe maximum values, using: width()for x, height()for y)
  int16 getCursorX(void);
  int16 getCursorY(void);
  const signed int16 WIDTH=TFTWIDTH, HEIGHT=TFTHEIGHT;//This is the 'raw' display w/h-never changes
  signed int16 _width=TFTWIDTH, _height=TFTHEIGHT, //Display w/h as modified by current rotation
    cursor_x=0, cursor_y=0;

  unsigned int16
    textcolor, textbgcolor;

  unsigned char
    textsize,
    rotation;

  int1
    wrap,//if set, 'wrap' text at right edge of display
    _cp437;//if set, use correct CP437 charset(default is off)
void Adafruit_GFX(int16 w, int16 h)
{
   //!  WIDTH(w), HEIGHT(h);
   _width=w;
   _height=h;
   rotation=0;
   cursor_y=cursor_x=0;
   textsize=1;
   textcolor=textbgcolor=0xFFFF;
   wrap = true;
   _cp437=false;
}

// Draw a circle outline
void drawCircle(int16 x0, int16 y0, int16 r,
    unsigned int16 color)
    {
       signed int16 f=1-r;
       int16 ddF_x=1;
       int16 ddF_y=-2*r;
       int16 x=0;
       int16 y=r;
       drawPixel(x0, y0+r, color);
       drawPixel(x0, y0-r, color);
       drawPixel(x0+r, y0, color);
       drawPixel(x0-r, y0, color);

       while(x<y)
       {
          if(f>=0)
          {
             y--;
             ddF_y+=2;
             f+=ddF_y;
          }

          x++;
          ddF_x+=2;
          f+=ddF_x;
          
          drawPixel(x0+x, y0+y,  color);
          drawPixel(x0-x, y0+y,  color);
          drawPixel(x0+x, y0-y,  color);
          drawPixel(x0-x, y0-y,  color);
          drawPixel(x0+y, y0+x,  color);
          drawPixel(x0-y, y0+x,  color);
          drawPixel(x0+y, y0-x,  color);
          drawPixel(x0-y, y0-x,  color);
       }
    }

    void drawCircleHelper( int16 x0, int16 y0,
    int16 r, unsigned char cornername, unsigned int16 color)
    {
       signed int16 f = 1-r;
       int16 ddF_x=1;
       int16 ddF_y=-2*r;
       int16 x = 0;
       int16 y = r;

       while(x<y)
       {
          if(f>=0)
          {
             y--;
             ddF_y+=2;
             f+=ddF_y;
          }

          x++;
          ddF_x+=2;
          f+=ddF_x;

          if(cornername & 0x4)
          {
             drawPixel(x0+x, y0+y,  color);
             drawPixel(x0+y, y0+x,  color);
          }

          if(cornername & 0x2)
          {
             drawPixel(x0+x, y0-y,  color);
             drawPixel(x0+y, y0-x,  color);
          }

          if(cornername & 0x8)
          {
             drawPixel(x0-y, y0+x,  color);
             drawPixel(x0-x, y0+y,  color);
          }

          if(cornername & 0x1)
          {
             drawPixel(x0-y, y0-x,  color);
             drawPixel(x0-x, y0-y,  color);
          }
       }
    }

    void fillCircle(int16 x0, int16 y0, int16 r,
    unsigned int16 color)
    {
       drawFastVLine(x0, y0-r, 2*r+1, color);
       fillCircleHelper(x0, y0, r, 3, 0, color);
    }

    // Used to do circles and roundrects
    void fillCircleHelper(int16 x0, int16 y0, int16 r,
    unsigned char cornername, int16 delta, unsigned int16 color)
    {
       signed int16 f = 1-r;
       signed int16 ddF_x=1;
       signed int16 ddF_y=-2*r;
       signed int16 x = 0;
       signed int16 y = r;

       while(x<y)
       {
          if(f>=0)
          {
             y--;
             ddF_y+=2;
             f+=ddF_y;
          }

          x++;
          ddF_x+=2;
          f+=ddF_x;

          if(cornername & 0x1)
          {
             drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
             drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
          }

          if(cornername & 0x2)
          {
             drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
             drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
          }
       }
    }

    // Bresenham's algorithm - thx wikpedia
    void drawLine(int16 x0, int16 y0,
    int16 x1, int16 y1,
    unsigned int16 color)
    {
       signed int16 steep=abs(y1-y0)>abs(x1-x0);

       if(steep)
       {
          swap(x0, y0);
          swap(x1, y1);
       }

       if(x0>x1)
       {
          swap(x0, x1);
          swap(y0, y1);
       }

       signed int16 dx, dy;
       dx=x1-x0;
       dy=abs(y1-y0);
       signed int16 err=dx/2;
       signed int16 ystep;

       if(y0<y1)
       {
          ystep=1;
          } else {
          ystep=-1;
       }

       for(;x0<=x1;x0++)
       {
          if(steep)
          {
             drawPixel(y0, x0, color);
             } else {
             drawPixel(x0, y0, color);
          }

          err-=dy;

          if(err<0)
          {
             y0+=ystep;
             err+=dx;
          }
       }
    }

    // Draw a rectangle
    void drawRect(signed int16 x, signed int16 y,
    int16 w, int16 h,
    unsigned int16 color)
    {
       drawFastHLine(x, y, w, color);
       drawFastHLine(x, y+h-1, w, color);
       drawFastVLine(x, y, h, color);
       drawFastVLine(x+w-1, y, h, color);
    }

    //!void drawFastVLine(signed int16 x, signed int16 y,
    //!int16 h,  unsigned int16 color)
    //
       //!  // Update in subclasses if desired!
       //!  drawLine(x, y, x, y+h-1, color);

       //!
   //}

    //!
    //!void drawFastHLine(signed int16 x, signed int16 y,
    //!int16 w,  unsigned int16 color)
    //{
       //!  // Update in subclasses if desired!
       //!  drawLine(x, y, x+w-1, y, color);

       //!
//    }

    //!void fillRect(signed int16 x, signed int16 y, int16 w, int16 h,
    //!unsigned int16 color)
 //  {
       //!  // Update in subclasses if desired!
       //!for(int16 i=x;i<x+w;i++)
 //    {
          //!    drawFastVLine(i, y, h, color);

          //!
    //7   }

       //!
    //}

    //!void fillScreen(unsigned int16 color)
  //  {
       //!  fillRect(0, 0, _width, _height, color);

       //!
   // }

    //!
    // Draw a rounded rectangle
    void drawRoundRect(signed int16 x, signed int16 y, int16 w,
    int16 h, int16 r, unsigned int16 color)
    {
       //smarter version
       drawFastHLine(x+r, y , w-2*r, color);//Top
       drawFastHLine(x+r, y+h-1, w-2*r, color);//Bottom
       drawFastVLine(x, y+r, h-2*r, color);//Left
       drawFastVLine(x+w-1, y+r, h-2*r, color);//Right
       //draw four corners
       drawCircleHelper(x+r, y+r, r, 1, color);
       drawCircleHelper(x+w-r-1, y+r, r, 2, color);
       drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
       drawCircleHelper(x+r, y+h-r-1, r, 8, color);
    }

    // Fill a rounded rectangle
    void fillRoundRect(signed int16 x, signed int16 y, int16 w,
    int16 h, int16 r, unsigned int16 color)
    {
       //smarter version
       fillRect(x+r, y, w-2*r, h, color);

       //draw four corners
       fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
       fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
    }

    // Draw a triangle
    void drawTriangle(int16 x0, int16 y0,
    int16 x1, int16 y1,
    int16 x2, int16 y2, unsigned int16 color)
    {
       drawLine(x0, y0, x1, y1, color);
       drawLine(x1, y1, x2, y2, color);
       drawLine(x2, y2, x0, y0, color);
    }

    // Fill a triangle
    void fillTriangle ( int16 x0, int16 y0,
    int16 x1, int16 y1,
    int16 x2, int16 y2, unsigned int16 color)
    {
       int16 a, b, y, last;

       //Sort coordinates by Y order(y2>=y1>=y0)
       if(y0>y1)
       {
          swap(y0, y1);swap(x0, x1);
       }

       if(y1>y2)
       {
          swap(y2, y1);swap(x2, x1);
       }

       if(y0>y1)
       {
          swap(y0, y1);swap(x0, x1);
       }

       if(y0==y2)
       {
          //Handle awkward all-on-same-line case as its own thing
          a=b=x0;
          if(x1<a)a=x1;
          else if(x1>b)b=x1;
          if(x2<a)a=x2;
          else if(x2>b)b=x2;
          drawFastHLine(a, y0, b-a+1, color);
          return;
       }

       int16
       dx01=x1-x0,
       dy01=y1-y0,
       dx02=x2-x0,
       dy02=y2-y0,
       dx12=x2-x1,
       dy12=y2-y1;

       int32
       sa=0,
       sb=0;

       //for upper part of triangle, find scanline crossings for segments
       //0-1 and 0-2. if y1=y2(flat-bottomed triangle), the scanline y1
       //is included here(and second loop will be skipped, avoiding a/0
       //error there), otherwise scanline y1 is skipped here and handled
       //in the second loop...which also avoids a/0 error here if y0=y1
       //(flat-topped triangle).
       if(y1==y2)last=y1;//Include y1 scanline
       else  last=y1-1;//Skip it
       for(y=y0;y<=last;y++)
       {
          a=x0+sa/dy01;
          b=x0+sb/dy02;
          sa+=dx01;
          sb+=dx02;

          /*longhand:
          a=x0+(x1-x0)*(y-y0)/(y1-y0);
          b=x0+(x2-x0)*(y-y0)/(y2-y0);

          */
          if(a>b)swap(a, b);
          drawFastHLine(a, y, b-a+1, color);
       }

       //for lower part of triangle, find scanline crossings for segments
       //0-2 and 1-2. This loop is skipped if y1=y2.
       sa=dx12*(y-y1);
       sb=dx02*(y-y0);
       for(;y<=y2;y++)
       {
          a=x1+sa/dy12;
          b=x0+sb/dy02;
          sa+=dx12;
          sb+=dx02;

          /*longhand:
          a=x1+(x2-x1)*(y-y1)/(y2-y1);
          b=x0+(x2-x0)*(y-y0)/(y2-y0);

          */
          if(a>b)swap(a, b);
          drawFastHLine(a, y, b-a+1, color);
       }
    }

    void drawBitmap(signed int16 x, signed int16 y,
    unsigned char*bitmap, int16 w, int16 h,
    unsigned int16 color)
    {
       int16 i, j, byteWidth=(w+7)/8;
       for(j=0;j<h;j++)
       {
          for(i=0;i<w;i++)
          {
             if(pgm_read_byte(bitmap+j*byteWidth+i/8) &(128>>(i & 7)))
             {
                drawPixel(x+i, y+j, color);
             }
          }
       }
    }

    // Draw a 1-bit color bitmap at the specified x, y position from the
    // provided bitmap buffer (must be PROGMEM memory) using color as the
    // foreground color and bg as the background color.
    void drawBitmap(signed int16 x, signed int16 y,
    unsigned char*bitmap, int16 w, int16 h,
    unsigned int16 color, unsigned int16 bg)
    {
       int16 i, j, byteWidth=(w+7)/8;
       
       for(j=0;j<h;j++)
       {
          for(i=0;i<w;i++)
          {
             if(pgm_read_byte(bitmap+j*byteWidth+i/8) &(128>>(i & 7)))
             {
                drawPixel(x+i, y+j, color);
             }

             else
             {
                drawPixel(x+i, y+j, bg);
             }
          }
       }
    }

    //Draw XBitMap Files (*.xbm), exported from GIMP,
    //Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
    //C Array can be directly used with this function
    void drawXBitmap(signed int16 x, signed int16 y,
    unsigned char*bitmap, int16 w, int16 h,
    unsigned int16 color)
    {
       
       int16 i, j, byteWidth=(w+7)/8;
       
       for(j=0;j<h;j++)
       {
          for(i=0;i<w;i++)
          {
             if(pgm_read_byte(bitmap+j*byteWidth+i/8) &(1<<(i%8)))
             {
                drawPixel(x+i, y+j, color);
             }
          }
       }
    }

    void write(unsigned char c)
    {
       if(c=='\f')
       {
          fillScreen(textbgcolor);
          }else if(c=='\n'){
          cursor_y+=textsize*8;
          } else if(c=='\r'){
          cursor_x=0;
          }else if(c=='\1'){
          textsize=1;
          }else if(c=='\2'){
          textsize=2;
          }else if(c=='\3'){
          textsize=3;
          }else if(c=='\4'){
          textsize=4;
          }else if(c=='\5'){
          textsize=5;
          }else if(c=='\6'){
          textsize=6;
          }else{
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
          cursor_x+=textsize*6;

          if(wrap &&(cursor_x>(_width-textsize*6)))
          {
             cursor_y+=textsize*8;
             cursor_x=0;
          }
       }
    }

    // Draw a character
    void drawChar(signed int16 x, signed int16 y, unsigned char c,
    unsigned int16 color, unsigned int16 bg, unsigned char size)
    {
       if((x>=_width) || //Clip right
       (y>=_height) || //Clip bottom
       ((x+6*size-1)<0) || //Clip left
       ((y+8*size-1)<0))//Clip top
       return;
       if(!_cp437 &&(c>=176))c++;//Handle 'classic' charset behavior
       for(int8 i=0;i<6;i++)
       {
          unsigned char line;

          if(i==5)
             line=0x0;

          else
          line=font[c][i];
          for(int8 j=0;j<8;j++)
          {
             if(line & 0x1)
             {
                if(size==1)//default size
                   drawPixel(x+i, y+j, color);

                else
                {
                   //big size
                   fillRect(x+(i*size), y+(j*size), size, size, color);
                }

                } else if(bg!=color){
                if(size==1)//default size
                   drawPixel(x+i, y+j, bg);

                else
                {
                   //big size
                   fillRect(x+i*size, y+j*size, size, size, bg);
                }
             }

             line>>=1;
          }
       }
    }

    void setCursor(int16 x, int16 y)
    {
       cursor_x=x;
       cursor_y=y;
    }

    int16 getCursorX(void)
    {
       return cursor_x;
    }

    int16 getCursorY(void)
    {
       return cursor_y;
    }

    void setTextSize(unsigned char s)
    {
       textsize=(s>0)? s : 1;
    }

    void setTextColor(unsigned int16 c)
    {
       //for 'transparent' background, we'll set the bg
       //to the same as fg instead of using a flag
       textcolor=textbgcolor=c;
    }

    void setTextColor(unsigned int16 c, unsigned int16 b)
    {
       textcolor=c;
       textbgcolor=b;
    }

    void setTextWrap(int1 w)
    {
       wrap=w;
    }

    unsigned char getRotation(void)
    {
       return rotation;
    }

    void setRotation(unsigned char x)
    {
       rotation=(x  & 3);

       switch(rotation)
       {
          case 0:
          case 2:
          _width=WIDTH;
          _height=HEIGHT;
          break;

          case 1:
          case 3:
          _width=HEIGHT;
          _height=WIDTH;
          break;
       }

       CS_ACTIVE;

       if(driver==ID_9341)
       {
          //MEME, HX8357D uses same registers as 9341 but different values
          unsigned int16 t;

          switch(rotation)
          {
             case 2:
             t=ILI9341_MADCTL_MX  | ILI9341_MADCTL_BGR;
             break;

             case 3:
             t=ILI9341_MADCTL_MV  | ILI9341_MADCTL_BGR;
             break;

             case 0:
             t=ILI9341_MADCTL_MY  | ILI9341_MADCTL_BGR;
             break;

             case 1:
             t=ILI9341_MADCTL_MX  | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
             break;
          }

          writeRegister8(ILI9341_MADCTL, t);//MADCTL
          //for 9341, init default full-screen address window:
          setAddrWindow(0, 0, _width-1, _height-1);//CS_IDLE happens here
       }

    }

    // Enable (or disable) Code Page 437-compatible charset.
    // There was an error in glcdfont.c for the longest time -- one character
    // (#176, the 'light shade' block) was missing -- this threw off the index
    // of every character that followed it.  But a TON of code has been written
    // with the erroneous character indices.  By default, the library uses the
    // original 'wrong' behavior and old sketches will still work.  Pass 'true'
    // to this function to use correct CP437 character values in your code.
    void cp437(int1 x)
    {
       _cp437=x;
    }

    // return the size of the display (per current rotation)
    int16 width(void)
    {
       return _width;
    }

    int16 height(void)
    {
       return _height;
    }

    void invertDisplay(int1 i)
    {
       //Do nothing, must be subclassed if supported
    }

    void TFTLCD_init(void)
    {
       _reset = RST_PIN;
       output_high(_reset);
       
       setWriteDir();//Set up LCD data port(s)for WRITE operations
       rotation=0;
       cursor_y=cursor_x=0;
       textsize=1;
       textcolor=0xFFFF;
       _width=TFTWIDTH;
       _height=TFTHEIGHT;
       begin(0x9341);
       setRotation(0);
       fillScreen(BLACK);
    }

    #define TFTLCD_DELAY 0xFF

    void begin(unsigned int16 id)
    {
       reset();

       if(id==0x9341)
       {
          
          driver=ID_9341;
          CS_ACTIVE;
          writeRegister8(ILI9341_SOFTRESET, 0);
          writeRegister8(ILI9341_DISPLAYOFF, 0);
          writeRegister8(ILI9341_POWERCONTROL1, 0x23);
          writeRegister8(ILI9341_POWERCONTROL2, 0x10);
          writeRegister16(ILI9341_VCOMCONTROL1, 0x2B2B);
          writeRegister8(ILI9341_VCOMCONTROL2, 0xC0);
          writeRegister8(ILI9341_MEMCONTROL, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
          writeRegister8(ILI9341_PIXELFORMAT, 0x55);
          writeRegister16(ILI9341_FRAMECONTROL, 0x001B);
          
          writeRegister8(ILI9341_ENTRYMODE, 0x07);
          /*writeRegister32(ILI9341_DISPLAYFUNC, 0x0A822700);*/
          writeRegister8(ILI9341_SLEEPOUT, 0);
          writeRegister8(ILI9341_DISPLAYON, 0);
          writeRegister8(ILI9341_INVERTON, 0x00);
          setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
       }
    }

    void reset(void)
    {
       CS_IDLE;
       //  CD_DATA;
       WR_IDLE;
       RD_IDLE;
       output_low(_reset);
       delay_ms(2);
       output_high(_reset);

       //Data transfer sync
       CS_ACTIVE;
       CD_COMMAND;
       write8(0x00);
       for(unsigned char i=0;i<3;i++)WR_STROBE;//Three extra 0x00s
       CS_IDLE;
    }

    void setAddrWindow(signed int16 x1, signed int16 y1, signed int16 x2, signed int16 y2)
    {
       CS_ACTIVE;

       if((driver==ID_9341) ||(driver==ID_HX8357D))
       {
          unsigned int32 t;
          t=x1;
          t<<=16;
          t |= x2;
          writeRegister32(ILI9341_COLADDRSET, t);//HX8357D uses same registers!
          t=y1;
          t<<=16;
          t |= y2;
          writeRegister32(ILI9341_PAGEADDRSET, t);//HX8357D uses same registers!
       }

       CS_IDLE;
    }

    void setLR(void)
    {
       CS_ACTIVE;
       //!  writeRegisterPair(HX8347G_COLADDREND_HI, HX8347G_COLADDREND_LO, _width  - 1);
       //!  writeRegisterPair(HX8347G_ROWADDREND_HI, HX8347G_ROWADDREND_LO, _height - 1);
       CS_IDLE;
    }

    void flood(unsigned int16 color, unsigned int32 len)
    {
       unsigned int16 blocks;

       unsigned char i, hi=color>>8,
       lo=color;
       CS_ACTIVE;
       CD_COMMAND;

       if(driver==ID_9341)
       {
          write8(0x2C);
          } else {
          write8(0x22);//Write data to GRAM
       }

       //Write first pixel normally, decrement counter by 1
       CD_DATA;
       write8(hi);
       write8(lo);
       len--;
       blocks=(unsigned int16)(len/64);//64 pixels/block
       if(hi==lo)
       {
          //High and low bytes are identical. Leave prior data
          //on the port(s)and just toggle the write strobe.
          while(blocks--)
          {
             i=16;//64 pixels/block/4 pixels/pass
             do
             {
                WR_STROBE;WR_STROBE;WR_STROBE;WR_STROBE;//2 bytes/pixel
                WR_STROBE;WR_STROBE;WR_STROBE;WR_STROBE;//x 4 pixels
             } while(--i);
          }

          //Fill any remaining pixels(1 to 64)
          for(i=(unsigned char)len & 63;i--;)
          {
             WR_STROBE;
             WR_STROBE;
          }

          } else {
          while(blocks--)
          {
             i=16;//64 pixels/block/4 pixels/pass
             do
             {
                write8(hi);write8(lo);write8(hi);write8(lo);
                write8(hi);write8(lo);write8(hi);write8(lo);
             } while(--i);
          }

          for(i=(unsigned char)len & 63;i--;)
          {
             write8(hi);
             write8(lo);
          }
       }

       CS_IDLE;
    }

    void drawFastHLine(signed int16 x, signed int16 y, int16 length,  unsigned int16 color)
    {
       signed int16 x2=(x+length-1);

       //Initial off-screen clipping
       if((length<=0) ||
       (y < 0 ) ||(y  >=_height) ||
       (x>=_width) ||(x2<0 ))return;

       if(x<0)
       {
          //Clip left
          length+=x;
          x = 0;
       }

       if(x2>=_width)
       {
          //Clip right
          x2 = _width-1;
          length=x2-x+1;
       }

       setAddrWindow(x, y, x2, y);
       flood(color, length);
       if(driver==ID_932X)setAddrWindow(0,  0, _width-1,  _height-1);
       else    setLR();
    }

    void drawFastVLine(signed int16 x, signed int16 y, int16 length,
    unsigned int16 color)
    {
       signed int16 y2;

       #ignore_warnings  201

       //Initial off-screen clipping
       if((length<=0) ||
       (x < 0 ) ||(x  >=_width) ||
       (y>=_height) ||((y2=(y+length-1))<0))return;

       #ignore_warnings  none

       if(y<0)
       {

          //Clip top
          length+=y;
          y = 0;
       }

       if(y2>=_height)
       {
          //Clip bottom
          y2 = _height-1;
          length=y2-y+1;
       }

       setAddrWindow(x, y, x, y2);
       flood(color, length);
       if(driver==ID_932X)setAddrWindow(0,  0, _width-1,  _height-1);
       else    setLR();
    }

    void fillRect(signed int16 x1,signed int16 y1,signed int16 w,signed int16 h,
    unsigned int16 fillcolor)
    {
       signed int16 x2, y2;
       y2=(y1+h-1);
       x2=(x1+w-1);

       //Initial off-screen clipping
       if((w <=0) || (h<=0) ||
       (x1 >=_width) ||(y1 >=_height) ||
       (x2<0) || (y2<0 ))return;

       if(x1<0)
       {
          //Clip left
          w+=x1;
          x1=0;
       }

       if(y1<0)
       {
          //Clip top
          h+=y1;
          y1=0;
       }

       if(x2>=_width)
       {
          //Clip right
          x2=_width-1;
          w=x2-x1+1;
       }

       if(y2>=_height)
       {
          //Clip bottom
          y2=_height-1;
          h=y2-y1+1;
       }

       setAddrWindow(x1, y1, x2, y2);
       flood(fillcolor,(unsigned int32)w*(unsigned int32)h);
    }

   
    

    void drawPixel(signed int16 x, signed int16 y, unsigned int16 color)
    {
       //Clip
       if((x<0) ||(y<0) || (x>=_width) || (y>=_height))return;
       CS_ACTIVE;

       if((driver==ID_9341) ||(driver==ID_HX8357D))
       {
          setAddrWindow(x, y, _width-1, _height-1);
          CS_ACTIVE;
          CD_COMMAND;
          write8(0x2C);
          CD_DATA;
          write8(color>>8);write8(color);
       }

       CS_IDLE;
    }

    void pushColors(unsigned int16*data, unsigned char len, int1 first)
    {
       unsigned int16 color;
       unsigned char hi, lo;
       CS_ACTIVE;

       if(first==true)
       {
          //Issue GRAM write command only on first call
          CD_COMMAND;

          if((driver==ID_9341) ||(driver==ID_HX8357D))
          {
             write8(0x2C);
             } else{
             write8(0x22);
          }
       }

       CD_DATA;

       while(len--)
       {
          color=*data++;
          hi=color>>8;//Don't simplify or merge these
          lo=color;//lines,  there's macro shenanigans
          write8(hi);//going on.
          write8(lo);
       }

       CS_IDLE;
    }

    #ifndef read8

    unsigned char read8fn(void);

    #define read8isFunctionalized
    #endif
    #ifdef read8isFunctionalized
    #define read8(x)x=read8fn()
    #endif

    unsigned int16 readPixel(signed int16 x, signed int16 y)
    {
       if((x<0) ||(y<0) || (x>=_width) || (y>=_height))return 0;
       CS_ACTIVE;
    }

    unsigned int16 readID(void)
    {
       //***SPFD5408 change--Begin
       return 0x9341;
    }

    unsigned int32 readReg(unsigned char r)
    {
       unsigned int32 id;
       unsigned char x;

       //try reading register #4
       CS_ACTIVE;
       CD_COMMAND;
       write8(r);
       setReadDir();//Set up LCD data port(s)for READ operations
       CD_DATA;
       delay_us(50);
       read8(x);
       id=x;//do not merge or otherwise simplify
       id<<=8;//these lines. It's an unfortunate
       read8(x);
       id |= x;//shenanigans that are going on.
       id<<=8;//these lines. It's an unfortunate
       read8(x);
       id |= x;//shenanigans that are going on.
       id<<=8;//these lines. It's an unfortunate
       read8(x);
       id |= x;//shenanigans that are going on.
       CS_IDLE;
       setWriteDir();//Restore LCD data port(s)to WRITE configuration
       //Serial.print("Read $");Serial.print(r, HEX);
       //Serial.print(":\t0x");Serial.println(id, HEX);
       return id;
    }

    // Pass 8-bit (each) R,G,B, get back 16-bit packed color
    unsigned int16 color565(unsigned char r, unsigned char g, unsigned char b)
    {
       return((r  & 0xF8)<<8) | ((g & 0xFC)<<3) |(b>>3);
    }

    #ifndef write8

    void write8(unsigned char value)
    {
       write8inline(value);
    }

    #endif
    #ifdef read8isFunctionalized

    unsigned char read8fn(void)
    {
       unsigned char result;
       read8inline(result);
       return result;
    }

    #endif
    #ifndef setWriteDir

    void setWriteDir(void)
    {
       setWriteDirInline();
    }

    #endif
    #ifndef setReadDir

    void setReadDir(void)
    {
       setReadDirInline();
    }

    #endif
    #ifndef writeRegister8

    void writeRegister8(unsigned char a, unsigned char d)
    {
       writeRegister8inline(a, d);
    }

    #endif
    #ifndef writeRegister16

    void writeRegister16(unsigned int16 a, unsigned int16 d)
    {
       writeRegister16inline(a, d);
    }

    #endif
    #ifndef writeRegisterPair

    void writeRegisterPair(unsigned char aH, unsigned char aL, unsigned int16 d)
    {
       writeRegisterPairInline(aH, aL, d);
    }

    #endif

    void writeRegister24(unsigned char r, unsigned int32 d)
    {
       CS_ACTIVE;
       CD_COMMAND;
       write8(r);
       CD_DATA;
       delay_us(10);
       write8(d>>16);
       delay_us(10);
       write8(d>>8);
       delay_us(10);
       write8(d);
       CS_IDLE;
    }

    void writeRegister32(unsigned char r, unsigned int32 d)
    {
       CS_ACTIVE;
       CD_COMMAND;
       write8(r);
       CD_DATA;
       delay_us(10);
       write8(d>>24);
       delay_us(10);
       write8(d>>16);
       delay_us(10);
       write8(d>>8);
       delay_us(10);
       write8(d);
       CS_IDLE;
    }

    //#ignore_warnings none
