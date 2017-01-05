#include "Configurar_micro.h"
#include "Configura_LCD_4-8_bits.c"


#define   F1  PIN_A5
#define   F2  PIN_E0
#define   F3  PIN_E1 
#define   F4  PIN_E2
                  
#define   C4  PIN_A3
#define   C3  PIN_A2
#define   C2  PIN_A1 
#define   C1  PIN_A0

//########################ESPACIO PARA VARIABLES GLOBALES

unsigned int16 i,j,dato_analogo, temp, hum, luz;
unsigned char tecla;
char BUFFER[4];
int1 limit,Flag,Flag1;
unsigned int16 limitTemp,limitHum,limitLuz;
unsigned int8 pwmTemp,pwmHum,pwmLuz;



//Prototipado de funciones
int32 regladeTres(int32  x, int32  in_min, int32  in_max, int32  out_min, int32  out_max)
{
   return(x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}
void asignarLimites(void);

void main(void){
  PLLEN = 1; //Inicializa el microcontrolador
  lcd_init_4bits (); //Inicializa la LCD
  setup_adc(  ADC_CLOCK_INTERNAL  );               
  setup_adc_ports(sAN12|sAN10|sAN9,VSS_VDD);
    printf(lcd_putc_4bits,"\fTEMP     C LUZ\nHUME    %%     lm"); 
  while(1){
  
  }
  //Ajusta Periodo de señal PWM
//Formula para calcular Periodo de señal PWM (1/clock)*4*mode*(period+1)
//          >opciones mode<
//            T2_DIV_BY_1
//            T2_DIV_BY_4
//            T2_DIV_BY_16

//         >opciones period<
//              0...255
// setup_timer_2(mode,        period,  postscale)
  setup_timer_2(T2_DIV_BY_16,255,2);   // 2929 Hz
  setup_ccp4(CCP_PWM);      
  setup_ccp5(CCP_PWM);
  setup_ccp6(CCP_PWM);  
  set_pwm4_duty(0);  //Inicializar PWM
  set_pwm5_duty(0); 
  set_pwm6_duty(0); 
  printf(lcd_putc_4bits,"\f    WELCOME!   ");
  delay_ms(2000);
  printf(lcd_putc_4bits,"\f(#)Save (D)Clear\n(*)Exit");
  delay_ms(3000);
  printf(lcd_putc_4bits,"\fAsignar Limites:\n");
  printf(lcd_putc_4bits,"A.Tmp B.Hm C.Luz");
  asignarLimites();
  printf(lcd_putc_4bits,"\fTEMP     C LUZ\nHUME    %%     lm"); 
  limit=0;
while(1){
 set_adc_channel(12);
 delay_ms(10);
 dato_analogo=read_adc();
 temp=(dato_analogo*0.03663003663); //  valor dato analogo PIN AN0
 set_adc_channel(10);
 delay_ms(10);
 dato_analogo=read_adc();
 
 hum=(dato_analogo*0.02442002442); //  valor dato analogo PIN AN1
 
 set_adc_channel(9);
 delay_ms(10);
 dato_analogo=read_adc();
 
 luz=(dato_analogo*0.2442002442); //  valor dato analogo PIN AN2

if(temp > limitTemp){
limit=1;
Flag=0;
if(!Flag1){
printf(lcd_putc_4bits,"\fTemp Fuera\n de limite");
delay_ms(200);
Flag1=1;
}
}else if(hum > limitHum){
limit=1;
Flag=0;
if(!Flag1){
printf(lcd_putc_4bits,"\fHumedad Fuera\n de limite");
delay_ms(200);
Flag1=1;
}

}else if(luz > limitLuz){
limit=1;
Flag=0;
if(!Flag1){
printf(lcd_putc_4bits,"\fLuz fuera \nde limite");
delay_ms(200);
Flag1=1;
}
}else{
if(limit==1){
if(!Flag){
Flag1=0;
printf(lcd_putc_4bits,"\fTEMP     C LUZ\nHUME    %%     lm"); 
Flag=1;
}
limit=0;
}
pwmTemp = (unsigned int8) regladeTres(temp,0,limitTemp,0,255);
pwmHum =  (unsigned int8) regladeTres(hum,0,limitHum,0,255); 
pwmLuz =  (unsigned int8) regladeTres(luz,0,limitLuz,0,255);

set_pwm4_duty(pwmTemp);  
set_pwm5_duty(pwmHum); 
set_pwm6_duty(pwmLuz); 

lcd_ubicaxy_4bits(5,1);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, "    "); /// TEMPERATURA PIN AN0
lcd_ubicaxy_4bits(5,1);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, " %lu", temp); /// TEMPERATURA PIN AN0

lcd_ubicaxy_4bits(6,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, "   "); /// HUMEDAD PIN AN1
lcd_ubicaxy_4bits(6,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, "%lu",hum); /// HUMEDAD PIN AN1


lcd_ubicaxy_4bits(11,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits,"    "); /// LUZ PIN AN2
lcd_ubicaxy_4bits(11,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits,"%lu",luz); /// LUZ PIN AN2
delay_ms(100);
}


}
}
//Funciones 
void asignarLimites(void){
while(true){
  static char teclaMenu,save;
  static char numDigits=3;
  tecla=barrer_teclado();
  if(tecla=='*'){
  break;
  }
  if(tecla >= 'A' || tecla >= 'B' || tecla >= 'C' || tecla == '#'  ){
   switch(tecla){
         case 'A': j=0; numDigits=3;  printf(lcd_putc_4bits,"\fLimite Temp:\n"); break;
         case 'B': j=0; numDigits=3;  printf(lcd_putc_4bits,"\fLimite Humedad:\n");   break;
         case 'C': j=0; numDigits=4;  printf(lcd_putc_4bits,"\fLimite Luz:\n"); break;
         case '#': 
         if(save == 'A'){
         limitTemp = atol(BUFFER);  printf(lcd_putc_4bits,"\fTemp Maxima:%Lu ",limitTemp);
         }
         if(save =='B'){
         limitHum = atol(BUFFER);  printf(lcd_putc_4bits,"\fHum Maxima:%Lu ",limitHum);
         }
         if(save =='C'){
         limitLuz = atol(BUFFER);  printf(lcd_putc_4bits,"\fLuz Maxima:%Lu ",limitLuz);
         }
         delay_ms(1000);
         printf(lcd_putc_4bits,"\fAsignar Limites:\n");
         printf(lcd_putc_4bits,"A.Tmp B.Hm C.Luz");
         break;
         case 'D': if( j > 0){lcd_ubicaxy_4bits(j,2); lcd_putc_4bits(" "); lcd_ubicaxy_4bits(j,2); j--; buffer[j]=0x00;} break;
  }
  if(tecla=='A' || tecla=='B' || tecla=='C'){
  save=tecla;
  }
  }
   if(tecla>= '0' && tecla <= '9' && j < numDigits){
    printf(lcd_putc_4bits,"%c",tecla); //Muestra el numero presionado
   BUFFER[j]=tecla;
   j++;
   }  
   if(j==0){
   memset(BUFFER,0x0,4); //Borra lo que haya en el vector
   }
  

  
  }
}
