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
#use  rs232(baud=115200,parity=N,UART1,bits=8,timeout=30)
//#use  I2C(master, sda=PIN_A1, scl=PIN_A0, SLOW) // I2C usando software
#use  I2C(master,I2C1, SLOW,FORCE_HW )          // I2C usando hardware
#define SDA PIN_B5
#define SCL PIN_B4
#define DAC_CS     PIN_B1
#define DAC_CLK    PIN_E2
#define DAC_DI     PIN_E1
#define DAC_LDAC   PIN_E0
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6
#include "Configura_LCD_4-8_bits.c"
#include "DS1307_MCP4821.c"
#include "waveGenerator.c"
#include <stdlib.h>

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned char tecla;
unsigned int16 j,valueTimer0,ouput;
volatile unsigned int32 millis,micros,m;
float frequency;
int1 printHour=false;unsigned char bitcount,i;
unsigned char auxTecla,exit;
char tipoOnda;
char input[10];
int1 antiRebote = false,status_b3,FlagWave;

#bit INTF_BIT = 0x0B.1 // INTCON BIT 1 = INTF RB0/int External Interrupt Flag Bit
#byte PORTA = 0xF80
//-------- Tabla de caracteres correspondientes a la pulsaci�n de la tecla
//-------- en modalidad normal (sin pulsar SHIFT)
unsigned char const unshifted[68][2] = {
   0x0d,9,
   0x0e,'�', 0x15,'q', 0x16,'1', 0x1a,'z', 0x1b,'s', 0x1c,'a', 0x1d,'w',
   0x1e,'2', 0x21,'c', 0x22,'x', 0x23,'d', 0x24,'e', 0x25,'4', 0x26,'3',
   0x29,' ', 0x2a,'v', 0x2b,'f', 0x2c,'t', 0x2d,'r', 0x2e,'5', 0x31,'n',
   0x32,'b', 0x33,'h', 0x34,'g', 0x35,'y', 0x36,'6', 0x39,',', 0x3a,'m',
   0x3b,'j', 0x3c,'u', 0x3d,'7', 0x3e,'8', 0x41,',', 0x42,'k', 0x43,'i',
   0x44,'o', 0x45,'0', 0x46,'9', 0x49,'.', 0x4a,'-', 0x4b,'l', 0x4c,'�',
   0x4d,'p', 0x4e,' ', 0x52,'�', 0x54,'`', 0x55,'�', 0x5a,13,  0x5b,'\n',
   0x5d,'�', 0x61,'<', 0x66,' ',   0x69,'1', 0x6b,'4', 0x6c,'7', 0x70,'0',
   0x71,'.', 0x72,'2', 0x73,'5', 0x74,'6', 0x75,'8', 0x79,'+', 0x7a,'3',
   0x7b,'-', 0x7c,'*', 0x7d,'9',
   0,0
};

//-----------------------------------------------------------------------------
// Definici�n de protipos
//---------------------------------------------------------------------------
void init_kb(void);
void decode(unsigned char sc);
unsigned char barrer_teclado();

//-----------------------------------------------------------------------------
// Rutina de gesti�n de interrupciones
//-----------------------------------------------------------------------------
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#int_ext

void int_ext_isr(void){
   unsigned char data;
   //--------Los bit 3 a 10 se considerran datos. Paridad,start y stop
   //--------son ignorados
   if (bitcount<11&&bitcount>2) {
      data= (data>>1);//desplaza el dato ?
      status_b3=input (PIN_B3) ;
      if ( (status_b3) == 1){
         data=data|0x80;
      }
   }
   //--------Todos los bits se han recibido
   if (--bitcount==0) {
      decode (data) ;
      data=0;
      bitcount=11;
      if (antiRebote) delay_ms (120);
   }
}

#int_timer0
void intTimer0(){   
  set_timer0( 45520 ); 
}
unsigned int32 get_millis(){
disable_interrupts(GLOBAL);
m = millis;
enable_interrupts(GLOBAL);
return m;
}
unsigned int32 get_micros(){
m = map(get_timer0(), 65476,65536,0,10);
return m;
}
/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram

void main(){
   PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\
   lcd_init_4bits();
   init_dac();
   init_kb();
   enable_interrupts (int_EXT) ;
   enable_interrupts( GLOBAL );
   tipoOnda = printHour = 0;

   /*
   printf(lcd_putc_4bits,"\fGen.de funciones");
   delay_ms(1000);
   printf(lcd_putc_4bits,"\nSelec. func->");
   delay_ms(1000);
   printf(lcd_putc_4bits,"\f1.Sen 2.Cuadrada\n3.Sierra 4.Triang");
   delay_ms(500);
   while(exit<3){
      static unsigned int8 cont;
      tecla=barrer_teclado();
         if(tecla){
            if (tipoOnda&&tecla>='0'&&tecla<='9'&&cont<4) {
            if (cont<4) lcd_putc_4bits (tecla) ;
            input[cont]=tecla;
            cont++;
            }

         if (tecla>='1'&&tecla<='4'&&tipoOnda==0) {
   printf (lcd_putc_4bits,"\fOnda->") ;
   switch (tecla) {
      case '1': tipoOnda='S';printf (lcd_putc_4bits,"Seno");antiRebote=true;break;
      case '2': tipoOnda='C';printf (lcd_putc_4bits,"Cuadrada");antiRebote=true;break;
      case '3': tipoOnda='D';printf (lcd_putc_4bits,"D. Sierra");antiRebote=true;break;
      case '4': tipoOnda='T';printf (lcd_putc_4bits,"Triangular");antiRebote=true;break;
   }

   printf (lcd_putc_4bits,"\nFreq->") ;
   lcdCursorOn () ;
         }

         if (tecla=='*') {
   exit++;
   cont=0;
         }

         if (exit==1) {
   lcdCursorOff () ;
   frequency=atof (input) ;
   printf (lcd_putc_4bits,"\f       OK!\nFreq->%0.1fHz",frequency);
   delay_ms (1000) ;
   exit=4;
         }
         }
      

   }
          switch (tipoOnda) {
   case 'S': seno (frequency);break;
   case 'C': square (frequency);break;
   case 'D': sawTooth (frequency);break;
   case 'T': triangular (frequency);break;
          }

     
      printf(lcd_putc_4bits,"\fP:%Lu\nTimer0:%Lu",wave.period,valueTimer0); */
      seno(400);
      disable_interrupts( INT_EXT );
      disable_interrupts(GLOBAL );
      setup_timer_0( RTCC_INTERNAL | RTCC_DIV_1 );
      set_timer0( 45520 );  
      
       enable_interrupts( INT_TIMER0 );
       enable_interrupts( GLOBAL );


//Bucle infinito
   for (;;){
   }
}

void init_kb(void){
   bitcount=11;
      // set_tris_b (0b00001111);
   //-------- Los pins indicados (B0 y B3) son configurados como entradas.
   output_float(PIN_B0);
   output_float(PIN_B3);
   status_b3 = 0;

   //-------- Desde que se activ� el modo PULLUPS del puerto B, el estado
   //-------- normal del pin B0 es ALTO. La gesti�n de la interrupci�n externa
   //-------- se gestiona cuando se produce un cambio de nivel ALTO a BAJO.
   ext_int_edge(H_TO_L);
   INTF_BIT = 0;
}

void decode(unsigned char sc){
   unsigned char i;
   for (i=0;unshifted[i][0]!=sc&&unshifted[i][0];i++);
   if (unshifted[i][0]==sc) {
      tecla='#';
      auxTecla=unshifted[i][1];
   }

   if (sc==0x5A) auxTecla='*';
}

unsigned char barrer_teclado(){
   
   static unsigned char aux;
   if (auxTecla) {
      init_kb () ;
      aux=auxTecla;
      auxTecla=0;
      }else{
      aux=0;
   }

   return aux;
}

