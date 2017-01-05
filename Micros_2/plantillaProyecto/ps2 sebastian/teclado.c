//****************************************************************************************
//            
//      barrer_teclado:    realiza un barrido del teclado y detecta si hay alguna tecla pulsada. La
//      variable "Tecla" retorna con el c�digo de la tecla pulsada o el 0x80 en caso 
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
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6
#bit PULL_UP = 0xff1.7 //Bit para activar la resistencia pull-up del puerto b
//#use  rs232(baud=9600,parity=N,UART1,bits=8,timeout=30)
#include "Configura_LCD_4-8_bits.c"
/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#bit INTF_BIT = 0x0B.1 // INTCON BIT 1 = INTF RB0/INT External Interrupt Flag Bit

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



unsigned char valores[3];
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned char bitcount,i,x=1,c=1;
unsigned char tecla,auxTecla;
char got_interrupt;
char interrupt_count;
char status_b3;

unsigned int16 Temperatura,Humedad,Luz;
unsigned int16 j;
unsigned int16 atoiTemp,atoiHum,atoiLuz;
unsigned int8 pwmTemp,pwmHum,pwmLuz;
      
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#include <stdlib.h> // for atoi32
/********************************************************/
/********************************************************/

/********************************************************/
/********************************************************/
 void init_kb(void);
void decode(unsigned char sc);
unsigned char barrer_teclado();
 void imprimir_tecla (void)
 {
    tecla = barrer_teclado ();
    if (tecla){lcd_putc_4bits(tecla); } // imprime la tecla presionada
 }
 int32 regla (int32 x, int32 in_min, int32 in_max, int32 out_min, int32 out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max-in_min)  + out_min;
}

 void entrada_teclado ()
 {
    lcd_ubicaxy_4bits (1, 1) ;
    printf (lcd_putc_4bits, "Bienvenido usuario");
    delay_ms (1000) ;
    lcd_ubicaxy_4bits (1, 2) ;
    printf (lcd_putc_4bits, " presione \"*\" ");
    lcd_ubicaxy_4bits (0, 3) ;
    printf (lcd_putc_4bits, " para guardar limites ");
    delay_ms (2500) ;
    //lcd_ubicaxy_4bits (3, 2) ;
    printf (lcd_putc_4bits, "\f ingrese limites \n ");
    printf (lcd_putc_4bits, "maximos de sensores");
    delay_ms (2000);
    printf (lcd_putc_4bits, "\f sensores: \n") ;
    printf (lcd_putc_4bits, "1) Temperatura:");
    lcd_ubicaxy_4bits (1, 3) ;
    printf (lcd_putc_4bits, "2) Humedad:");
    lcd_ubicaxy_4bits (1, 4) ;
    printf (lcd_putc_4bits, "3) Luz:");
    lcd_ubicaxy_4bits (16, 2) ;
    lcdCursorOn () ;
    while (true)
    {
       static unsigned int16 cont;
       tecla = barrer_teclado();
       if ( (tecla >= '0'&& tecla <= '9') && j < 4)
       {
          valores[j] = tecla;
          lcd_putc_4bits (tecla) ;
          j++;
         
       }
       if (tecla == '*')
       {

          lcdCursorOff () ;
          lcd_ubicaxy_4bits (18, 1) ;
          printf (lcd_putc_4bits, "OK!") ;
          delay_ms (500) ;
          lcd_ubicaxy_4bits (18, 1) ;
          printf (lcd_putc_4bits, "   ");
          if (cont == 0)
          {
             atoiTemp = atol (valores);
             lcd_ubicaxy_4bits (12, 3) ;
             j = 0;
          }
          if (cont == 1)
          {
             atoiHum = atol (valores);
             lcd_ubicaxy_4bits (8, 4) ;
             j = 0;
          }
          if (cont == 2)
          {
             atoiLuz = atol (valores);
             break;
          }
          lcdCursorOn () ;
          cont++;
           init_kb();
          set_tris_b (0b00001111);
          init_kb();
       }
    }
 }

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


 
 /******************************************************************************/
 /******************************************************************************/
 /*--------------------- Espacio de codigo principal --------------------------*/
 /******************************************************************************/
 
 #zero_ram
 void main ()
 {
    PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\
    PULL_UP = 1;
    lcd_init_4bits (); //Inicializa la LCD
    init_kb();
    set_tris_b (0b00001111);
    init_kb();


//  lcd_putc_4bits('f');
    /*while(true){
    imprimir_tecla();
    //tecla = barrer_teclado();
    //if(tecla) lcd_putc_4bits(tecla);
     //-------- Chequear si se produce alguna interrupci�n (got_interrupt).
    
         
       //-------- Chequear si se produce alguna interrupci�n (got_interrupt).
   
    }*/
    
    entrada_teclado ();
    lcdCursorOff() ;
    
    // activar interrupciones (inicializa el teclado ps/2)
    
    setup_adc_ports (sAN0|sAN1|sAN2);  //Configura los pines a usar como analogos
    setup_adc (ADC_CLOCK_INTERNAL); //Inicializa el ADC
   
    printf (lcd_putc_4bits, "\f 150C 100%cHR 1000Lm ", '%');
    lcd_ubicaxy_4bits (1, 2) ;
    printf (lcd_putc_4bits, "Nivel Temp:Ambiente") ;
    lcd_ubicaxy_4bits (1, 3) ;
    printf (lcd_putc_4bits, "Nivel Hum:Estable") ;
    lcd_ubicaxy_4bits (1, 4) ;
    printf (lcd_putc_4bits, "Nivel Luz:Normal") ;
    
   
    
    //Configurar PWM
   setup_timer_2(T2_DIV_BY_16,74,1);   // 10Khz, por que el periodo es 74, si se varia, varia la frecuencia
   setup_ccp5(CCP_PWM);      
   setup_ccp6(CCP_PWM);
   setup_ccp7(CCP_PWM);  
    set_pwm5_duty(pwmTemp); 
    set_pwm6_duty(pwmHum); 
    set_pwm7_duty(pwmLuz); 
    
    for (;;)
    {
       //Lee los pines de los potenciometros usando el ADC
       set_adc_channel (0);
       delay_ms (10) ;
       Temperatura = read_adc ();
       set_adc_channel (1);
       delay_ms (10) ;
       Humedad = read_adc ();
       set_adc_channel (2);
       delay_ms (10) ;
       Luz = read_adc ();
       //Realiza regla de 3 segun el valor deseado
       Temperatura = regla (Temperatura, 12, 1861, 0, 150);
       Humedad = regla (Humedad, 0, 4095, 0, 101);
       if (Luz < 100) Luz = 0;
       Luz = regla (Luz, 0, 1200, 0, 1000);
       if (Luz > 1000) Luz = 1000;
       Luz = 1000 - Luz;
       //le doy la ubicacion de donde quiero que me ense�e el TEM (Temperatura)
       lcd_ubicaxy_4bits (2, 1) ;
       if (Temperatura < 10)
       {
          printf (lcd_putc_4bits, "  %Lu", Temperatura);
          }else if (Temperatura < 100&&Temperatura > 9){
          printf (lcd_putc_4bits, " %Lu", Temperatura);
          }else if (Temperatura > 99){
          printf (lcd_putc_4bits, "%Lu", Temperatura) ;
       }
       
       lcd_ubicaxy_4bits (7, 1); //Humedad
       if (Humedad < 10)
       {
          printf (lcd_putc_4bits, "  %Lu", Humedad);
          }else if (Humedad < 100&&Temperatura > 9){
          printf (lcd_putc_4bits, " %Lu", Humedad);
          }else if (Humedad > 99){
          printf (lcd_putc_4bits, "%Lu", Humedad) ;
       }
       lcd_ubicaxy_4bits (14, 1) ;
       if (Luz < 10)
       {
          printf (lcd_putc_4bits, "   %Lu", Luz);
          }else if (Luz < 100&&Luz > 9){
          printf (lcd_putc_4bits, "  %Lu", Luz);
          }else if (Luz > 99&&Luz < 1000){
          printf (lcd_putc_4bits, " %Lu", Luz);
          }else{
          printf (lcd_putc_4bits, "%Lu", Luz) ;
       }
        lcd_ubicaxy_4bits (12, 2) ;
        if(Temperatura >= atoiTemp){
        printf (lcd_putc_4bits, "Alta    ") ;
        }else{
         printf (lcd_putc_4bits, "Ambiente") ;
        }
       
       lcd_ubicaxy_4bits (11,3);
       if(Humedad >= atoiHum){
        printf (lcd_putc_4bits, "Alta   ") ;
        }else if(Humedad < 20){
         printf (lcd_putc_4bits, "Baja   ") ;
        }else{
         printf (lcd_putc_4bits, "Estable") ;
        }
       
    lcd_ubicaxy_4bits (11, 4) ;
     if(Luz >= atoiLuz){
    printf (lcd_putc_4bits, "Alta  ") ;
     }else{
      printf (lcd_putc_4bits, "Normal") ;
     }
     
    pwmTemp = regla(Temperatura, 0, 150, 0, 255);
    pwmHum = regla(Humedad, 0, 100, 0, 255);
    pwmLuz = regla(Luz, 0, 1000, 0, 255);
    
    delay_ms (100) ;
    set_pwm5_duty( pwmTemp); 
    set_pwm6_duty(pwmHum); 
    set_pwm7_duty(pwmLuz); 
    
    }
    //end main
 }
 
 //***********funciones ps/2******************
 
 void init_kb(void)
{

   //-------- Longitud de la trama para cada pulsaci�n y mensaje de bienvenida
   bitcount = 11;
  //lcd_putc_4bits("\n PC AT Keyboard Interface Ver 1.0 by XP8100");
  //lcd_putc_4bits("\n Adpapted for 16F628A by Redpic");
  //lcd_putc_4bits("\n Decoder and Monitoring for 16F628A connected ");
  // lcd_init_4bits(); 
   
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
}

void decode(unsigned char sc)
{
   static unsigned char is_up=0, shift = 0;
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
                  auxTecla='*';
                 
                  //printf(lcd_putc_4bits,"\n");
                  break;
              case 0x66:
               shift = 0; //Borrado
              auxTecla = 'D';
               break;
               case 0x76: //ESSC
               shift = 0; //Borrado
               auxTecla = '*';
               break;
                 //-------- Borrado
               //-------- Si no es ninguno de los identificadores especiales, procesar
               //-------- pulsaci�n, localizando caracter en tabla de caracteres.
               default:
                  //-------- Pulsacfi�n normal
                  if(!shift)
                     {
                        for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
                      
                         if(unshifted[i][0] == sc){         
                         auxTecla=unshifted[i][1];
                         }                   
                     }
                  else
                  //-------- Pulsaci�n + SHIFT presionado
                     {
                        for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
                        if (shifted[i][0] == sc)
                           { 
                                  auxTecla=shifted[i][1];
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
      
}

unsigned char barrer_teclado(){
  static unsigned char aux;
 
      if(auxTecla){
      aux= auxTecla;
       auxTecla=0;
      interrupt_count = 0;
  // got_interrupt = FALSE;
   status_b3 = 0;
   bitcount=11;
      }else{
      aux = 0;
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
         return aux;
         
}
