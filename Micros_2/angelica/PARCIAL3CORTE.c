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
#include <stdlib.h> //Función para convertir cadena de caracteres a int

#define   F1  PIN_C1
#define   F2  PIN_C0
#define   F3  PIN_C2 
#define   F4  PIN_D1

#define   C4  PIN_A3
#define   C3  PIN_A2
#define   C2  PIN_A1 
#define   C1  PIN_A0

//########################ESPACIO PARA VARIABLES GLOBALES

unsigned int16 i,j,dato_analogo, temp,viento;
int32 luz;
unsigned char tecla,dato;
char inputChar[20];
unsigned int16 limite1,limite2,limite3;

const char CaracteresTeclado[4][4]={
{'1','2','3', 'A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};//caracteres del teclado

//Vector con los pines de fila
const char pinFilas[4] = { F1,F2,F3,F4 };

unsigned char barrer_teclado (void) //Esta funcion retorna la tecla presionada
{       
        static unsigned char tecla;
        tecla=0; //Reinicia el valor de la tecla
        delay_ms(10);
        for(i=0;i<4;i++){
        output_low(pinFilas[i]); //Pone en bajo la fila
        tecla = ~(input_a()) & 0x0F; //Invierte los bits para leerlos de 1-8
        if(tecla > 0){
        output_high(pinFilas[i]);
        delay_ms(200);
        switch(tecla){
        case 1:  return CaracteresTeclado[i][0]; break;
        case 2:  return CaracteresTeclado[i][1]; break; 
        case 4:  return CaracteresTeclado[i][2]; break;
        case 8:  return CaracteresTeclado[i][3]; break;
        }
        }
        output_high(pinFilas[i]);
        }   
 return 0;
}


void main(void){
  PLLEN = 1; //Inicializa el microcontrolador
  lcd_init_4bits(); //Inicializa la LCD
  
  setup_adc(  ADC_CLOCK_INTERNAL  );
  setup_adc_ports(sAN4|sAN12|sAN11 |VSS_VDD);
  // \f borrar todo y iniciar 0
  printf(lcd_putc_4bits,"\f ##BIENVENIDO##"); //Imprimir mensaje de bienvenida en la LCD
  delay_ms(1000); 
 printf(lcd_putc_4bits,"\fRango de Temp:\n");
 dato = 0;
   while(dato < 3){
   tecla = barrer_teclado();
   
   if(tecla > 0 ) //Si se presiona una tecla
   {
  if(tecla>='0' && tecla <= '9'){
  inputChar[j] = tecla;
   lcd_putc_4bits(inputChar[j]);
   j++;
  }
   if(tecla == '#'){
   inputChar[j] = '\0'; //Para indicar el fin de los datos al atoi y los convierta a int
   if(dato == 0){
   limite1= (unsigned int16)atol(inputChar);
   printf(lcd_putc_4bits,"\fRango de Luz:\n");
   j=0;
   }else if( dato == 1){
   limite2= (unsigned int16) atol(inputChar);
   printf(lcd_putc_4bits,"\fRango de Viento:\n");
   j=0;
   }else if(dato== 2){
   limite3= (unsigned int16) atol(inputChar);
   printf(lcd_putc_4bits,"\fRangos->T:%Lu C\nL:%Lu Lm V:%Lu",limite1,limite2,limite3);
   delay_ms(1000);
   j=0;
   }
   dato++;
   }
   }
   }
   printf(lcd_putc_4bits,"\fTEMP     C LUZ\nVIEN          lm"); 
while(1){
//Lectura de sensores
 set_adc_channel(12); //Selecciona el canal al cual esta conectado el lm35
 delay_us(10);
 dato_analogo=read_adc();
 temp=(dato_analogo*1.5); //  valor dato analogo PIN AN12
 //Activa salida 1, MOTOR 
 if(limite1 <= temp){
 output_high(PIN_B1);
 }else{
 output_low(PIN_B1); 
 }
 set_adc_channel(4);
 delay_us(10);
 dato_analogo=read_adc();
 viento=dato_analogo; //  valor dato analogo PIN AN4
 //Activa el buzzer
  if(limite3 <= viento){
 output_high(PIN_B3);
 }else{
 output_low(PIN_B3); 
 }
 set_adc_channel(11);
 delay_us(10);
 dato_analogo=read_adc();
 luz=(dato_analogo); //  valor dato analogo PIN A11
  if(luz <= 5) {
  luz = 1000;
  }else if(luz > 130){
   luz = 0;
   }
   else{
   luz = (int32)(130 - luz)*7.67645;
   }
   if(limite2 <= luz){
 output_high(PIN_D7);
 }else{
 output_low(PIN_D7);
 }
 
 //Muestra los valores de los sensores a la LCD
 lcd_ubicaxy_4bits(5,1);    // POSICION EN LA PANTALLA X,Y
 printf(LCD_PUTC_4bits, "    "); 
 lcd_ubicaxy_4bits(5,1);    // POSICION EN LA PANTALLA X,Y
 printf(LCD_PUTC_4bits, " %Lu", temp); /// 

 lcd_ubicaxy_4bits(6,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, "   "); ///
lcd_ubicaxy_4bits(6,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits, "%Lu",viento); ///


lcd_ubicaxy_4bits(11,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits,"    "); /// 
lcd_ubicaxy_4bits(11,2);    // POSICION EN LA PANTALLA X,Y
printf(LCD_PUTC_4bits,"%Lu",luz); ///
delay_ms(100);
}
}
