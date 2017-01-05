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

//#use  rs232(baud=115200,parity=N,UART1,bits=8,timeout=30)
#use  I2C(master, sda=PIN_D1,scl=PIN_D0,SLOW) // I2C usando software
//#use  I2C(master,I2C2, SLOW,FORCE_HW )          // I2C usando hardware



#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6



#include "Configura_LCD_4-8_bits.c"
#include <stdlib.h> // for atoi32
#include <math.h>
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
#define   F1  PIN_A5
#define   F2  PIN_E0
#define   F3  PIN_E1 
#define   F4  PIN_E2
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/


unsigned int16 i=0,n=0,duty;
unsigned char tecla;
char DS1307[64],OnTime[3][3],OffTime[3][3];
char dutyUser[3];
 char col = 1, row = 2;   
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
//Funciones para el teclado matricial
unsigned char barrer_teclado (void)
{       
        unsigned char tecla=15;
        delay_ms(10);

        output_low(F1); 
         
        tecla=  input_a()&0x0f;
        if(tecla!=15){
        output_high(F1);
        delay_ms(180);
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
      delay_ms(180);
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
     delay_ms(180);
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
      delay_ms(180);     
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
//Funciones para el reloj I2C
void write_ds1307(void) {
char ds1307_register;
  i2c_start();                              //Genera señal de Start
  i2c_write(0xD0);                          //Envia apuntador de dispositivo I2C
  i2c_write(0x00);                          //Envia direccion LSB
  for(ds1307_register=0;ds1307_register<64;ds1307_register++){
    i2c_write(DS1307 [ds1307_register]);    //Envia dato
  }
  i2c_stop();                               //Genera señal de Stop
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
i2c_stop();                                  // Genera señal de STOP    
}
 void printHora(int1 fecha){
      lcd_ubicaxy_4bits(1,1);
      // Imprime resultado de lectura ds1307 
      printf(lcd_putc_4bits,"HORA:%2x:%2x:%2x   ",DS1307[horas],DS1307[minutos],DS1307[segundos]); 
      if(fecha == true){
      printf(lcd_putc_4bits,"\nFECHA:%2x/%2x/20%2x",DS1307[dia_mes],DS1307[mes],DS1307[anho]);
      }
}
unsigned char esperar_teclado2(void)
{       
        unsigned int16 lastSeconds=0;
        unsigned char tecla=0x80;
        while(tecla==0x80){
        tecla=barrer_teclado ();
        read_ds1307();                              // Lectura DS1307 
        if(DS1307[segundos] != lastSeconds){
        printHora(false);
        }
        lastSeconds = DS1307[segundos];
        }
        return(tecla);
}
void convertDecToHex(char *data){
char string[10];
string[0] = '0'; //Añadir formato hexadecimal
string[1] = 'x';
i = 2; //Iniciar en 2
while(tecla != 'x'){
tecla = esperar_teclado(); //Espera que se presione una tecla
if(tecla == 'D'){
col--;
lcd_ubicaxy_4bits(col,row);
lcd_putc_4bits(' ');
lcd_ubicaxy_4bits(col,row);
i--;
}
if(tecla >= '0' && tecla <= '9') { //Si la tecla presionada es un numero
string[i] = tecla;
lcd_putc_4bits(string[i]);
col++;
i++;
}
}
col++;
tecla = 0;
string[i] = '\0';
//Escribe sobre la dirección de memoria ingresada
*data = atoi(string); //Convertir de string a entero
}
/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 
lcd_init_4bits();    
//Configuracion de PWM
setup_timer_2(T2_DIV_BY_16,149,1);
setup_adc( NO_ANALOGS);
setup_ccp5(CCP_PWM);
setup_ccp6(CCP_PWM);
setup_ccp7(CCP_PWM);
set_pwm5_duty (0);
set_pwm6_duty (0);
set_pwm7_duty (0);


for(i=0;i<3;i++){ 
  printf(lcd_putc_4bits,"\f !!BIENVENIDO!!  \n   "); //
  delay_ms(400);
}
for(i=0;i<4;i++){ 
 printf(lcd_putc_4bits,"\fPARA ASIGNAR \n OPRIMA '*'"); //
 read_ds1307(); 
 delay_ms(400);
}

printf(lcd_putc_4bits,"\fModificar Hora?\nSi[A] No[B]->");   // Imprime mensaje de estado actual    
tecla = esperar_teclado();
//################## Modificar hora - minutos ############
if(tecla == 'A' || tecla == 'B'){
lcd_putc_4bits(tecla);
delay_ms(500);
if(tecla == 'A'){
col = 1, row = 2;
printf(lcd_putc_4bits,"\fIngrese la Hora:\n");
delay_ms(500);
read_ds1307();
convertDectoHex(&DS1307[horas]);
printf(lcd_putc_4bits,":");
lcd_ubicaxy_4bits(1,1);
printf(lcd_putc_4bits,"Ingrese los Min:");
lcd_ubicaxy_4bits(4,2);
delay_ms(500);
convertDectoHex(&DS1307[minutos]);
printf(lcd_putc_4bits,":00");
delay_ms(500);
/*
DS1307[dia_semana]=0x04;
DS1307[dia_mes]=0x15;
DS1307[mes]=0x09;
DS1307[anho]=0x16;
DS1307[control]=0x10;*/
write_ds1307();
printf(lcd_putc_4bits,"\fHora actualizada\n  con exito...!");
delay_ms(500);
}
}
//############### Ingreso de tiempo de encendido ##################
col = 8, row = 2;
for(n=0;n<3;n++){
col = 4, row = 2;
printf(lcd_putc_4bits,"\fIngrese Hora On\nB%Lu=",n+1);
convertDecToHex(&onTime[horas][n]);
printf(lcd_putc_4bits,":");
lcd_ubicaxy_4bits(1,1);
printf(lcd_putc_4bits,"Ingrese Min On   ");
lcd_ubicaxy_4bits(7,2);
convertDecToHex(&onTime[minutos][n]);
printf(lcd_putc_4bits,":00");
delay_ms(500);
}
//############### Ingreso de tiempo de apagado ##################
for(n=0;n<3;n++){
col = 4, row = 2;
printf(lcd_putc_4bits,"\fIngrese Hora Off\nB%Lu=",n+1);
convertDecToHex(&offTime[horas][n]);
printf(lcd_putc_4bits,":");
lcd_ubicaxy_4bits(1,1);
printf(lcd_putc_4bits,"Ingrese Min Off   ");
lcd_ubicaxy_4bits(7,2);
convertDecToHex(&offTime[minutos][n]);
printf(lcd_putc_4bits,":00");
delay_ms(500);
}
//################## Ingreso de porcentaje de PWM ###################
tecla = esperar_teclado();
printf(lcd_putc_4bits,"\fBombillo 1\nIngrese %cPWM:",'%');
int menu,cont,decena,centena,unidad;
  
  while(menu < 3){
  static char col = 14, row = 2;
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
  col++;
  }
  if(tecla == 'D'){
  col--;
  lcd_ubicaxy_4bits(col,row);
  lcd_putc_4bits(' ');
  lcd_ubicaxy_4bits(col,row);
  cont--;
  }
 //Guardar valor de PWM
  if(tecla == 'x'){
  if(cont > 2) duty = (centena * 100 ) + (decena*10) + (unidad);
  else if (cont == 2) duty = (centena * 10 ) +  decena;
  else if (cont == 1) duty = centena;
  if(duty > 100) duty = 100;
  duty = (255*duty) / 100;
  dutyUser[menu] = duty;
  printf(lcd_putc_4bits,"\fPWM Asignado \n%u",dutyUser[menu]);
  delay_ms(1000);
  menu++; 
  IF(menu == 3){
  printf(lcd_putc_4bits,"\fDesea cambiar el\n%cPWM?Si[A] No[B]",'%');
  tecla = esperar_teclado();
  if(tecla == 'A') menu = 0;
  }
   col = 14; 
    switch(menu){
   case 0: printf(lcd_putc_4bits,"\fBombillo 1\nIngrese %cPWM:",'%');  break;
   case 1: printf(lcd_putc_4bits,"\fBombillo 2\nIngrese %cPWM:",'%'); break;
   case 2: printf(lcd_putc_4bits,"\fBombillo 3\nIngrese %cPWM:",'%'); break;     
   }
   //Reinicia las variables
   cont = duty = 0;
  }
  }
  //############################# Lectura del reloj y  ####################
  char lastSeconds;
  int1 Flag1[3],Flag2[3];
   for(;;){ 
    read_ds1307(); //Obtener la hora 
    if(DS1307[segundos] != lastSeconds){
       printHora(true);
       for(n=0;n<3;n++){
       if(DS1307[horas] == onTime[horas][n] && DS1307[minutos] == onTime[minutos][n] && Flag1[n] == 0){
       Flag1[n] = 1;
       Flag2[n] = 0;
       printf(lcd_putc_4bits,"\fEncendiendo...\n   Bombillo %Lu   ",n+1);
       switch(n){
       case 0: set_pwm5_duty (dutyUser[n]);  break;
       case 1: set_pwm6_duty (dutyUser[n]); break;
       case 2: set_pwm7_duty (dutyUser[n]); break;
       }
       delay_ms(900);
       }
       if(DS1307[horas] == offTime[horas][n] && DS1307[minutos] == offTime[minutos][n]  && Flag2[n] == 0 ){
       Flag2[n] = 1;
       Flag1[n] = 0;
       printf(lcd_putc_4bits,"\fApagando...\n   Bombillo %Lu   ",n+1);
       switch(n){
       case 0: output_low(PIN_B5);  set_pwm5_duty (0); break;
       case 1: output_low(PIN_B6);  set_pwm6_duty (0); break;
       case 2: output_low(PIN_B7);  set_pwm7_duty (0); break;
       }
       delay_ms(200);
       }
       
       }
      }
      lastSeconds = DS1307[segundos];
   } 
}


