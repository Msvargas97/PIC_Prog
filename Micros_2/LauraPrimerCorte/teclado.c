//****************************************************************************************
//            
//      barrer_teclado:    realiza un barrido del teclado y detecta si hay alguna tecla pulsada. La
//      variable "Tecla" retorna con el código de la tecla pulsada o el 0x80 en caso 
//      de no haber ninguna.
//
//      esperar_teclado:   Esta funcion espera a que se presione una tecla y no retorna hasta que se halla pulsado una tecla.
//
//
//           
//                               TECLA        CODIGO
//                                     ---------   -----
//          |---|---|---|---       0   11101011 => tecla=PORTA&0b00001111 = 13
//   RA5 -->| 1 | 2 | 3 | A |      1   11101110 => tecla=PORTA&0b00001111 = 14
//          |---|---|---|---       2   11011101 => tecla=PORTA&0b00001111 = 13
//   RE0 -->| 4 | 5 | 6 | B |      3   10111011 => tecla=PORTA&0b00001111 = 11
//          |---|---|---|---       4   11101110 => tecla=PORTA&0b00001111 = 14
//   RE1 -->| 7 | 8 | 9 | C |      5   11011101 => tecla=PORTA&0b00001111 = 13
//          |---|---|---|---       6   10111011 => tecla=PORTA&0b00001111 = 11
//   RE2 -->|0n | 0 | = | D |      7   11101110 => tecla=PORTA&0b00001111 = 14
//          |---|---|---|---       8   01111101 => tecla=PORTA&0b00001111 = 13
//           ^   ^   ^   ^         9   10111011 => tecla=PORTA&0b00001111 = 11
//           |   |   |   |         A   01110111 => tecla=PORTA&0b00001111 = 7
//           RA0 RA1 RA2 RA3       B   01110111 => tecla=PORTA&0b00001111 = 7
//                                 C   01110111 => tecla=PORTA&0b00001111 = 7
//                                 D   01110111 => tecla=PORTA&0b00001111 = 7
//                                 #   10111011 => tecla=PORTA&0b00001111 = 11
//                                 *   11101110 => tecla=PORTA&0b00001111 = 14
//
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
#FUSES DSWDTOSC_INT  //DSWDT uses INTOSC/INTRC as clock
#FUSES RTCOSC_INT    //RTCC uses INTRC as clock
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
 
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/


char i=0,n=0 ;
unsigned char tecla=0;
unsigned char bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;
 
      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

#include <stdlib.h> // for atoi32


 
void conver_bcd (unsigned int16 x){
bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;
         while(x>=1000){
          x= x-1000;
          ++bcd_mil;
}
         while(x>=100){
          x= x-100;
          ++bcd_cent;
}
          while(x>=10){    // este procedimiento convirte los datos de binario a bcd 
          x= x-10;
          ++bcd_dec;
}
         bcd_unid=x;
}
/********************************************************/
/********************************************************/
unsigned char barrer_teclado (void)
{       
        unsigned char tecla=15;
        delay_ms(30);

        output_low(F1); 
         
        tecla=  input_a()&0x0f;
        if(tecla!=15){
        output_high(F1);
        delay_ms(200);
        if(tecla==7)return('A');
        if(tecla==11)return('3');
      if(tecla==13)return('2');
      if(tecla==14)return('1'); 
        }
        output_high(F1);
        
        output_low(F2);
     
       tecla=  input_a()&0x0f;
        if(tecla!=15){ 
      output_high(F2);
      delay_ms(200);
        if(tecla==7)return('B');
        if(tecla==11)return('6');
      if(tecla==13)return('5');
      if(tecla==14)return('4'); 
        }
        output_high(F2);
          
      output_low(F3); 
      
        tecla=  input_a()&0x0f;
        if(tecla!=15){
      output_high(F3);
       delay_ms(200);
        if(tecla==7)return('C');
        if(tecla==11)return('9');
      if(tecla==13)return('8');
      if(tecla==14)return('7');
        } 
        output_high(F3);
        
      output_low(F4); 
        
        tecla=  input_a()&0x0f;
         if(tecla!=15){
       output_high(F4);
        delay_ms(200);      
        if(tecla==7)return('D');
        if(tecla==11)return('#');
      if(tecla==13)return('0');
      if(tecla==14)return('x'); 
        }
        output_high(F4);
  
         
       return(0x80);
       
}
/********************************************************/
/********************************************************/
 
unsigned char esperar_teclado (void)
{       
        unsigned char tecla=0x80;
        while(tecla==0x80)tecla=barrer_teclado ();
        return(tecla);
}
/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 
   
   setup_adc_ports (NO_ANALOGS );
   set_tris_a (0x0f);
   set_tris_e (0);
   output_e(0b111);
   output_high(F1);
   
   lcd_init_4bits();   // Comandos de inicialización del LCD.
   for(i=0;i<4;i++){ 
   printf(lcd_putc_4bits,"\f !!Presione!!  \n !!el Teclado!!    "); //
   delay_ms(400);
   printf(lcd_putc_4bits,"\f               \n                   "); //
   } 
    i=1,n=1;
    for(;;){
   
    tecla= esperar_teclado ();
    lcd_ubicaxy_4bits(i,n);
    lcd_putc_4bits(tecla);
    ++i;
    if((i==17)&&(n==2)){
    i=1,n=1;
    }
    if((i==17)&&(n==1)){
    i=1,n=2;
    }
   output_toggle(PIN_B5);
   delay_ms(500);
   
    }


   
   



    

    }//end main
     

  
