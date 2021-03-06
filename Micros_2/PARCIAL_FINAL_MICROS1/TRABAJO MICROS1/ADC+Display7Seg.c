#include <18F47J53.h>
#device ADC=8
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
#pin_select U2TX=PIN_D2 //Selecciona hardware UART2
#pin_select U2RX=PIN_D3 //Selecciona hardware UART2
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6
#use  rs232(baud=9600,parity=N,UART1,bits=8,timeout=30)

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define d_unidad  PIN_E0 // Pin seleccionado para el control del 7segmento Unidad
#define d_decena  PIN_A5     // Pin seleccionado para el control del 7segmento Decena
#define d_centena PIN_E1   // Pin seleccionado para el control del 7segmento Centena
#define d_umil    PIN_E2   // Pin seleccionado para el control del 7segmento Unidad de Mil
#define CH_VIENTO 12 //Canal del ADC seleccionado para leer la humedad
#define CH_TEMP 10 //Canal del ADC seleccionado para leer la temperatura
#define CH_LUZ 9 //Canal del ADC seleccionado para leer la luz

//Pines de pulsadores
#define PULSADOR1 PIN_B7
#define PULSADOR2 PIN_B6
#define PULSADOR3 PIN_B5

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned int16 i=0,n=0 ;
unsigned int16 dato_analogo;
int32 luz;
unsigned char temp,vel_viento,select;//Variables para guardar datos de los sensores
unsigned char bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;
char unidad=0,decena=0,centena=0,umil=0;

char display[16]=  
{
   0b00111111,//0
   0b00000110,//1
   0b01011011,//2
   0b01001111,//3
   0b01100110,//4
   0b01101101,//5
   0b01111101,//6
   0b00000111,//7
   0b01111111,//8
   0b01100111,//9
   0b01110111,//A
   0b01111100,//B
   0b01011000,//C
   0b01011110,//D
   0b01111011,//E
   0b01110001,//F
};// La anterior Tabla contiene los valores binarios para la visualización de los

                      // números en los segmentos 
      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#include <stdlib.h> // for atoi32

void conver_bcd (unsigned int16 x)
{
   bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;

   while(x >= 1000)
   {
      x = x - 1000;
      ++bcd_mil;
   }

   while(x >= 100)
   {
      x = x - 100;
      ++bcd_cent;
   }

   while(x >= 10)
   {
      // este procedimiento convirte los datos de binario a bcd
      x = x - 10;
      ++bcd_dec;
   }

   bcd_unid = x;
}

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram

void main()
{
   PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\
   //Configuta el ADC y los canales que se utilizaran
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(sAN10|sAN9|sAN12,VSS_VDD);
   for(;;)
   {
      //Lectura de pulsadores para saber que valor desea mostrar
      if( !input(PULSADOR1))
      {
         select = 0;
         }else if( !input(PULSADOR2)){
         select = 1;
         }else if( !input(PULSADOR3)){
         select = 2;
      }

      //Selecciona la salida segun el valor de la variable select
      switch(select)
      {
         case 0:
         set_adc_channel(CH_TEMP);// Inicia lectura por ADC canal donde esta conectado el sensor de temperatura
         delay_us(100);//Espera 100 microsegunds para estabilizar el ADC
         temp = (read_adc() * 1.3022);//Calcula Temperatura 1 - 150 grados centrigrados
         dato_analogo = temp;//Asigna el valor de salida con el valor de temperatura
         break;

         case 1:
         set_adc_channel(CH_LUZ);
         delay_us(100);
         luz =  read_adc();
         if(luz > 3){
         luz = (int32)(110 - luz);
         
         if(luz < 0)luz = 0;
         luz = luz * 8.2;
        if(luz > 1000 && luz < 1100){
        luz = 1000;
        }else if(luz > 1100)luz = 0;
         }else{
         luz = 1000;
         }
         dato_analogo = luz;
         break;
         case 2:
         set_adc_channel(CH_VIENTO);// Inicia lectura
         delay_us(100);
         vel_viento = read_adc();
         dato_analogo = vel_viento;
         break;
      }

      //Salida display 7 segmentos
      conver_bcd(dato_analogo);
      for(i = 0;i < 80;i++)
      {
         output_D(display[bcd_unid]);
         output_high(d_unidad);
         delay_us(200);
         output_low(d_unidad);
         output_D(display[bcd_dec]);
         output_high(d_decena);
         delay_us(500);
         output_low(d_decena);
         output_D(display[bcd_cent]);
         output_high(d_centena);
         delay_us(500);
         output_low(d_centena);
         output_D(display[bcd_mil]);
         output_high(d_umil);
         delay_us(500);
         output_low(d_umil);
      }

   }
}//end main

     
  
