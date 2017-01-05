// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// (c) ladyada / adafruit
// Code under MIT License



// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// 1 is no oversampling, whatever data we get is immediately returned
// 2 is double-sampling and we only return valid data if both points are the same
// 3+ uses insert sort to get the median value.
// We found 2 is precise yet not too slow so we suggest sticking with it!

#define NUMSAMPLES 2
#define _yp PIN_YP
#define _xp PIN_XP
#define _xm PIN_XM  
#define _ym PIN_YM
//funcion de mapeo
 int32 mapeo (int32 x, int32 in_min, int32 in_max, int32 out_min, int32 out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max-in_min)  + out_min;
}

struct TSPointTFT{
 unsigned int16 x, y, z;
}TSPoint;

unsigned int16 _rxplate,TS_MINX = MINX,TS_MINY = MINY,TS_MAXX = MAXX,TS_MAXY = MAXY;

#if (NUMSAMPLES > 2)
void insert_sort(int16 array[], unsigned char size) {
  unsigned char j;
  int16 save;
  
  for (int16 i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save; 
  }
}
#endif
void TouchScreen_init(unsigned int16 AN,unsigned int16 rxplate) {
 setup_adc(ADC_CLOCK_DIV_4);
 setup_adc_ports(AN);
  _rxplate = rxplate;
  TSPoint.x = TSPoint.y = TSPoint.z = 0;
}

void getPoint(void) {
  
  unsigned int16 x=0, y=0, z=0;
  unsigned int16 samples[NUMSAMPLES];
  unsigned char i, valid;
  
  valid = 1;

  output_float(_yp);
  output_float(_ym);
   
  output_drive(_xp);
  output_drive(_xm);
  output_bit(_xp, 1);
  output_bit(_xm, 0);
  
  set_adc_channel(YP);
  delay_us(10);

   for (i=0; i<NUMSAMPLES; i++) {
     samples[i] = read_adc();
   }
#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) { valid = 0; }
#endif
   x = samples[NUMSAMPLES/2];
 
   output_float(_xp);
   output_float(_xm);
  
   output_drive(_yp);
   output_bit(_yp,1);
   output_drive(_ym);
   output_bit(_ym, 0);

   set_adc_channel(XM);
   for (i=0; i<NUMSAMPLES; i++) {
     samples[i] = read_adc();
   }

#if NUMSAMPLES > 2
   insert_sort(samples, NUMSAMPLES);
#endif
#if NUMSAMPLES == 2
   if (samples[0] != samples[1]) { valid = 0; }
#endif

  y = samples[NUMSAMPLES/2];
  output_drive(_xp);
  output_bit(_xp, 0);
  
  // Set Y- to VCC
  output_drive(_ym);
  output_bit(_ym,1); 
  
  // Hi-Z X- and Y+
  output_float(_xm);
  output_float(_yp);
  
   set_adc_channel(XM);
   delay_us(10);
   int16 z1 = read_adc();
   
   set_adc_channel(YP);
   delay_us(10);
   int16 z2 = read_adc();

   if (_rxplate != 0) {
     // now read the x 
     float rtouch;
     rtouch = z2;
     rtouch /= z1;
     rtouch -= 1;
     rtouch *= x;
     rtouch *= _rxplate;
     rtouch /= (RESOLUTION+1);
     z = rtouch;
   }
   else
   {
     z = (RESOLUTION-(z2-z1));
   }

   if (! valid) {
     z = 0;
   }

  switch(rotation){
  case 0: 
  case 2:
          TS_MINX = MINY,TS_MINY = MINX, TS_MAXX = MAXY, TS_MAXY = MAXX;
          TSPoint.x = y;
          TSPoint.y = x;
          break;
  case 1: 
  case 3:
         TS_MINX = MINX,TS_MINY = MINY, TS_MAXX = MAXX, TS_MAXY = MAXY;   
         TSPoint.x = x;
         TSPoint.y = y;
         break;
  
  }
  TSPoint.z = z;

   // *** SPFD5408 change -- Begin
   // SPFD5408 change, because Y coordinate is inverted in this controller
  // return TSPoint(y, x, z);
   // -- End
}

void convertPoint(struct TSPointTFT *p){
  p->x = (unsigned int16)mapeo(TSPoint.x,TS_MINX ,TS_MAXX,0,_width);
  p->y =  (unsigned int16)mapeo(TSPoint.y,TS_MINY,TS_MAXY,0,_height);
  if(rotation == 0) {
  p->x = _width - p->x;
  p->y = _height - p->y;
  }else if(rotation == 1)   p->x = _width - p->x;
  else if(rotation == 3)  p->y = _height - p->y;
  p->z = TSPoint.z;
  if(p->x > _width) p->x = _width;
  if(p->y > _height) p->y = _height;
}


struct TSPointTFT waitOneTouch() {
  
  struct TSPointTFT p;
  
  do {
    p= getPoint(); 
    convertPoint(&p);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  return p;
}

