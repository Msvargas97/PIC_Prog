//-----------------------------------------------------------------------------
// Title:         KEYBOARD_PC_To_RS232.c
// Description:   Interfase entre un teclado convencional tipo AT y un puerto RS232C
// Date:          Abr-2005
// Ver.Rev.:      V01
// Author:        XP8100 (xp8100@gmail.com)
//
//                #Based on the AN AVR313: Interfacing the PC AT Keyboard from ATMEL#
//                #Adaptado para 16F628A por Redraven
//
//-----------------------------------------------------------------------------
//
// init_kb()      Inicializa rutina y contadores
//
// decode (char)  Decodifica la pulsación realizada, convirtiendola a un caracter de la tabla
//
// int_ext_isr    Rutina de gestión de interrupciones. Captura los diferentes bit's
//
//-----------------------------------------------------------------------------
// RB0 - Señal de reloj
// RB3 - Tren de impulsos (11 bit) Start+10101010+Paridad+Stop
//-----------------------------------------------------------------------------
//
// Commment   : Permite conectar un teclado convencional de PC a un entorno
//              gestionado por un PIC 16F877.
//              El actual sistema se define como un primer prototipo, en el que no se realizan
//              acciones concretas asociadas a teclas establecidas.
//              Tampoco se actua sobre los indicadores luminosos del teclado, repetición de teclas, ...
//
//
//
// THIS DOCUMENT IS PROVIDED TO THE USER 'AS IS'
//-----------------------------------------------------------------------------


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
#use delay(clock=48000000) //de la primera plantilla

//#uses232(baud=115200,parity=N,UART1,bits=8,timeout=30,xmit=PIN_C6,rcv=PIN_C7)


#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6
#include <math.h>
#include <stdlib.h>



#include "Configura_LCD_4-8_bits.c"
// para el pic18f


#zero_ram

//-----------------------------------------------------------------------------
// Definiciones globales
//-----------------------------------------------------------------------------

unsigned char bitcount,i;
unsigned char auxTecla;
char got_interrupt;
char interrupt_count;
char status_b3;

#bit INTF_BIT = 0x0B.1 // INTCON BIT 1 = INTF RB0/INT External Interrupt Flag Bit

//-------- Tabla de caracteres correspondientes a la pulsación de la tecla
//-------- en modalidad normal (sin pulsar SHIFT)

unsigned char const unshifted[68][2] = {
0x0d,9,
0x0e,'º', 0x15,'q', 0x16,'1', 0x1a,'z', 0x1b,'s', 0x1c,'a', 0x1d,'w',
0x1e,'2', 0x21,'c', 0x22,'x', 0x23,'d', 0x24,'e', 0x25,'4', 0x26,'3',
0x29,' ', 0x2a,'v', 0x2b,'f', 0x2c,'t', 0x2d,'r', 0x2e,'5', 0x31,'n',
0x32,'b', 0x33,'h', 0x34,'g', 0x35,'y', 0x36,'6', 0x39,',', 0x3a,'m',
0x3b,'j', 0x3c,'u', 0x3d,'7', 0x3e,'8', 0x41,',', 0x42,'k', 0x43,'i',
0x44,'o', 0x45,'0', 0x46,'9', 0x49,'.', 0x4a,'-', 0x4b,'l', 0x4c,'ñ',
0x4d,'p', 0x4e,' ', 0x52,'´', 0x54,'`', 0x55,'¡', 0x5a,13,  0x5b,'\n',
0x5d,'ç', 0x61,'<', 0x66,' ',   0x69,'1', 0x6b,'4', 0x6c,'7', 0x70,'0',
0x71,'.', 0x72,'2', 0x73,'5', 0x74,'6', 0x75,'8', 0x79,'+', 0x7a,'3',
0x7b,'-', 0x7c,'*', 0x7d,'9',
0,0
};

/*//-------- Tabla de caracteres correspondientes a la pulsación de la tecla
//-------- en modalidad desplazamiento (pulsando SHIFT)

unsigned char const shifted[68][2] = {
0x0d,9,
0x0e,'ª', 0x15,'Q', 0x16,'!', 0x1a,'Z', 0x1b,'S', 0x1c,'A', 0x1d,'W',
0x1e,'"', 0x21,'C', 0x22,'X', 0x23,'D', 0x24,'E', 0x25,'$', 0x26,'·',
0x29,' ', 0x2a,'V', 0x2b,'F', 0x2c,'T', 0x2d,'R', 0x2e,'%', 0x31,'N',
0x32,'B', 0x33,'H', 0x34,'G', 0x35,'Y', 0x36,'&', 0x39,'L', 0x3a,'M',
0x3b,'J', 0x3c,'U', 0x3d,'/', 0x3e,'(', 0x41,';', 0x42,'K', 0x43,'I',
0x44,'O', 0x45,'=', 0x46,')', 0x49,':', 0x4a,'_', 0x4b,'L', 0x4c,'Ñ',
0x4d,'P', 0x4e,'?', 0x52,'¨', 0x54,'^', 0x55,'¿', 0x5a,13,  0x5b,'*',
0x5d,'Ç', 0x61,'>', 0x66,' ',   0x69,'1', 0x6b,'4', 0x6c,'7', 0x70,'0',
0x71,'.', 0x72,'2', 0x73,'5', 0x74,'6', 0x75,'8', 0x79,'+', 0x7a,'3',
0x7b,'-', 0x7c,'*', 0x7d,'9',
0,0
};

*/
//-----------------------------------------------------------------------------
// Definición de protipos
//---------------------------------------------------------------------------

void init_kb(void);
void decode(unsigned char sc);
unsigned char barrer_teclado();
//-----------------------------------------------------------------------------
// Rutina de gestión de interrupciones
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



//-----------------------------------------------------------------------------
// Funciones
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inicialización de teclado.
//-----------------------------------------------------------------------------
void init_kb(void)
{
   bitcount = 11;
   lcd_init_4bits(); 
}
void decode(unsigned char sc)
{
   unsigned char i;
 for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);     
        if(unshifted[i][0] == sc){
             auxTecla= (unshifted[i][1] >= 'a' && unshifted[i][1] >= 'z' ) ?  unshifted[i][1]-32 : unshifted[i][1];
             delay_ms(100);
        }
}

unsigned char barrer_teclado(){
  
 static unsigned char aux;
      if(auxTecla){
      aux = auxTecla;
       auxTecla=0;
      }else{
      aux = 0;
      }
      //-------- Chequear si se produce alguna interrupción (got_interrupt).
      if(got_interrupt == TRUE)

         {

            //-------- Borrar el flag global que se inicio en la rutina de servicio
            //-------- de interrupciones externas.
            got_interrupt = FALSE;

            //-------- Esperar 50 ms para evitar rebotes en los contactos de las teclas.
            //delay_ms(50);

            //-------- Borrar cualquier interrupción producida durante el periodo de espera.
            INTF_BIT = 0;

            //-------- Reactivar interrupciones
            enable_interrupts(INT_EXT);

         } // --- End If ---
         return aux;
         
}
