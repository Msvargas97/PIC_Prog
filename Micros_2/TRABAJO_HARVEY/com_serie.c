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
#use rs232(baud=115200,parity=N,UART1,bits=8,timeout=30)
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6
#include <string.h>
#include "Configura_LCD_4-8_bits.c"

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define   F1  PIN_A5
#define   F2  PIN_E0
#define   F3  PIN_E1 
#define   F4  PIN_E2
                  
#define   C4  PIN_A0
#define   C3  PIN_A1
#define   C2  PIN_A2 
#define   C1  PIN_A3 
#define FLECHA 0b01111110
#define GRADOS 0b11011111
#define BUFFER_SIZE 64

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned int16 i=0,size,size_data[(BUFFER_SIZE/2)],contador,sensor1,sensor2,sensor3;
unsigned int16 temp,hum,luz;
unsigned char dato_rs232,buffer[BUFFER_SIZE];
int1 Flag;
      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

#include <stdlib.h>

#int_rda
void rd_isr(void)//función de interrupción por recepción de datos USART
{
   static int1 start;
   static char cont_data;
   if ( contador==0 ) Flag=true;
   dato_rs232=getc ( ) ;
   if ( dato_rs232=='$' )
   {
      start=true;
   }

   if ( start)
      if ( dato_rs232=='#' )
      {
         size_data[cont_data]=contador;
      cont_data++;
      }

   if(dato_rs232 == '\n')
   {
      size=contador;
      contador=cont_data=0;
      Flag=false;
      start=false;
      char string[10];
      char j;
      for ( i=0;i<size;i++ )
      {
         string[j]=buffer [i];
         //printf ( lcd_putc_4bits,"%c",string[j] ) ;
         j++;

         if (i== ( size_data[0]-1 ) )
         {
            //printf ( lcd_putc_4bits," " );
            temp= atol (string) ;
            j=0;
            }else if (i== ( size_data[1]-1) ) {
            hum=atol ( string ) ;
            j=0;
            }else if (i==size-1 ) {
            luz=atol ( string ) ;
            j=0;
            for ( i=0;i<10;i++ ) string[i]=0; //Borra el buffer2
         }
      }
      }else{
      if ( dato_rs232!='$' && dato_rs232!='#' )
      {
         buffer[contador]=dato_rs232;
         contador++;
      }
   }
}


/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram

void main()
{
   PLLEN=1;//Habilita PLL para generar 48MHz de oscilador*/\
   lcd_init_4bits ( );//Comandos de inicialización del LCD.
   setup_adc_ports ( sAN0|sAN1|sAN2);//Selecciona el puerto a realizar la conversion
   setup_adc ( adc_clock_internal );//Selecciona el reloj de conversion
   set_adc_channel ( 0 );//Selecciona el canal de conversion
   printf ( lcd_putc_4bits,"\fCom. Serial\n" );//
 //  printf ( "%Lu\n",sensor1); //Envia dato de prueba
   enable_interrupts ( global );//Habilito interrupción USART
   enable_interrupts ( int_rda ) ;
   lcd_ubicaxy_4bits ( 1,2 ) ;
  
   for ( ;; )
   {
      
      if ( !Flag )
      {
      set_adc_channel ( 0 );//Selecciona el canal de conversion
      delay_us(10);
      sensor1=read_adc ( );
      set_adc_channel ( 1 );//Selecciona el canal de conversion
      delay_us(10);
      sensor2=read_adc ( );
      set_adc_channel ( 2 );//Selecciona el canal de conversion
      delay_us(10);
      sensor3=read_adc ( );
     // lcd_ubicaxy_4bits ( 1,1 ) ;
     // printf ( lcd_putc_4bits,"%Lu,%Lu,%Lu,%Lu",size_data[0],size_data[1],size_data[2],size);
      lcd_ubicaxy_4bits ( 1,2 ) ;  
      printf(lcd_putc_4bits,"              ");
      lcd_ubicaxy_4bits ( 1,2 ) ; 
      printf(lcd_putc_4bits,"T:%Lu L:%Lu H:%Lu",temp,hum,luz); 
     //Envia los datos de los sensores con cierto formato
      printf ( "$%Lu#$%Lu#$%Lu#\n",sensor1,sensor2,sensor3);
      delay_ms ( 200 ) ;
      }
   }
}

