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

#pin_select U2TX=PIN_D2 //Selecciona hardware UART2
#pin_select U2RX=PIN_D3 //Selecciona hardware UART2
#pin_select P1A=PIN_B0
#pin_select P2A=PIN_B1
#pin_select P3A=PIN_B2





#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6

//#use  rs232(baud=9600,parity=N,UART1,bits=8,timeout=30)

#include "Configura_LCD_4-8_bits.c"
/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/

 
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/


unsigned int16 i=0,n=0,tecla,duty ;

      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

#include <stdlib.h> // for atoi32
#include <math.h>
#define   F1  PIN_A5
#define   F2  PIN_E0
#define   F3  PIN_E1 
#define   F4  PIN_E2
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

 lcd_init_4bits();   // Comandos de inicialización del LCD.
  //Configuracion de PWM
  setup_timer_2(T2_DIV_BY_16,149,1);
  setup_adc( NO_ANALOGS);
  setup_ccp4(CCP_PWM);
  setup_ccp5(CCP_PWM);
  setup_ccp6(CCP_PWM);

  set_pwm5_duty (0);
  set_pwm6_duty (0);
  set_pwm4_duty (0);
     for(i=0;i<4;i++){ 
   printf(lcd_putc_4bits,"\f !!BIENVENIDO!!  \n   "); //
   delay_ms(400);
   }
      for(i=0;i<4;i++){ 
   printf(lcd_putc_4bits,"\fPARA ASIGNAR \n OPRIMA '*'"); //
   delay_ms(400);
   }
    printf(lcd_putc_4bits,"\fLED Rojo\nIngrese %cPWM:",'%');
  
  int menu,cont,decena,centena,unidad;
  
  while(true){

  tecla= esperar_teclado ();
  //Guarda el # oprimido
  if(tecla >= '0' && tecla <= '9'){
  lcd_putc_4bits(tecla);
  tecla-=48;
  switch(cont){
   case 0: centena = tecla; break;
   case 1: decena = tecla; break;
   case 2: unidad = tecla; break;
  }
  cont++;
  }
 //Asignación del PWM
  if(tecla == 'x'){
  if(cont > 2) duty = (centena * 100 ) + (decena*10) + (unidad);
  else if (cont == 2) duty = (centena * 10 ) +  decena;
  else if (cont == 1) duty = centena;
  if(duty > 100) duty = 100;
  duty = (255*duty) / 100;
  printf(lcd_putc_4bits,"\fPWM Asignado -->\n%Lu",duty);
  delay_ms(1000);
  switch(menu){
   case 0: set_pwm4_duty(duty);break;
   case 1: set_pwm5_duty(duty); break;
   case 2: set_pwm6_duty(duty); break;     
   }
    menu++; IF(menu == 3) menu = 0;
    switch(menu){
   case 0: printf(lcd_putc_4bits,"\fLED Rojo\nIngrese %cPWM:",'%');  break;
   case 1: printf(lcd_putc_4bits,"\fLED Amarillo\nIngrese %cPWM:",'%'); break;
   case 2: printf(lcd_putc_4bits,"\fLED Verde\nIngrese %cPWM:",'%'); break;     
   }
   //Reinicia las variables
   cont = duty = 0;

  }
  }
}//end main
     

  
