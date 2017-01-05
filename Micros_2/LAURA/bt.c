//incluye librerias
#include "EMm47J53_StackConfig.h" //libreria con configuracion de pic

// configura comunicacion serial, para ver los datos si se desea
#use  rs232(baud=9600,parity=N,UART1,bits=8,timeout=30)

///////////////******************************************************************************/////////////
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)//// bluetooth-----**********
//la comunicación serie 9600 bps, en RC7 y RC6 y activar las interrupciones en recepción del puerto serie.
///////////////******************************************************************************/////////////
#INCLUDE <stdlib.h> // for atoi32

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/

char dato; // valor ascci enviado app
int16 dato1=0,dato2=0; // valor datos leidos sensores
float conversor =255.0,mv=0, grados=0,lum=0; //varibles usadas en las conversiones de los sensores

/// DATOS MAPEO (de ser necesario usar la funcion de mapeo)//////
int x=0,
   in_min=0,
   in_max=134,
   out_min=0,
   out_max=100;

/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

//funcion de mapeo
 int32 mapeo (int32 x, int32 in_min, int32 in_max, int32 out_min, int32 out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max-in_min)  + out_min;
}

void PWM_ini(void)
{
//Ajusta Periodo de señal PWM
//Formula para calcular Periodo de señal PWM (1/clock)*4*mode*(period+1)
//          >opciones mode<
//            T2_DIV_BY_1
//            T2_DIV_BY_4
//            T2_DIV_BY_16

//         >opciones period<
//              0...255
// setup_timer_2(mode,        period,  postscale)
   setup_timer_2(T2_DIV_BY_16,74,      1);   // (1/48000000)*4*16*75 =  100us o 10 khz

setup_ccp1(CCP_PWM);       // c1 Configure CCP1 as a PWM B6
setup_ccp2(CCP_PWM);       //c6  Configura CCP2 as a PWM B5
setup_ccp3(CCP_PWM);       //c7 Configura CCP3 as a PWM B4
}

//******************************************************************//
//////////////////// interrupcion del bluetooth//////////////////////
#int_rda
void serial_isr(){
dato=getc();//recibe el dato de la app

switch (dato) // se evalua el dato
      {
      case '0':
         set_pwm1_duty(0); // carga pwm B6
//!         printf("0");
      break;
      
      case '1':
         set_pwm1_duty(50); // carga pwm B6
//!         printf("1");
      break;
      
      case '2':
         set_pwm1_duty(100); // carga pwm B6
//!         printf("2");   
      break;
      
      case '3':
         set_pwm2_duty(0); // carga pwm B5
//!         printf("3");   
      break;
      
       case '4':
         set_pwm2_duty(50); // carga pwm B5
//!         printf("4");
      break;
      
      case '5':
         set_pwm2_duty(100); // carga pwm B5
//!         printf("5");
      break;
      
      case '6':
         set_pwm3_duty(0); // carga pwm B4
//!         printf("6");   
      break;
      
      case '7':
         set_pwm3_duty(50); // carga pwm B4
//!         printf("7");   
      break;
      
      case '8':
         set_pwm3_duty(100); // carga pwm B4
//!         printf("8");
      break;
      
      //por si se envia un caracter erroneo
      default:
//!         printf("dato erroneo !!");      
      break;
//!      case '9':
//!         set_pwm3_duty(20); // carga pwm B4
//!         printf("9");
//!      break;
      
      
     }

}

/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
   mcu_init();    // Inicializa microcontrolador
   enable_interrupts(INT_RDA);//      habilita interrupcion bt
   enable_interrupts(GLOBAL); // habilita interrupciones globales
   
   set_pwm1_duty(0);  // Ajusta un Duty PWM1 en OFF
   set_pwm2_duty(0);  // Ajusta un Duty PWM2 en OFF
   set_pwm3_duty(0);  // Ajusta un Duty PWM3 en OFF
   
   setup_adc(  ADC_CLOCK_INTERNAL  ); //configura reloj interno para ADC
   setup_adc_ports(sAN0); //Setea el puerto A0 como ADC
   setup_adc_ports(sAN2); //Setea el puerto A1 como ADC
   
   PWM_ini();// funcion activa y configura pwms
   
   while(true)       //Ciclo infinito
   {
       ////lectura sensores////////////////
       set_adc_channel(0);// se prepara lectura A0 temp
       delay_ms (10); //demora para el ADC 
       dato1 = read_adc(); //captura la lectura A0
       mv = (dato1 / conversor) * 3300; // conversion del dato leido a mv
       grados = mv / 10; // conversion de mv a grados centigrados temperatura 
       
       set_adc_channel(2);// se prepara lectura A0 lum
       delay_ms (10); //demora para el ADC   
       dato2 = read_adc();//captura la lectura A0
       lum = dato2;// conversion del dato leido a lum %
       
       ///////// envio de la trama a la aplicacion////////////
       printf("%3.1f,%3.1f\r\n",grados,lum); // trama con los valores de la temp y lum
       //printf("Temperatura: %3.1f c  Luminosidad: %3.1f %c \n\r",grados,lum,'%');
       delay_ms(1000);//demora de tiempo para evitar saturar el bt y la app
   }//fin wh infinito
 }//fin main

