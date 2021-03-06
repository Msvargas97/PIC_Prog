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

#use  rs232(baud=9600,parity=N,UART1,bits=8)




#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6

/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/
#define DEBUG 0
/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/

unsigned int16 i,j;
const unsigned char maxDataLength = 20;         
char receivedChars[maxDataLength+1], buffer[maxDataLength+1];
unsigned int16 values[maxDataLength],totalValues=0;
unsigned int32 checksum = 0;
int1 newData = false,noNumber = false;   
char cmd1[7],cmd2[7];
unsigned int8 duty1,duty2,duty3;
unsigned int8 temp;
int16 luz;
/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#include <stdlib.h>
#include <string.h>

void recvWithStartEndMarkers() 
{
     static int1 recvInProgress = false;
     static unsigned char ndx = 0 ,ndx2 = 0,idx = 0;
     static char startMarker = '<'; //Marcador de inicio de comandos
     static char endMarker = '>',mediumMarker = ','; //Marcador o simbolo de fin de comandos
    
     if (kbhit() > 0)
     {
          char rc = getc();
          if (recvInProgress == true) 
          {
          if(ndx == 0 && rc > '9'){ //Si el dato enviado es un comando y no valores
          noNumber = true;
          }
               if (rc != endMarker) //Si encuentra el marcador final
               {    
                    if(rc == mediumMarker){
                    receivedChars[ndx] = '\0'; 
                    ndx = 0;
                    values[idx] = atol(receivedChars);
                    idx++;
                    }else if (ndx < maxDataLength) { 
                      receivedChars[ndx] = buffer[ndx2] = rc; ndx++; ndx2++; //A�ade el caracter recibido
                    }
                    
               }
               else 
               {
                     receivedChars[ndx] = '\0'; // Termina el string
                     buffer[ndx2] = '\0';
                     if(!noNumber){
                     values[idx] = atol(receivedChars);//Guarda el ultimo dato enviado
                     checksum =  values[idx]; //Guarda el valor del checksum
                     }
                     recvInProgress = false;
                     totalValues = idx;
                     ndx = ndx2 = idx = 0;
                     newData = true;
               }
          }
          else if (rc == startMarker) { 
          recvInProgress = true; 
          } //Si el inicio de la cadena de caracteres es correcto
     }
}
void processCommand()
{
    static unsigned int32 VerifyChecksum;
    newData = false;
    if (DEBUG)  {   
    printf("Datos recibidos =%s\n",buffer);
    }
    if(!noNumber){
    for(i=0;i<totalValues;i++){
     if (DEBUG) printf("Value%Lu=%Lu\n",i,values[i]); 
    VerifyChecksum += values[i];
    }
    if( VerifyChecksum == checksum){
      if (DEBUG) printf("Checksum[CORRECT]\n");
      duty1 = values[0];
      duty2 = values[1];
      set_pwm5_duty(duty1); 
      set_pwm6_duty(duty2);
     // printf("OK");
    }else{
      if (DEBUG) printf("Checksum[INCORRECT]\n");
     // printf("ERROR"); 
    }
    }else{
       //Procesar comandos
       if      (strcmp (cmd1,receivedChars) == 0)  {
       output_high(PIN_A0);
      set_pwm5_duty(0); 
      set_pwm6_duty(0); 
       }else  if      (strcmp (cmd2,receivedChars) == 0){
        output_low(PIN_A0);
      set_pwm5_duty(255); 
      set_pwm6_duty(255); 
       }
      
    }
    
    noNumber = false; //Evita que envie los datos sin son comandos
    VerifyChecksum  = checksum = 0;
}
/********************************************************/
/********************************************************/
#int_timer0
    void intTimer0(){
      set_timer0( 64036 ); 
       recvWithStartEndMarkers();   //Lee el puerto serial si es enviado con el formato '< comando / valores >'            
    if (newData){   processCommand();  }   //Si se recibio los datos o comandos correctos se procesa la informaci�n
 
    }
    
/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
#zero_ram
void main(){
PLLEN = 1;          //Habilita PLL para generar 48MHz de oscilador*/\ 
   setup_adc_ports (sAN9 | sAN10);
   setup_adc(adc_clock_internal);//Selecciona el reloj de conversion
   printf("<0,0,0>");
   delay_ms(1000);
   setup_timer_0( RTCC_INTERNAL | RTCC_DIV_2 );
   set_timer0( 64036 ); 
   enable_interrupts( INT_TIMER0 );
   enable_interrupts( GLOBAL );
   delay_us(1060);
 //Crea el comando de apagar todo
   strcpy(cmd1,"OFFALL"); 
   strcpy(cmd2,"ONALL"); 
   newData = false;
      //Configuracion de PWM
   setup_timer_2(T2_DIV_BY_16,74,1);   // 10KHz
   setup_ccp5(CCP_PWM); //Captura o compara o PWM  PIN B5    
   setup_ccp6(CCP_PWM); // PIN B6
   setup_ccp7(CCP_PWM); // PIN B7
   set_pwm5_duty(0);  //Inicia PWM en 0 , PWM 8 bits ( 0 - 255)
   set_pwm6_duty(0);  //Inicializar PWM
   set_pwm7_duty(0);  //Inicializar PWM
   for(;;){
   set_adc_channel(9); //Selecciona el canal del ADC para leer la temperatura
   delay_us(50);
   temp = read_adc() * 1.29;
   set_adc_channel(10); //Selecciona el canal del ADC para leer la temperatura
   delay_us(50);
   luz = read_adc();
   if(luz <= 4) luz = 1000;
   else if(luz >= 230){luz = 0;
   }else{
   luz = (unsigned int16)(127 - luz)*8.2;
   }
   int16 check = (int16)(luz + temp);
   printf("<%u,%Lu,%Lu>",temp,luz,check);
   delay_ms(100);
   }  
}


