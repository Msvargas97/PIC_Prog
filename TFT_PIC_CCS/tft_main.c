#include <18F47J53.h>
#device ADC=12
#FUSES NOWDT         //WDT disabled (enabled by SWDTEN bit)                    
#FUSES PLL3          //Divide by 3 (12 MHz oscillator input)            
#FUSES NOPLLEN       //PLL Disabled
#FUSES NOSTVREN      //stack overflow/underflow reset enabled                
#FUSES NOXINST       //Extended instruction set disabled            
#FUSES NOCPUDIV      //No CPU system clock divide         
#FUSES NOPROTECT     //Program memory is not code-protected          
#FUSES HSPLL         //HS oscillator, PLL enabled, HSPLL used by USB           
#FUSES SOSC_HIGH     //High Power T1OSC/SOSC circuit selected
#FUSES CLOCKOUT      //CLKO output enabled on the RA6 pin 
#FUSES NOFCMEN       //Fail-Safe Clock Monitor disabled
#FUSES NOIESO        //Two-Speed Start-up disabled
#FUSES WDT32768      //Watchdog Postscaler 1:32768
#FUSES DSWDTOSC_int  //DSWDT uses INTOSC/INTRC as clock
#FUSES RTCOSC_int    //RTCC uses INTRC as clock
#FUSES NODSBOR       //Zero-Power BOR disabled in Deep Sleep
#FUSES NODSWDT       //Deep Sleep Watchdog Timer Disabled
#FUSES DSWDT8192     //Deep Sleep Watchdog Postscaler: 1:8,192 (8.5 seconds)   
#FUSES NOIOL1WAY     //IOLOCK bit can be set and cleared
#FUSES ADC12         //ADC 10 or 12 Bit Select:12 - Bit ADC Enabed 
#FUSES MSSPMSK7      //MSSP 7 Bit address masking
#FUSES NOWPFP        //Write Protect Program Flash Page 0
#FUSES NOWPCFG       //Write/Erase last page protect Disabled
#FUSES WPDIS         //WPFP[5:0], WPEND, and WPCFG bits ignored 
#FUSES WPEND         //Start protection at page 0
#FUSES LS48MHZ       //Low Speed USB mode with 48 MHz System clock at 48 MHz USB CLKEN divide-by is set to 8 
#use delay(clock=48000000)
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6
#use  rs232(baud=9600,parity=N,UART1,bits=8,timeout=30)

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
/*############ PINOUT DISPLAY #########################*/
#define RD_PIN PIN_A3            /*pin A3 */
#define WR_PIN PIN_A5            /*pin A5*/
#define CD_PIN PIN_E0            /*pin E0 */
#define CS_PIN PIN_E2            /*pin E2 */
#define RST_PIN PIN_E1           /*pin E1*/
#define fast_io(d)
/*############ TOUCHSCREEN   #########################*/
#define RESOLUTION   4095        /*Resolucion del ADC*/
#define PIN_YP PIN_A5
#define PIN_XM PIN_E0
#define PIN_YM PIN_D7   // can be a digital pin
#define PIN_XP PIN_D6   // can be a digital pin
#define YP 4  // # del canal analogo conectado en PIN YP
#define XM 5  // # del canal analogo conectado en PIN_XM
/*########### CALIBRATION #######################*/
#define SENSIBILITY 300 //Valor de la resistencia entre el pin XP y YM
#define MINPRESSURE 10  //Minima presion aplicada en el eje z
#define MAXPRESSURE 1000//Maxima presion aplicada en el eje z
#define MINX 385        //Minimo valor del ADC en x
#define MINY 448        //Minimo valor del ADC en Y
#define MAXX 3391       //Maximo valor del ADC en X
#define MAXY 2816       //Maximo valor del ADC en Y
/*############### COLORES FORMATO HEX ###########*/
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#include "SPFD5408_Adafruit_GFX.c" //Libreria para el manejo del display
#include "SPFD5408_TouchScreen.c"  //Libreria para el manejo del touch resistivo
/*###############################################*/
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
int16 currentcolor,oldcolor;
      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#define BOXSIZE 40
#define PENRADIUS 3
#include <stdio.h>
#define print(x,y)  printf(tft_putc,x,y)

void test_texto(unsigned int16 color);
void calibrate_TS(void);
void showResults();
void  drawBorder();

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram

void main()
{
   PLLEN=1;//Habilita PLL para generar 48MHz de oscilador*/\
   TFTLCD_init();
   TouchScreen_init(sAN4 | sAN5, SENSIBILITY);
   setRotation(0);
//   calibrate_TS();
//    while(1);
   drawBorder();
   
   //Initial screen

   setCursor(55, 50);
   setTextSize(3);
   setTextColor(RED);
   print("SPFD5408\r\n");
   setCursor(65, 85);
   print("Library\r\n");
   setCursor(55, 150);
   setTextSize(2);
   setTextColor(BLACK);
   print(" TFT Paint\r\n     %s\r\n", getenv("DEVICE"));
   setCursor(80, 250);
   setTextSize(1);
   setTextColor(BLACK);
   print("\r           Touch to proceed");
   //Wait touch
   waitOneTouch();
   fillScreen(BLACK);
   test_texto(RED);
   delay_ms(2000);
   fillScreen(BLACK);
   fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
   fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
   fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
   fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
   fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
   fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
   fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, WHITE);
   
   drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
   currentcolor=RED;
   struct TSPointTFT p;
   
   for(;;)
   {
      getPoint();
      convertPoint(&p);

      //if sharing pins, you'll need to fix the directions of the touchscreen pins
      //we have some minimum pressure we consider 'valid'
      //pressure of 0 means no pressing!
      if(p.z>0 && p.z<MAXPRESSURE)
      {
         if(p.y<(TS_MINY-5))
         {
                 fillRect(0, BOXSIZE, _width, _height-BOXSIZE, BLACK);

            
         }
      setTextSize(2);
      setCursor(0, 50);
      setTextColor(BLACK, GREEN);

   //   printf(tft_putc, "\rADC:\n\rX:%4Lu Y:%4Lu \n\rZ:%4Lu ",TSPoint.x, TSPoint.y, TSPoint.z);
         if(p.y<BOXSIZE)
         {
            oldcolor=currentcolor;

            if(p.x<BOXSIZE)
            {
               currentcolor=RED;
               drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
               } else if(p.x<BOXSIZE*2){
               currentcolor=YELLOW;
               drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
               } else if(p.x<BOXSIZE*3){
               currentcolor=GREEN;
               drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
               } else if(p.x<BOXSIZE*4){
               currentcolor=CYAN;
               drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
               } else if(p.x<BOXSIZE*5){
               currentcolor=BLUE;
               drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
               } else if(p.x<BOXSIZE*6){
               currentcolor=MAGENTA;
               drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
            }

            if(oldcolor!=currentcolor)
            {
               if(oldcolor==RED)fillRect(0,  0,  BOXSIZE, BOXSIZE, RED);
               if(oldcolor==YELLOW)fillRect(BOXSIZE,  0,  BOXSIZE, BOXSIZE, YELLOW);
               if(oldcolor==GREEN)fillRect(BOXSIZE*2,  0,  BOXSIZE, BOXSIZE, GREEN);
               if(oldcolor==CYAN)fillRect(BOXSIZE*3,  0,  BOXSIZE, BOXSIZE, CYAN);
               if(oldcolor==BLUE)fillRect(BOXSIZE*4,  0,  BOXSIZE, BOXSIZE, BLUE);
               if(oldcolor==MAGENTA)fillRect(BOXSIZE*5,  0,  BOXSIZE, BOXSIZE, MAGENTA);
            }
         }

         if(((p.y-PENRADIUS)>BOXSIZE) &&((p.y+PENRADIUS)<_height))
         {
            fillCircle(p.x, p.y, PENRADIUS, currentcolor);
         }
      }
   }

}//end main

void test_texto(unsigned int16 color)
{
   fillScreen(BLACK);
   setTextColor(WHITE);
   setTextSize(3);
   setCursor(0, 20);
   setTextColor(GREEN);
   setCursor(0, 50);
   printf(tft_putc, "%s",getenv("DEVICE"));
   setTextColor(BLUE);
   setCursor(0, 80);
   printf(tft_putc, "TFT 320x240");
   setTextColor(YELLOW);
   setCursor(0, 110);
   printf(tft_putc, "ABCDEFGH...");
   setTextColor(CYAN);
   setTextSize(5);
   setCursor(0, 150);
   printf(tft_putc, "1234567890");
}

void drawBorder () 
{
   //Draw a border
   unsigned int16 width=_width-1;
   unsigned int16 height=_height-1;
   unsigned int8 border=10;
   fillScreen(RED);
   fillRect(border, border,(width-border*2), (height-border*2),  WHITE);
}

void calibrate_TS(void) 
{
   //Based in code posted in https://forum.arduino.cc/index.php?topic=223769.15
   struct TSPointTFT p1, p2;
   int16 temp;
   unsigned int32 tempL;
   
   fillScreen(BLACK);
   
   fillCircle(4, 4, 4, WHITE);//show the first point
   setCursor(5, 30);
   setTextColor(WHITE);
   setTextSize(1);
   printf(tft_putc, "Please touch the dot");
   unsigned int16 limit=40;
   
   do
   {
      p1=waitOneTouch();
   } while(!(p1.x<limit  && p1.y<limit));

   fillScreen(BLACK);
   fillCircle(234, 314, 4, WHITE);//show the 2nd point
   setCursor(50, 280);
   printf(tft_putc, "Please touch the other dot\r\nX:%Lu Y:%Lu ", TSPoint.x, TSPoint.y);
   delay_ms(500);//debunce
   do
   {
      p2=waitOneTouch();
   } while(!(p2.x>(_width-limit) &&  p2.y>(_height-limit)));

   
   fillScreen(BLACK);
   delay_ms(200);
   setCursor(50, 280);
   printf(tft_putc, "\r\nX:%Lu Y:%Lu ", TSPoint.x, TSPoint.y);

   //!
   temp=p2.x-p1.x;//Calculate the new coefficients, get X difference
   tempL=((unsigned int32)temp*(RESOLUTION+1))/(_width-20);
   TS_MINX=p1.x-((tempL*12)>>12);//10 pixels du bord
   TS_MAXX=p1.x+((tempL*_width)>>12);//220 pixels entre points
   temp=p2.y-p1.y;//¨get Y difference
   tempL=((unsigned int32)temp*(RESOLUTION+1))/(_height-20);
   TS_MINY=p1.y-((tempL*12)>>10);//10 pixels du bord
   TS_MAXY=TS_MINY+((tempL*_height)>>12);

   // Show results
  
   showResults();
}

void showResults() 
{
   fillScreen(BLACK);
   //Header 
   fillRect(0, 0, _width, 10, RED);
   setCursor(40, 0);
   setTextColor(WHITE);
   print("***Results of calibration***\r\n");
   //Results
   setCursor(5, 30);
   setTextSize(2);
   print("After calibration:\r\n");
   printf(tft_putc, "TS_MINX=%Lu\r\n ", TS_MINX);
   printf(tft_putc, "TS_MINY=%Lu\r\n", TS_MINY);
   printf(tft_putc, "TS_MAXX=%Lu\r\n", TS_MAXX);
   printf(tft_putc, "TS_MAXY=%Lu\r\n ", TS_MAXY);
}

