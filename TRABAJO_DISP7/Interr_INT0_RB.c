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

#pin_select U2TX=PIN_D2 //Selecciona hardware UART2
#pin_select U2RX=PIN_D3 //Selecciona hardware UART2




#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6


/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define d_unidad  PIN_B5   // Pin seleccionado para el control del 7segmento Unidad
#define d_decena  PIN_E2   // Pin seleccionado para el control del 7segmento Decena
#define d_centena PIN_B6      // Pin seleccionado para el control del 7segmento Centena
#define d_umil    PIN_B7   // Pin seleccionado para el control del 7segmento Unidad de Mil
#define PULSADOR_1   PIN_B0

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/

char i=0,sw;
unsigned int16 num1,num2,suma;
unsigned int16 umil=0,cent=0,dec=0,unid=0;
      //PGFEDCBA
char display[16] =  { 0b00111111,//0
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
                   };
                       // La anterior Tabla contiene los valores binarios para la visualización de los
                      // números en los segmentos

/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

#include <stdlib.h> // for atoi
               
                 

  
     // }

void conver_bcd (int32 x){
cent=0,dec=0,unid=0,umil = 0;
         while(x>=1000){
          x= x-1000;
          ++umil;
}
         while(x>=100){
          x= x-100;
          ++cent;
}
     while(x>=10){    // este procedimiento convirte los datos de binario a bcd 
          x= x-10;
          ++dec;
   }
         unid=(unsigned char)x;
}

void barrer_display(int16 k){
int i;
for(i=0;i<=k;i++){
output_d(~display[unid]);
         output_low(d_unidad);
         delay_ms(2);
         output_high(d_unidad);

       

         output_d(~display[dec]);  
         output_low(d_decena); 
         delay_ms(2);
         output_high(d_decena); 
         
         
         output_d(~display[cent]);
         output_low(d_centena);
         delay_ms(2);
         output_high(d_centena);
        

         output_d(~display[umil]);
         output_low(d_umil);
         delay_ms(2);
         output_high(d_umil); 
         
}
}
/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 

 enable_interrupts(GLOBAL);
 enable_interrupts(INT_RB);
 ext_int_edge( 0, H_TO_L);
 for(;;){   
     //conver_bcd(1234);   
         barrer_display(1);
     if(!input(PIN_B0)){
     sw++;
     if(sw == 1){
     num1 = ((unsigned int16)(umil*1000)) + (cent*100) + (dec* 10) + unid;
     conver_bcd(0);
     }else if(sw == 2){
     suma =((unsigned int16)(umil*1000)) + (cent*100) + (dec* 10) + unid;
     suma += num1;
     conver_bcd(suma);
     }else if(sw == 3){
     conver_bcd(0);
     sw = 0;
     suma = 0;
     num1 = 0;
     }
      
     barrer_display(25);
     }
    if( !input(PIN_B4)){
    ++unid;
    
    if(unid==10)unid=0;
      barrer_display(25);
    }
    if(!input(PIN_B3) ){ 
    ++dec;
    if(dec==10)dec=0;
      barrer_display(25);
    }
    if(!input(PIN_B2) ){ 
    ++cent;
    if(cent==10)cent=0;
      barrer_display(25);
    }
    if(!input(PIN_B1) ){
    ++umil;
    if(umil==10)umil=0;
      barrer_display(25);
    }
 

      
 }
    }//end main
     

  
  
  

