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

#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6

//Libreria para configurar LCD a 4 bits
#include "Configura_LCD_4-8_bits.c"
#include <stdlib.h> //Funci�n para convertir cadena de caracteres a int

#define   F1  PIN_C1
#define   F2  PIN_C0
#define   F3  PIN_C2 
#define   F4  PIN_D1

#define   C4  PIN_A3
#define   C3  PIN_A2
#define   C2  PIN_A1 
#define   C1  PIN_A0

//########################ESPACIO PARA VARIABLES GLOBALES

unsigned char barrer_teclado (void){
         unsigned char tecla = 15;
     
         output_low (F1);
         
         tecla = input_a ()&0x0f;
         if (tecla != 15){
            output_high (F1) ;
           
            if (tecla == 7) return (10) ;
            if (tecla == 11) return (3) ;
            if (tecla == 13) return (2) ;
            if (tecla == 14) return (1);
         }
         output_high (F1);
         output_low (F2) ;
         tecla = input_a ()&0x0f;
         if (tecla != 15){
            output_high (F2) ;
           
            if (tecla == 7) return (11) ;
            if (tecla == 11) return (6) ;
            if (tecla == 13) return (5) ;
            if (tecla == 14) return (4);
         }
         output_high (F2);
         output_low (F3);
         tecla = input_a ()&0x0f;
         if (tecla != 15){
            output_high (F3) ;
            
            if (tecla == 7) return (12) ;
            if (tecla == 11) return (9) ;
            if (tecla == 13) return (8) ;
            if (tecla == 14) return (7) ;
         }
         output_high (F3) ;
         output_low (F4);
         tecla = input_a ()&0x0f;
         if (tecla != 15){
            output_high (F4) ;
            
            if (tecla == 7) return (13) ;
            if (tecla == 11) return (14) ;
            if (tecla == 13) return (0) ;
            if (tecla == 14) return (15);
         }
         output_high (F4) ;
         
         
         return (0x80) ;
}
unsigned int16 i,j,dato_analogo,temp,motorViento;
unsigned char tecla;
unsigned int16 unidad,decena,centena,umil,pos;
unsigned int16 tempUsuario,luzUsuario,vientoUsuario;
int32 luz;

void main(void){
  PLLEN = 1; //Inicializa el microcontrolador
  lcd_init_4bits(); //Inicializar la LCD
  setup_adc(  ADC_CLOCK_INTERNAL  );
  setup_adc_ports(sAN6|sAN12|sAN5 |VSS_VDD);
  // \f borrar todo y iniciar 0  
 printf(lcd_putc_4bits,"\fTemp. Max=\nLuz. Min=");
 lcd_ubicaxy_4bits(13,1);
 j=0;
 pos = 0;
  unidad = decena = centena = umil = 0;
 WHILE(TRUE){
 tecla = barrer_teclado();
 if(TECLA != 0x80){
 if(pos == 0) lcd_ubicaxy_4bits(13-j,1);
 else if (pos == 1)  lcd_ubicaxy_4bits(13-j,2);
 else  lcd_ubicaxy_4bits(15-j,1);
 if(tecla <= 9 ){
 printf(lcd_putc_4bits,"%u",tecla);
  switch(j){
      case 0: unidad = tecla; break;
      case 1: decena = tecla; break;
      case 2: centena = tecla; break;
      case 3: umil = tecla; break;
   }
 j++;
 }
 if(tecla == 15){
 if(pos == 0){
 lcd_ubicaxy_4bits(12-j,2);
 tempUsuario =  ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)   +  unidad;
 unidad = decena = centena = umil = 0;
 }else if(pos == 1){
 luzUsuario =  ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)   +  unidad;
 printf(lcd_putc_4bits,"\fVel. viento=");
 unidad = decena = centena = umil = 0;
 }else{
  vientoUsuario =  ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)   +  unidad;
 //printf(lcd_putc_4bits,"\f%Lu %Lu %Lu",tempUsuario,luzUsuario,vientoUsuario);
 unidad = decena = centena = umil = 0;
 break;
 }
   j=0;
  pos++;
 }
  delay_ms(150);
 }
}
 
   printf(lcd_putc_4bits,"\fTemp.  C \nMot           lm"); 
while(1){
//Lectura de sensores
 set_adc_channel(12); //Selecciona el canal al cual esta conectado el lm35
 delay_us(10);
 dato_analogo=read_adc();
 temp=(dato_analogo*1.4);
 //MOTOR 
 if(temp >= tempUsuario){
 output_high(PIN_E2);
 }else{
 output_low(PIN_E2); 
 }
 set_adc_channel(5);
 delay_us(10);
 dato_analogo=read_adc();
 motorviento=dato_analogo; //  valor dato analogo PIN AN4



 
 //Muestra los valores de los sensores a la LCD
 lcd_ubicaxy_4bits(5,1);   
 printf(LCD_PUTC_4bits, "    "); 
 lcd_ubicaxy_4bits(5,1);    
 printf(LCD_PUTC_4bits, " %Lu", temp); 

 lcd_ubicaxy_4bits(6,2);   
printf(LCD_PUTC_4bits, "   "); 
lcd_ubicaxy_4bits(6,2);  
printf(LCD_PUTC_4bits, "%Lu",motorViento); 

 //Activa el buzzer
  if(motorviento >= vientoUsuario){
 output_high(PIN_A5);
 }else{
 output_low(PIN_A5); 
 }
  set_adc_channel(6);
 delay_us(10);
 dato_analogo=read_adc();
 luz=(dato_analogo); //  valor dato analogo PIN A11
 if(luz <= 5) {
  luz = 1000;
  }else if(luz > 210){
   luz = 0;
   }
   else{
   luz = (int32)(190 - luz)*7.7;
   }
   if(luz < 0  || luz > 1000) luz = 1000;
lcd_ubicaxy_4bits(11,2);    
printf(LCD_PUTC_4bits,"    ");  
lcd_ubicaxy_4bits(11,2); 
printf(LCD_PUTC_4bits,"%Lu",luz); 
   if(luz >= luzUsuario){
 output_high(PIN_D7);
 }else{
 output_low(PIN_D7);
 }
delay_ms(100);
}

}
