#include "PLANTILLA_18F47J53.h"
#include "Configura_LCD_4-8_bits.c"
#include <stdlib.h>

#use rs232(baud=19200, xmit=PIN_B2, rcv=PIN_B1)

#define MAX_LUMENS   1280 //Valor offset del adc para sensor de Luz

//################Espacio para variables globales##########
volatile char tecla; //Variable para almacenar la tecla presionada
unsigned char inputTeclado[4]; //Vector para guardar los datos ingresados 
unsigned int16 i,j;
unsigned int16 sensorValues[3];
unsigned int16 lastValueLuz;
unsigned int16 outMax[3] = {150,1000,100};
int16 outLimitMax[3];
unsigned int8 duty1,duty2,duty3;
int1 writeLCD=0; //Variable booleana para habilitar o deshabilitar el cursor
char menu;
int1 EnterFlag,Bandera1;
//*** ADD ***
//######### Variables para teclado PS2 #########
unsigned char edge, bitcount,y=1,x=1,z=1,d=1,c=1;
unsigned mensaje[17];
unsigned mensaje2[18];
unsigned char dato_rs232;
char got_interrupt;
char interrupt_count;
char status_b3;

#bit INTF_BIT = 0xFF2.1 // INTCON BIT 1 = INTF RB0/INT External Interrupt Flag Bit
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

//-------- Tabla de caracteres correspondientes a la pulsaci�n de la tecla
//-------- en modalidad desplazamiento (pulsando SHIFT)

unsigned char const shifted[68][2] = {
0x0d,9,
0x0e,'�', 0x15,'Q', 0x16,'!', 0x1a,'Z', 0x1b,'S', 0x1c,'A', 0x1d,'W',
0x1e,'"', 0x21,'C', 0x22,'X', 0x23,'D', 0x24,'E', 0x25,'$', 0x26,'�',
0x29,' ', 0x2a,'V', 0x2b,'F', 0x2c,'T', 0x2d,'R', 0x2e,'%', 0x31,'N',
0x32,'B', 0x33,'H', 0x34,'G', 0x35,'Y', 0x36,'&', 0x39,'L', 0x3a,'M',
0x3b,'J', 0x3c,'U', 0x3d,'/', 0x3e,'(', 0x41,';', 0x42,'K', 0x43,'I',
0x44,'O', 0x45,'=', 0x46,')', 0x49,':', 0x4a,'_', 0x4b,'L', 0x4c,'�',
0x4d,'P', 0x4e,'?', 0x52,'�', 0x54,'^', 0x55,'�', 0x5a,13,  0x5b,'*',
0x5d,'�', 0x61,'>', 0x66,' ',   0x69,'1', 0x6b,'4', 0x6c,'7', 0x70,'0',
0x71,'.', 0x72,'2', 0x73,'5', 0x74,'6', 0x75,'8', 0x79,'+', 0x7a,'3',
0x7b,'-', 0x7c,'*', 0x7d,'9',
0,0
};

//################Espacio para declaracion de funciones##########

int32 rule3(int32  x, int32  in_min, int32  in_max, int32  out_min, int32  out_max); //Regla de tres
void imprimir_sensores(); //Funcion para imprimir datos de los sensores
void decode(unsigned char sc);
//-----------------------------------------------------------------------------
// Rutina de gesti�n de interrupciones
//-----------------------------------------------------------------------------

#int_ext
void int_ext_isr(void){

unsigned char data;
   //-------- Los bit 3 a 10 se considerran datos. Paridad, start y stop
   //-------- son ignorados
   if(bitcount < 11 && bitcount > 2){
      data = (data >> 1); //desplaza el dato ?
      status_b3 = input(PIN_B3);
      if((status_b3) == 1){
          data = data | 0x80;
      }
   }
   //-------- Todos los bits se han recibido
   if(--bitcount == 0){
  
      decode(data);
      data = 0;
      bitcount = 11;
      got_interrupt = TRUE;
   }
   got_interrupt = TRUE;
   interrupt_count++;
   disable_interrupts(INT_EXT);
}

//################################################################
void main(void)
{
   mcu_init();//Inicializa el microcontrolador
   //PULL_UP=0;//Con un 0 se habilita la resistencia pull-up del puerto b. con 1 se deshabilita
 //  set_tris_b(0b00011011);//Define cuales pines van a usar la resistencia pull-up y cuales van a ser pines de salida del puerto B
   set_tris_a (0b00100000);
   set_tris_b (0b00001111);
   set_tris_d (0x00);
   bitcount = 11;
   lcd_init_4bits();//Inicializa la LCD
     //-------- Los pins indicados (B0 y B3) son configurados como entradas.
   output_float(PIN_B0);
   output_float(PIN_B3);

   
   
   //--------

   output_low(PIN_B5);

   //-------- Inicializa las variables usadas por la rutina de interrupci�n
   //-------- antes de activar las interrupciones
   interrupt_count = 0;
   got_interrupt = FALSE;
   status_b3 = 0;

   //-------- Desde que se activ� el modo PULLUPS del puerto B, el estado
   //-------- normal del pin B0 es ALTO. La gesti�n de la interrupci�n externa
   //-------- se gestiona cuando se produce un cambio de nivel ALTO a BAJO.
   ext_int_edge(H_TO_L);

   //-------- Asegurarse de que el el bit de flag de la interrupci�n externa
   //-------- es borrado antes de activar la gesti�n de dicha interrupci�n
   //-------- externa.
   INTF_BIT = 0;

   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);
   setup_adc(ADC_CLOCK_INTERNAL);   //Inicializamos el ADC
   setup_adc_ports(sAN2|sAN3|sAN11,VSS_VDD); 
   printf(lcd_putc_4bits,"\f   Bienvenido \nMenu[ESC]");  
   delay_ms(1000);

  // printf(lcd_putc_4bits,"\f");
  while(true){
 // if(tecla){
  //lcd_putc_4bits(tecla);
  //tecla=0;
 // }
   if(tecla=='*' || tecla=='#'){
   tecla=0;
   delay_ms(50);
   break;
   }
  /*  if(EnterFlag){
     printf(lcd_putc_4bits,"\n");
    EnterFlag=false;
    }*/
    //-------- Chequear si se produce alguna interrupci�n (got_interrupt).
      if(got_interrupt == TRUE){
      //-------- Borrar el flag global que se inicio en la rutina de servicio
      //-------- de interrupciones externas.
       got_interrupt = FALSE;
       //-------- Esperar 50 ms para evitar rebotes en los contactos de las teclas.
            //-------- Borrar cualquier interrupci�n producida durante el periodo de espera.
            INTF_BIT = 0;
            //-------- Reactivar interrupciones
            enable_interrupts(INT_EXT);

         } // --- End If ---
  }
   printf(lcd_putc_4bits,"\f[<-]Delete\n[ENTER]Set");
   delay_ms(2500);   
   printf(lcd_putc_4bits,"\fAsigne los sig.\n Limites->");
   delay_ms(2500);
   printf(lcd_putc_4bits,"\f[A]Temp. [C]Luz\n[B]Humedad [ESC]");
   tecla=0;

   while(tecla!='*') //Hasta que se presione la tecla '*' lee los valores limites
   {
   if(tecla){
   
   if(tecla >= 97 && tecla <= 99) tecla-=32;
   
      switch(tecla) //Menu
      {
         case 'A': printf(lcd_putc_4bits,"\fTemperatura Max: \n         [Enter]");break;
         case 'B': printf(lcd_putc_4bits,"\fHumedad Max: \n         [Enter]");break;
         case 'C': printf(lcd_putc_4bits,"\fLuz Max: \n         [Enter]");break;
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
         printf(lcd_putc_4bits,"\f[A]Temp. [C]Luz\n[B]Humedad [ESC]");  writeLCD=0;
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
         if(tecla>='0' && tecla <= '9' && j < 4 ) //Cuatro digitos maximos
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
      tecla=0;
   //   delay_ms(10);
   }
        
    //-------- Chequear si se produce alguna interrupci�n (got_interrupt).
      if(got_interrupt == TRUE){
      //-------- Borrar el flag global que se inicio en la rutina de servicio
      //-------- de interrupciones externas.
       got_interrupt = FALSE;
       //-------- Esperar 50 ms para evitar rebotes en los contactos de las teclas.
            //-------- Borrar cualquier interrupci�n producida durante el periodo de espera.
            INTF_BIT = 0;
            //-------- Reactivar interrupciones
            enable_interrupts(INT_EXT);

         } // --- End If ---
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

//-----------------------------------------------------------------------------
// Decodificaci�n de pulsaciones
//-----------------------------------------------------------------------------
void decode(unsigned char sc)
{
   static unsigned char is_up=0, shift = 0, mode = 0;
   unsigned char i;

     
   //printf(lcd_putc_4bits,"[%X]",sc);
      
   

   //-------- El �ltimo dato recibido fue el identificador de Up-Key
   if (!is_up){
         switch (sc){
               //-------- Identificador de Up-Key
               case 0xF0 :
                  is_up = 1;
                  break;
               //-------- SHIFT Izquierdo
               case 0x12 :
                  shift = 1;
                  break;
               //-------- SHIFT Derecho
               case 0x59 :
                  shift = 1;
                  break;
               //-------- ENTER
               case 0x5A :
                  shift = 0;
                  EnterFlag=true;
                  tecla='#';
                  putc ('*'); 
                 
                  //printf(lcd_putc_4bits,"\n");
                  break;
              case 0x66:
               shift = 0; //Borrado
               tecla = 'D';
               break;
               case 0x76: //ESSC
               shift = 0; //Borrado
               tecla = '*';
               break;
                 //-------- Borrado
               //-------- Si no es ninguno de los identificadores especiales, procesar
               //-------- pulsaci�n, localizando caracter en tabla de caracteres.
               default:
                  //-------- Pulsacfi�n normal
                  if(!shift)
                     {
                        for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
                      
                         if(unshifted[i][0] == sc && sc==0x66){
                         --x;
                         tecla=unshifted[i][1];
                         //lcd_ubicaxy_4bits(x,1);
                        // lcd_putc_4bits(unshifted[i][1]);
                         putc('#');
                         }
                         
                            if (unshifted[i][0] == sc && sc!=0x66)
                           {
                           /* if (x<16)lcd_ubicaxy_4bits(x,1);
                            else
                            lcd_ubicaxy_4bits(x-16,2);
                           */ 
                            
                            
                            //lcd_putc_4bits(unshifted[i][1]);
                            tecla=unshifted[i][1];
                            putc (mensaje[x]);
                           if ( x++==32)x=1;
                           
                           
                           }  
                           
                           if (unshifted[i][0] == sc && sc==0x5b)
                           {
                           putc('!');
                           }  
                           
                     }
                  else
                  //-------- Pulsaci�n + SHIFT presionado
                     {
                        for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
                        if (shifted[i][0] == sc)
                           { 
                              
                         if(shifted[i][0] == sc && sc==0x66){
                         --x;
                         //lcd_ubicaxy_4bits(x,1);
                         //lcd_putc_4bits();
                         tecla = shifted[i][1];
                         putc('#');
                         }
                              
                         if (shifted[i][0] == sc && sc!=0x66) { 
                         
                         //if (x<16)lcd_ubicaxy_4bits(x,1);
                           // else
                           // lcd_ubicaxy_4bits(x-16,2);
                            
                         //lcd_putc_4bits(shifted[i][1]);
                         tecla=shifted[i][1];
                         putc (mensaje[x]);
                          if ( x++==32)x=1;                         }
                         }
                     }
                     break;
               } // --- End Switch
         }
      else
         {
         //-------- No se permiten 2 0xF0 en una fila
         is_up = 0;
         switch (sc)
            {
               //-------- SHIFT Izquierdo
               case 0x12 :
                  shift = 0;
                  break;
               //-------- SHIFT Derecho
               case 0x59 :
                  shift = 0;
                  break;
            } // --- End Switch
         }
         //tecla=0;
}



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

