
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

#use  rs232(baud=115200,parity=N,UART1,bits=8,timeout=30)
#use  I2C(master, sda=PIN_B1,scl=PIN_B3,SLOW) // I2C usando software
//#use  I2C(master,I2C1, SLOW,FORCE_HW )          // I2C usando hardware
#define SDA PIN_B5
#define SCL Pin_B4

#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6



#include "Configura_LCD_4-8_bits.c"
/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define segundos     0
#define minutos      1
#define horas        2
#define dia_semana   3
#define dia_mes      4
#define mes          5
#define anho         6
#define control      7 


/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/


char i=0,n=0 ;
unsigned char dato_rs232,tecla;
unsigned char bcd_cent=0,bcd_dec=0,bcd_unid=0,bcd_mil=0;
char DS1307 [64]; 
     
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/

void write_ds1307(void) {
char ds1307_register;
  i2c_start();                              //Genera se�al de Start
  i2c_write(0xD0);                          //Envia apuntador de dispositivo I2C
  i2c_write(0x00);                          //Envia direccion LSB
  for(ds1307_register=0;ds1307_register<64;ds1307_register++){
    i2c_write(DS1307 [ds1307_register]);    //Envia dato
  }
  i2c_stop();                               //Genera se�al de Stop
}

/******************************************************************************/
/******************************************************************************/

void read_ds1307(void) {
char ds1307_register;
i2c_start();
i2c_write(0xd0);
i2c_write(0);
i2c_stop();
i2c_start();
i2c_write(0xd1);
   for(ds1307_register=0;ds1307_register<63;ds1307_register++){
      DS1307 [ds1307_register]=i2c_read();   // Toma lectura desde DS1307   
   }
DS1307 [63]=i2c_read(0);                     // Toma lectura desde DS1307 
i2c_stop();                                  // Genera se�al de STOP    
}

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 

lcd_init_4bits();

DS1307[horas]=0x23;
DS1307[minutos]=0X25;
DS1307[segundos]=0;
DS1307[dia_semana]=0x04;
DS1307[dia_mes]=0x23;
DS1307[mes]=0x09;
DS1307[anho]=0x16;
DS1307[control]=0x10;


printf(lcd_putc_4bits,"\fInit DS1307!");   // Imprime mensaje de estado actual

write_ds1307();
printf(lcd_putc_4bits,"\f");
   for(;;){ 
     // printf("\r\nIniciando Lectura DS1307:   "); // Imprime mensaje de estado actual
      read_ds1307();                              // Lectura DS1307 
      
      // Imprime resultado de lectura ds1307 
      printf(lcd_putc_4bits,"\f%2x:%2x:%2x  \n%2x/%2x/20%2x",DS1307[horas],DS1307[minutos],DS1307[segundos],DS1307[dia_mes],DS1307[mes],DS1307[anho]);      
     // printf(lcd_putc_4bits,"\f%2x",DS1307[horas]);
      delay_ms(1000);                             // Espera en milisegundos 
   } 
}
