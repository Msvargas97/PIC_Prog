#include "PLANTILLA_18F47J53.h"
#include "Configura_LCD_4-8_bits.c"
#include <stdlib.h>

#define   F4  PIN_E2
#define   F3  PIN_E1
#define   F2  PIN_E0 
#define   F1  PIN_A5

#define MAX_LUMENS   1280 //Valor offset del adc para sensor de Luz

//################Espacio para variables globales##########
unsigned char tecla; //Variable para almacenar la tecla presionada
unsigned char inputTeclado[4]; //Vector para guardar los datos ingresados 
unsigned int16 i,j;
unsigned int16 sensorValues[3];
unsigned int16 lastValueLuz;
unsigned int16 outMax[3] = {150,1000,100};
int16 outLimitMax[3];
unsigned int8 duty1,duty2,duty3;
int1 writeLCD=0; //Variable booleana para habilitar o deshabilitar el cursor
char menu;
int1 Bandera1;

// Vector multidimensional con los caracteres del teclado
const char TECLADO[4][4]=
{
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'}
};

//Vector con los pines de fila
const char pinRows[4] = {F1,F2,F3,F4};
//################Espacio para declaracion de funciones##########

int32 rule3(int32  x, int32  in_min, int32  in_max, int32  out_min, int32  out_max); //Regla de tres
unsigned char barrer_teclado(); //Obtener la tecla presionada
void imprimir_sensores(); //Funcion para imprimir datos de los sensores

//################################################################
void main(void)
{
   mcu_init();//Inicializa el microcontrolador
   PULL_UP=0;//Con un 0 se habilita la resistencia pull-up del puerto b. con 1 se deshabilita
   set_tris_b(0b00011011);//Define cuales pines van a usar la resistencia pull-up y cuales van a ser pines de salida del puerto B
   lcd_init_4bits();//Inicializa la LCD
   setup_adc(ADC_CLOCK_INTERNAL);   //Inicializamos el ADC
   setup_adc_ports(sAN2|sAN3|sAN11,VSS_VDD); 
   printf(lcd_putc_4bits,"\f   Bienvenido \nMenu[*]");
   while(1){
      tecla = barrer_teclado();
      if(tecla=='*') break;
   }
   printf(lcd_putc_4bits,"\f[D]Delete\n[#]Set  [*]Start");
   delay_ms(2500);   
   printf(lcd_putc_4bits,"\fAsigne los sig.\n Limites->");
   delay_ms(2500);
   printf(lcd_putc_4bits,"\f[A]Temp. [C]Luz\n[B]Humedad [*]OK");
   tecla=0;

   while(tecla!='*') //Hasta que se presione la tecla '*' lee los valores limites
   {
      tecla=barrer_teclado();
      switch(tecla) //Menu
      {
         case 'A': printf(lcd_putc_4bits,"\fTemperatura Max: \n          [#]Set");break;
         case 'B': printf(lcd_putc_4bits,"\fHumedad Max: \n          [#]Set");break;
         case 'C': printf(lcd_putc_4bits,"\fLuz Max: \n          [#]Set");break;
         case '#': lcdCursorOff(); printf(lcd_putc_4bits,"\f       OK!       \n");//Asigna el valor ingresado
         switch(menu)
         { //Guardar limites ingresados
            case 'A': //Temperatura
            outLimitMax[0] =  atol(inputTeclado);//Convierte los caracteres almacenados en el vector buffer y los guarda en el vector outlimitMax
            if(outLimitMax[0]>outMax[0]){printf(lcd_putc_4bits,"\fEl limite maximo \n  es->%Lu C",outMax[0]);outLimitMax[0]=0;break;}
            printf(lcd_putc_4bits,"Temp. Max->%LuC",outLimitMax[0]);
            break;
            case 'B': //Humedad
            outLimitMax[2]=atol(inputTeclado);
            if(outLimitMax[2]>outMax[2]){printf(lcd_putc_4bits,"\fEl limite maximo \n  es->%Lu",outMax[2]);lcd_putc_4bits('%');break;}
            printf(lcd_putc_4bits,"Hum. Max->%Lu",outLimitMax[2]);
            lcd_putc_4bits('%');
            break;
            case 'C': //Luz
            outLimitMax[1]=atol(inputTeclado);
            if(outLimitMax[1]>outMax[1]){printf(lcd_putc_4bits,"\fEl limite maximo \n  es->%Lu lm",outMax[1]);outLimitMax[1]=0;break;}
            printf(lcd_putc_4bits,"Luz .Max->%Lulm",outLimitMax[1]);
            break;
         }
         delay_ms(3000);
         printf(lcd_putc_4bits,"\f[A]Temp. [C]Luz\n[B]Humedad [*]OK");  writeLCD=0;
         break;
      }
      if(tecla=='A'||tecla=='B'||tecla=='C') //Si la tecla presionada hace parte del menu, la guarda
      {
         menu=tecla;
         memset(inputTeclado,0,4); //Borra los valores ingresados anteriormente
         lcd_ubicaxy_4bits(1,2); 
         j=0;
         writeLCD=1;
      }
      
      if(writeLCD==1) //Para habilitar la entrada de numeros cuando se entra a un submenu
      {
         lcdCursorOn(); //Habilitar Cursor
         if(tecla>='0' && tecla<='9' && j < 4 ) //Cuatro digitos maximos
         {  lcd_putc_4bits(tecla);//Muestra el numero presionado
            inputTeclado[j]=tecla;//Almacena el numero presionado
            j++;//Aumenta el contador para almacenar el siguiente numero presionado
         }
         if(tecla=='D' && j>0 ) //Borrar numero ingresado entre 0 y 4 digitos
         {
            lcd_ubicaxy_4bits(j,2);
            lcd_putc_4bits(" ");
            lcd_ubicaxy_4bits(j,2);
            j--;
            inputTeclado[j]=0x00;
         }
      }
   } //Fin de asignacion de limites
   lcdCursorOff(); //Deshabilita el cursor
   printf(lcd_putc_4bits,"\fTmp:   C Hm:   %c\nLuz:      ",'%');
   //Configuracion de PWM
   setup_timer_2(T2_DIV_BY_16,74,1);   // 10KHz
   setup_ccp5(CCP_PWM); //Captura o compara o PWM  PIN B5    
   setup_ccp6(CCP_PWM); // PIN B6
   setup_ccp7(CCP_PWM); // PIN B7
   set_pwm5_duty(0);  //Inicia PWM en 0 , PWM 8 bits ( 0 - 255)
   set_pwm6_duty(0);  //Inicializar PWM
   set_pwm7_duty(0);  //Inicializar PWM
 while(true)
   {
      //Borrar valores anteriores
      sensorValues[0]=0;
      sensorValues[1]=0;
      sensorValues[2]=0;
     //Lectura de sensores
      set_adc_channel(3); //Selecciona el canal del ADC para leer la temperatura
      delay_us(100);
      for(i=0;i<10;i++) //Toma 10 muestras del adc para posteriormente realizar un promedio
      {
         sensorValues[0]= sensorValues[0] + read_adc(); //Temperatura
         delay_ms(1);
      }
      
      set_adc_channel(2); // Lee el sensor de Luz
      delay_us(100);
      for(i=0;i<20;i++)
      {
         //Toma 20 muestras del adc
         sensorValues[1]+=read_adc(); //LUZ
         delay_ms(1);
      }
      
      set_adc_channel(11); // Lee la humedad
      delay_us(100);
      for(i=0;i<10;i++)
      {
         //Toma 10 muestras del adc
         sensorValues[2]+=read_adc(); //Humedad
         delay_ms(1);
      }
      
      sensorValues[0]/=10;//Realiza el promedio para el sensor de temperatura
      sensorValues[1]/=20;//Realiza el promedio para el sensor de Luz
      sensorValues[2]/=10;//Realiza el promedio para el sensor de Humedad

                       
     sensorValues[0]= rule3(sensorValues[0],12,1862,0,150);//Realiza regla de tres para determinar la temperatura,sabiendo que un valor adc de 12=1C y 1861=150C
                         // 1C = (10mV * 4095 ) / 3.3V ,  150C =  (150 * 10mV * 4095 ) / 3.3V
   
      if(sensorValues[1] < 600) sensorValues[1]=0;
      else sensorValues[1] = (unsigned int16) rule3(sensorValues[1],0,MAX_LUMENS,0,1000); //Regla de 3 sensor de Luz
      
      if(sensorValues[1] > 1000) sensorValues[1] = 1000;
      
      sensorValues[1] = 1000 - sensorValues[1];
      
      sensorValues[2]= rule3(sensorValues[2],0,4095,0,105); //Humedad

      if(sensorValues[2] > 100 ) sensorValues[2] = 100;
             
      if(abs(sensorValues[1] - lastValueLuz) <= 6){
      sensorValues[1] = lastValueLuz;
      }else{
      lastValueLuz=sensorValues[1];
      }
      //Salidas PWM
      duty1 = (unsigned int8)rule3(sensorValues[2],0,outLimitMax[2],0,255); //Bombillo Humedad
      duty2 = (unsigned int8)rule3(sensorValues[1],0,outLimitMax[1],0,255);  // Bombillo Luz
      duty3 = (unsigned int8)rule3(sensorValues[0],0,outLimitMax[0],0,255); //Bombillo Temperatura
     
      set_pwm5_duty(duty1);
      set_pwm6_duty(duty2);
      set_pwm7_duty(duty3);
      
      if(sensorValues[0] >= outLimitMax[0]){
      Bandera1=false;
      set_pwm7_duty(255);
      printf(lcd_putc_4bits,"\f   Limite Max.\nde temperatura!");
      delay_ms(500);
      }else if(sensorValues[2] >= outLimitMax[2]){
      Bandera1=false;
      set_pwm5_duty(255);
      printf(lcd_putc_4bits,"\f   Limite Max.\n de Humedad!");
      delay_ms(500);
      }else if(sensorValues[1] >= outLimitMax[1]){
      Bandera1=false;
      set_pwm6_duty(255);
      printf(lcd_putc_4bits,"\f   Limite Max.\n  de Luz!");
      delay_ms(500);
      }else{
      if(!Bandera1){ //Imprime una sola vez
      printf(lcd_putc_4bits,"\fTmp:   C Hm:   %c\nLuz:      ",'%');
      Bandera1=true;
      }
      imprimir_sensores();
      }
     delay_ms(150);
   }

}
//FUNCIONES
void imprimir_sensores(){
      //MOSTRAR VALOR DE TEMPERATURA
      lcd_ubicaxy_4bits(5,1); //Cursor para la temperatura
      printf(lcd_putc_4bits,"%Lu",sensorValues[0]);
      if(sensorValues[0]<100 && sensorValues[0]>9)
      {
         printf(lcd_putc_4bits," ");
      }else if(sensorValues[0]<10)printf(lcd_putc_4bits,"  ");
     
     //MOSTRAR VALORES DE LUZ
     lcd_ubicaxy_4bits(5,2);
      printf(lcd_putc_4bits,"%Lu",sensorValues[1]);
       printf(lcd_putc_4bits," lm");
      if(sensorValues[1]<1000 && sensorValues[1]>99)
      {
      printf(lcd_putc_4bits," ");
      }else if(sensorValues[1]<100 && sensorValues[1] > 9){
       printf(lcd_putc_4bits," ");
   }else if(sensorValues[1]<10)printf(lcd_putc_4bits,"   ");
      
       //VALORES DE HUMEDAD
       lcd_ubicaxy_4bits(13,1); //Cursor para la humedad
       if(sensorValues[2] > 100 ) sensorValues[2] = 100;
      printf(lcd_putc_4bits,"%Lu",sensorValues[2]);
      if(sensorValues[2]<100 && sensorValues[0]>9)
      {
         printf(lcd_putc_4bits," ");
      }else if(sensorValues[2]<10)printf(lcd_putc_4bits,"  ");
}//Fin del codigo
int32 rule3(int32  x, int32  in_min, int32  in_max, int32  out_min, int32  out_max)
{
   return(x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}
unsigned char barrer_teclado (void) //Esta funcion retorna la tecla presionada
{
   static unsigned char tecla,fila;
   tecla=0;//inicializa la tecla en 0
   delay_ms(10);
   for(fila=0;fila<4;fila++)
   {
      output_low(pinRows[fila]);//Pone en bajo la fila
      tecla =~(input_b())&0b00011011;//Invierte los bits para leerlos de 1-8
      
      if(tecla)
      {
         output_high(pinRows[fila]);
         delay_ms(250);
         switch(tecla)
         {
            case 1: return TECLADO[fila][3];break;
            case 2: return TECLADO[fila][2];break;
            case 8: return TECLADO[fila][1];break;
            case 16: return TECLADO[fila][0];break;
         }
      }
      output_high(pinRows[fila]);
   }
   //Si no detecta ninguna tecla presionada retorna 0
   return 0;
}

