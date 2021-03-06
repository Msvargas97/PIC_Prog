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
#use  I2C(master, sda=PIN_B1,scl=PIN_B3,SLOW) // I2C usando software
//#use  I2C(master,I2C2, SLOW,FORCE_HW )          // I2C usando hardware



#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6



#include "Configura_LCD_4-8_bits.c"
#include <stdlib.h> // for atoi32
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
#define F1  PIN_A5
#define F2  PIN_E0
#define F3  PIN_E1 
#define F4  PIN_E2
#define FLECHA 0b01111110
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/



char DS1307[64],input,HORA[6],MINUTO[6]; 
unsigned int16 i=0,j=0,PWM[3];
char buff[10];
char menu;

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

unsigned char barrer_teclado (void)
{       
        unsigned char tecla=15;
        delay_ms(15);

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
      if(tecla==14)return('*'); 
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

unsigned char HEX_DEC(unsigned char data){
return( data + ((data/10)*6));
}

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

setup_timer_2(T2_DIV_BY_16,149,1);
setup_adc( NO_ANALOGS);
setup_ccp4(CCP_PWM);
setup_ccp5(CCP_PWM);
setup_ccp6(CCP_PWM);
set_pwm4_duty (0);
set_pwm5_duty (0);
set_pwm6_duty (0);

printf(lcd_putc_4bits,"\f!!...WELCOME...!\n################");
delay_ms(1000);
printf(lcd_putc_4bits,"\fTo change the\ntime DS1307?%c'*'",FLECHA);
input = esperar_teclado();
if(input == '*'){
read_ds1307();
printf(lcd_putc_4bits,"\fHour%c        '*'\n",FLECHA);
lcd_ubicaxy_4bits(6,1);
i=0;
while(menu < 2){
input = esperar_teclado();
if(input=='*'){
if(menu==0){
DS1307[horas] = HEX_DEC((buff[0]*10)+buff[1]);
lcd_ubicaxy_4bits(1,2);
printf(lcd_putc_4bits,"Minutes%c",FLECHA);

}else if(menu==1){
DS1307[minutos] = HEX_DEC((buff[0]*10)+buff[1]);
}
i=0;
menu++;
}else if(input >= 48 && input <= 58){
buff[i]= (input-48);
lcd_putc_4bits(input);
i++;
}
}
/*DS1307 [dia_mes] = 0x23;
DS1307 [anho]= 0x16;
DS1307 [mes] = 0x09;
 */
 DS1307[segundos] = 0;
 write_ds1307();
printf(lcd_putc_4bits,"\fTime saved!!!\n%c%2x:%2x:00    '*'",FLECHA,DS1307[horas],DS1307[minutos]);
delay_ms(1000);
}
//Seteo de PWM
for(j=0;j<3;j++){
printf(lcd_putc_4bits,"\fSet PWM Output %Lu\n%c",j+1,FLECHA);
i = 0;
do{
input=esperar_teclado();

 if(input >= 48 && input <= 58){
 lcd_putc_4bits(input);
 buff[i] = input-48;
 i++;
 }
}while(input != '*');
switch(i){
case 2: PWM[j] =  ((unsigned int16)((buff[0]*10)+buff[1])*255) / 100; break;
case 3: PWM[j] =  ((unsigned int16)((buff[0]*100)+(buff[1]*10)+buff[2])*255)/100; break;
}
printf(lcd_putc_4bits,"%%");
delay_ms(1500);
}
//Hora de encedido apagado
for(j=0;j<3;j++){
printf(lcd_putc_4bits,"\fSet HH:mm ON %Lu\n%c",j+1,FLECHA);
i = 0;
do{
input=esperar_teclado();

 if(input >= 48 && input <= 58){
 lcd_putc_4bits(input);
 buff[i] = input-48;
 i++;
 }
}while(input != '*');
HORA[j] =  HEX_DEC((buff[0]*10)+buff[1]);
printf(lcd_putc_4bits,":");
i = input = 0;
do{
input=esperar_teclado();
 if(input >= 48 && input <= 58){
 lcd_putc_4bits(input);
 buff[i] = input-48;
 i++;
 }
}while(input != '*');
MINUTO[j] =  HEX_DEC((buff[0]*10)+buff[1]);
delay_ms(500);
printf(lcd_putc_4bits,"\fSet HH:mm OFF %Lu\n%c",j+1,FLECHA);
i = 0;
do{
input=esperar_teclado();

 if(input >= 48 && input <= 58){
 lcd_putc_4bits(input);
 buff[i] = input-48;
 i++;
 }
}while(input != '*');
HORA[j+3] =  HEX_DEC((buff[0]*10)+buff[1]);
printf(lcd_putc_4bits,":");
i = input = 0;
do{
input=esperar_teclado();
 if(input >= 48 && input <= 58){
 lcd_putc_4bits(input);
 buff[i] = input-48;
 i++;
 }
}while(input != '*');
MINUTO[j+3] =  HEX_DEC((buff[0]*10)+buff[1]);
}

read_ds1307();
printf(lcd_putc_4bits,"\fDate%c%2x/%2x/20%2x",FLECHA,DS1307[dia_mes],DS1307[mes],DS1307[anho]);
while(1==1){
read_ds1307();
for(j=0;j<6;j++){
if(DS1307[horas] == HORA[j] && DS1307[minutos] == MINUTO[j]){
if(j<3) {
if(j==0)set_pwm4_duty(PWM[0]); 
else if (j==1) set_pwm5_duty(PWM[1]);
else set_pwm6_duty(PWM[2]);
}else{
if((j-3)==0)set_pwm4_duty(0); 
else if ((j-3)==1) set_pwm5_duty(0);
else set_pwm6_duty(0);
}
}


}
printf(lcd_putc_4bits,"\nHour%c%2x:%2x:%2x"FLECHA,DS1307[horas],DS1307[minutos],DS1307[segundos]);
delay_ms(250);
}

}


