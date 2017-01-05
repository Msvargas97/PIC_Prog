
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
//#use  I2C(master, sda=PIN_D1, scl=PIN_D0, SLOW) // I2C usando software
#use  I2C(master,I2C1, SLOW,FORCE_HW )          // I2C usando hardware



#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6


#INCLUDE <stdlib.h>
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

#bit INTF_BIT = 0x0B.1
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/


char i=0,n=0 ;
char DS1307 [64],buffer_DS1307[6],buffer_limitON[6],buffer_limitOFF[6];
signed int8 pos;
int1 flag;
char buffer[20];
unsigned CHAR bitcount,y;
unsigned CHAR auxTecla;
char got_interrupt;
char interrupt_count;
char status_b3;

unsigned char CONST unshifted[68][2] = {
   0x0d,9,
   0x0e,'º',0x15,'q',0x16,'1',0x1a,'z',0x1b,'s',0x1c,'a',0x1d,'w',
   0x1e,'2',0x21,'c',0x22,'x',0x23,'d',0x24,'e',0x25,'4',0x26,'3',
   0x29,' ',0x2a,'v',0x2b,'f',0x2c,'t',0x2d,'r',0x2e,'5',0x31,'n',
   0x32,'b',0x33,'h',0x34,'g',0x35,'y',0x36,'6',0x39,',',0x3a,'m',
   0x3b,'j',0x3c,'u',0x3d,'7',0x3e,'8',0x41,',',0x42,'k',0x43,'i',
   0x44,'o',0x45,'0',0x46,'9',0x49,'.',0x4a,'-',0x4b,'l',0x4c,'ñ',
   0x4d,'p',0x4e,' ',0x52,'´',0x54,'`',0x55,'¡',0x5a,13,0x5b,'\n',
   0x5d,'ç',0x61,'<',0x66,' ',0x69,'1',0x6b,'4',0x6c,'7',0x70,'0',
   0x71,'.',0x72,'2',0x73,'5',0x74,'6',0x75,'8',0x79,'+',0x7a,'3',
   0x7b,'-',0x7c,'*',0x7d,'9',
   0,0
};
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
void init_kb(VOID);
void decode(UNSIGNED char sc);
unsigned CHAR barrer_teclado();
#INT_ext
void int_ext_isr(VOID){

   UNSIGNED char data;

   //--------Los bit 3 a 10 se considerran datos. Paridad,start y stop
   //--------son ignorados
   IF( bitcount<11&&bitcount>2 ){
      data=( data>>1 );//desplaza el dato ?
      status_b3=input( PIN_B3 );
      IF(( status_b3 )==1 ){
         data=data|0x80;
      }
   }

   //--------Todos los bits se han recibido
   IF(--bitcount==0 ){
      decode( data );
      data=0;
      bitcount=11;
      got_interrupt=TRUE;
   }
}

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
      DS1307 [ds1307_register]= i2c_read();   // Toma lectura desde DS1307
    if(ds1307_register <= anho){
    if(  DS1307 [ds1307_register] >= 0x10 &&   DS1307 [ds1307_register]<=0x19)   DS1307 [ds1307_register] =   DS1307 [ds1307_register]-6;
    else if(  DS1307 [ds1307_register] >= 0x20 &&   DS1307 [ds1307_register]<=0x29)   DS1307 [ds1307_register] =   DS1307 [ds1307_register]-12;
    else if(  DS1307 [ds1307_register] >= 0x30 &&   DS1307 [ds1307_register]<=0x39)   DS1307 [ds1307_register] =   DS1307 [ds1307_register]-18;
    else if(  DS1307 [ds1307_register] >= 0x40 &&   DS1307 [ds1307_register]<=0x49)   DS1307 [ds1307_register] =   DS1307 [ds1307_register]-24;
    else if(  DS1307 [ds1307_register] >= 0x50 &&   DS1307 [ds1307_register]<=0x59)   DS1307 [ds1307_register] =   DS1307 [ds1307_register]-30;
    if(ds1307_register == horas ) buffer_DS1307[0] = DS1307[ds1307_register];
    if(ds1307_register == minutos ) buffer_DS1307[1] = DS1307[ds1307_register];
    if(ds1307_register == segundos ) buffer_DS1307[2] = DS1307[ds1307_register];
    if(ds1307_register == dia_semana ) buffer_DS1307[3] = DS1307[ds1307_register];
    if(ds1307_register == mes ) buffer_DS1307[4] = DS1307[ds1307_register];
    if(ds1307_register == anho ) buffer_DS1307[5] = DS1307[ds1307_register];
   }
   }
DS1307 [63]=i2c_read(0);                     // Toma lectura desde DS1307 
i2c_stop();                                  // Genera señal de STOP    
}

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 
lcd_init_4bits( );
//printf(lcd_putc_4bits,"\fComunicación OK!");                        // Imprime mensaje de bienvenida
/*
DS1307[horas]=0x18;
DS1307[minutos]=0x17;
DS1307[segundos]=0x0;
DS1307[dia_semana]=0x01;
DS1307[dia_mes]=0x01;
DS1307[mes]=0x05;
DS1307[anho]=0x16;
DS1307[control]=0x10;
*/
 //write_ds1307();
   init_kb( );
   output_FLOAT( PIN_B0 );
   output_FLOAT( PIN_B3 );
   interrupt_count=0;
   got_interrupt=FALSE;
   status_b3=0;
   ext_INT_edge( H_TO_L );
   INTF_BIT=0;
   enable_interrupts( INT_EXT );
   enable_interrupts( GLOBAL ); 
   printf( lcd_putc_4bits,"\fON:  Hora:Mn:Sg:\n       " );   

 WHILE( 1 ){
      STATIC int8 enter;
      static UNSIGNED int8 x,cont,cont1;
      static int1 Flag;
      x=barrer_teclado();
      IF( x ){
         
         IF(pos<=13&&( x>='0'&&x<='9' ) ){
            lcd_putc_4bits(x);
            buffer[pos] =x;
            pos++;
            if(pos==2){
            pos = 0;
            if(!Flag) buffer_limitON[cont]= atoi(buffer);
            else  buffer_limitOFF[cont]= atoi(buffer);
            cont++;
            }
                        SWITCH( cont1 ){
               CASE 1:
               CASE 3: printf( lcd_putc_4bits,":" );break;
               CASE 7: 
               CASE 9: printf( lcd_putc_4bits," /" );break;
            }
            cont1++;
           
         }
         IF( x=='*' ){
            IF( enter==0 ){
            lcd_ubicaxy_4bits( 2,1 );
            printf( lcd_putc_4bits,"\nSave..." );
            delay_ms(300);           
            printf( lcd_putc_4bits,"\fON: Dia/Mes/A o" );
            lcd_ubicaxy_4bits( 14,1 );
            printf( lcd_putc_4bits,"%c",0xee );
            lcd_ubicaxy_4bits( 5,2 );
            }
         else if(enter==1){
            lcd_ubicaxy_4bits( 2,1 );
            printf( lcd_putc_4bits,"\nSave" );
            delay_ms(250);
            cont = cont1= 0;
            Flag = true;
            printf( lcd_putc_4bits,"\fOFF:  Hora:Mn:Sg:\n       " );      
         }else if(enter==2){
         lcd_ubicaxy_4bits( 2,1 );
          printf( lcd_putc_4bits,"\nSave..." );
            delay_ms(300);           
            printf( lcd_putc_4bits,"\fOFF:Dia/Mes/A o" );
            lcd_ubicaxy_4bits( 14,1 );
            printf( lcd_putc_4bits,"%c",0xee );
            lcd_ubicaxy_4bits( 5,2 );
         }else{
       /* printf( lcd_putc_4bits,"\f" );
         for( i=0;i<6;i++){
        // if(i==6) lcd_ubicaxy_4bits( 9, 2);
         printf( lcd_putc_4bits,"%u",buffer_limitON[i]);
         }
         while(1);*/
         break;
         }
        enter++;
         }
      }
   }
printf(lcd_putc_4bits,"\f\nIniciand DS1307!\f");   // Imprime mensaje de estado actual  

   for(;;){ 
        static int1 FlagOFF;
     read_ds1307();                              // Lectura DS1307 
      printf(lcd_putc_4bits,"\f");
      flag = FlagOFF = false;
      for(i=0;i<6;i++){
      printf(lcd_putc_4bits,"%u",buffer_DS1307[i]); 
      if(i<3){
      if(i<2){
       printf(lcd_putc_4bits,":");
      }else {
       printf(lcd_putc_4bits," ");
      }
      }else if (i < 5){
      printf(lcd_putc_4bits,"/");
      }
      if(buffer_limitON[i] != buffer_DS1307[i]){
      flag = true;
      }
       if(buffer_limitOFF[i] != buffer_DS1307[i]){
      FlagOFF = true;
      }
      }
      if(flag==false){
      printf(lcd_putc_4bits,"\n");
       for(i=0;i<6;i++){
   output_high(PIN_B6);
      printf(lcd_putc_4bits,"%u", buffer_limitON[i]); 
      }
      }
      if(FlagOFF==false){
      printf(lcd_putc_4bits,"\n");
       for(i=0;i<6;i++){
      output_low(PIN_B6);
      printf(lcd_putc_4bits,"%u", buffer_limitOFF[i]); 
      }
      }
     

      // Imprime resultado de lectura ds1307 
    //  printf(lcd_putc_4bits,"\f%2x:%2x:%2x\n%2x/%2x/20%2x",\
      //DS1307[horas],DS1307[minutos],DS1307[segundos],DS1307[dia_mes],DS1307[mes],DS1307[anho]);      
      
      delay_ms(650);                             // Espera en milisegundos 
   }

 
}
//-----------------------------------------------------------------------------
// Inicialización de teclado.
//-----------------------------------------------------------------------------
void init_kb(VOID){
   bitcount=11;
   lcd_init_4bits( );
}

void decode(UNSIGNED char sc){
   UNSIGNED char i;
   FOR( i=0;unshifted[i][0]!=sc&&unshifted[i][0];i++ );
   IF( unshifted[i][0]==sc ){
      auxTecla=( unshifted[i][1]>='a'&&unshifted[i][1]>='z' )? unshifted[i][1]-32 : unshifted[i][1];
      delay_ms( 120);
   }IF( sc==0x5A )auxTecla='*';
}

unsigned CHAR barrer_teclado(){
   
   static UNSIGNED char aux;
   IF( auxTecla ){
      aux=auxTecla;
      auxTecla=0;
      }ELSE{
      aux=0;
   }

   
   RETURN aux;
}

