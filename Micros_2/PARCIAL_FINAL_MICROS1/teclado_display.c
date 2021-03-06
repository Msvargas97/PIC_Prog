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
#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}
#bit PLLEN = 0xf9b.6

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define   C1  PIN_A0
#define   C2  PIN_A1
#define   C3  PIN_A2
#define   C4  PIN_A3
                  
#define   F4  PIN_B7
#define   F3  PIN_B6
#define   F2  PIN_B5
#define   F1  PIN_B4
#include <stdlib.h> // for atoi32

#define d_unidad  PIN_E0 // Pin seleccionado para el control del 7segmento Unidad
#define d_decena  PIN_A5     // Pin seleccionado para el control del 7segmento Decena
#define d_centena PIN_E1   // Pin seleccionado para el control del 7segmento Centena
#define d_umil    PIN_E2   // Pin seleccionado para el control del 7segmento Unidad de Mil
#define CH_VIENTO 10 //Canal del ADC seleccionado para leer la humedad
#define CH_TEMP 12 //Canal del ADC seleccionado para leer la temperatura
#define CH_LUZ 11 //Canal del ADC seleccionado para leer la luz
#define PIN_MOTOR PIN_C7
#define PIN_LED PIN_C6
#define PIN_BUZZER PIN_B3

/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
char n=0 ;
unsigned char tecla=0;
unsigned char bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;  
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/
unsigned int16 i,j;
unsigned int16 dato_analogo;
int32 luz;
unsigned char temp,vel_viento,select;//Variables para guardar datos de los sensores
char unidad=0,decena=0,centena=0,umil=0;
int1 asignarDatos;
unsigned int16 rangoTemp,rangoLuz,rangoViento;
      //PGFEDCBA
char display[17]=  { 0b00111111,//0
         0b00000110, //1
         0b01011011, //2
         0b01001111, //3
         0b01100110, //4
         0b01101101, //5
         0b01111101, //6
         0b00000111, //7
         0b01111111, //8
         0b01100111, //9
         0b01110111, //A
         0b01111100, //B
         0b01011000, //C
         0b01011110, //D
         0b01111011, //E
         0b01110001, //F
         0b01000000//-
}; // La anterior Tabla contiene los valores binarios para la visualizaci�n de los
      // n�meros en los segmentos
      void conver_bcd (unsigned int16 x){
         bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;
         umil = centena = decena = unidad = 0;
         while (x >= 1000){
            x = x - 1000;
            ++umil;
         }
         while (x >= 100){
            x = x - 100;
            ++centena;
         }
         while (x >= 10){ // este procedimiento convirte los datos de binario a bcd
            x = x - 10;
            ++decena;
         }
         unidad = x;
      }
      void barrido_display(int16 repet){
         int k = 0;
         for (k = 0; k < repet; k++) {
            output_d (display[unidad]) ;
            output_high (d_unidad) ;
            delay_ms (2) ;
            output_low (d_unidad) ;
            
            output_d (display[decena]);
            output_high (d_decena);
            delay_ms (2) ;
            output_low (d_decena);
            
            output_d (display[centena]);
            output_high (d_centena) ;
            delay_ms (2) ;
            output_low (d_centena);
            output_d (display[umil]);
            output_high (d_umil) ;
            delay_ms (2) ;
            output_low (d_umil);
         }
      }
      /********************************************************/
      /********************************************************/
      unsigned char barrer_teclado (void){
         unsigned char tecla = 15;
         barrido_display (4) ;
         output_low (F1);
         
         tecla = input_a ()&0x0f;
         if (tecla != 15){
            output_high (F1) ;
            barrido_display (22) ;
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
            barrido_display (22) ;
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
            barrido_display (22) ;
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
            barrido_display (22);
            if (tecla == 7) return (13) ;
            if (tecla == 11) return (14) ;
            if (tecla == 13) return (0) ;
            if (tecla == 14) return (15);
         }
         output_high (F4) ;
         
         
         return (0x80) ;
      }
      /********************************************************/
      /********************************************************/
      unsigned char esperar_teclado (void){
         unsigned char tecla = 0x80;
         while (tecla == 0x80) tecla = barrer_teclado  () ;
         return (tecla) ;
      }
      /******************************************************************************/
      /******************************************************************************/
      /*--------------------- Espacio de codigo principal --------------------------*/
      /******************************************************************************/
      #zero_ram
      void main(){
         PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\
         
         //Configuta el ADC y los canales que se utilizaran
         setup_adc (ADC_CLOCK_INTERNAL) ;
         setup_adc_ports (sAN10|sAN11|sAN12, VSS_VDD) ;
         set_tris_a (0x0f);
         set_tris_e (0);
         SET_TRIS_C(0b00000111);
         output_e (0b111) ;
         output_high (F1) ;
         //Pone en bajo todas los pines de control
         while(1){
         output_toggle(PIN_E2);
         delay_ms(1000);
         }
         output_low(PIN_MOTOR);
         output_low(PIN_LED);
         output_LOW(PIN_BUZZER);
         unidad = decena = centena = umil = 0;
         
         for (;;) {
            for (j = 0; j < 3; j++) {
            barrido_display (1) ;
            unidad = decena = centena = umil = 0;
            asignarDatos = false;
            if (asignarDatos == false){
               tecla = esperar_teclado ();
               unidad = tecla;
               if (tecla == 15){
                  asignarDatos = true;
                  unidad = 0;
               }
            }
            
            if (asignarDatos == false) {
               tecla = esperar_teclado ();
               decena = tecla;
               if (tecla == 15) {
                  asignarDatos = true;
                  decena = 0;
               }
            }
            if (asignarDatos == false){ tecla = esperar_teclado ();
               centena = tecla;
               if (tecla == 15){
                  asignarDatos = true;
                  centena = 0;
               }
            }
            if (asignarDatos == false){tecla = esperar_teclado ();
               umil = tecla;
               if (tecla == 15) umil = 0;
               barrido_display (150) ;
               asignarDatos = true;
            }
            if (asignarDatos) {
               switch (j) {
               case 0: rangoTemp = ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)   +  unidad;
               break;
               case 1: rangoLuz = ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)  +  unidad;
               break;
               case 2: rangoViento = ( (unsigned int16) umil * 1000) + ( (unsigned int16) centena * 100) + ( (unsigned int16)decena * 10)  +  unidad;
               break;
               }
            asignarDatos = false;
            unidad = decena = centena = umil = 0;
            }
            }
      unidad = centena = decena = umil = 16;
      if (rangoTemp > 150) rangoTemp = 150;
      if (rangoLuz > 1000) rangoLuz = 1000;
      if (rangoViento > 255) rangoViento = 255;
      barrido_display (80) ;
      delay_ms (250) ;
      barrido_display (80) ;
      delay_ms (250) ;
      barrido_display (80) ;
      delay_ms (250) ;
      conver_bcd (rangoTemp) ;
      barrido_display (100) ;
      conver_bcd (rangoLuz) ;
      barrido_display (100) ;
      conver_bcd (rangoViento) ;
      barrido_display (100) ;
      while (true) {
      //Seleccionar el valor del sensor que se desea mostrar
         tecla = barrer_teclado ();
         switch (tecla) {
            case 10:
            select = 0;
            break;
            case 11:
            select = 1;
            break;
            case 12:
            select = 2;
            break;
         }
         set_adc_channel (CH_TEMP); // Inicia lectura por ADC canal donde esta conectado el sensor de temperatura
         delay_us (100); //Espera 100 microsegunds para estabilizar el ADC
         temp = (read_adc () * 1.3022); //Calcula Temperatura 1 - 150 grados centrigrados
         set_adc_channel (CH_LUZ) ;
         delay_us (100) ;
         luz = read_adc ();
         if (luz > 3){
            luz = (int32) (110 - luz);
            
            if ( luz < 0 )luz = 0;
            luz = luz * 8.2;
            if (luz > 1000 && luz < 1100){
               luz = 1000;
            }else if (luz > 1100) luz = 0;
            }else{
            luz = 1000;
         }
         set_adc_channel (CH_VIENTO); // Inicia lectura
         delay_us (100) ;
         vel_viento = read_adc ();
         if (vel_viento <= 2) vel_viento = 0;
         //Selecciona la salida segun el valor de la variable select
         switch (select){
            case 0:
            dato_analogo = temp; //Asigna el valor de salida con el valor de temperatura
            break;
            case 1:
            dato_analogo = luz;
            break;
            case 2:
            dato_analogo = vel_viento;
            break;
         }
         conver_bcd (dato_analogo) ;
         barrido_display (1) ;
         if(temp >= rangoTemp && vel_viento <= 10){
         output_high(PIN_MOTOR);
         }else{
         output_low(PIN_MOTOR);
         }
         if(luz <= rangoLuz){
         output_high(PIN_LED);
         }else{
         output_low(PIN_LED);
         }
         if(vel_viento >= rangoViento){
         output_high(PIN_BUZZER);
         }else{
         output_low(PIN_BUZZER);
         }
      }
         }
      }
