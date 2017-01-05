#include "EMm47J53_StackConfig.h"
#use rs232(baud=9600,UART1)
#define DEC 0x00
#define HEX 0x01
/********************************************************/
/*------- Espacio para declaracion de constantes  ------*/
/********************************************************/


/********************************************************/
/*--- Espacio para declaracion de variables globales  --*/
/********************************************************/

/********************************************************/
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
#include <usb_cdc.h>
#include <math.h>
#include <stdlib.h>
//meda library, a compatable media library is required for FAT.
#use fast_io(b)
#define MMCSD_PIN_SCL     PIN_B6 //o
#define MMCSD_PIN_SDI     PIN_B7 //i
#define MMCSD_PIN_SDO     PIN_B5 //o
#define MMCSD_PIN_SELECT  PIN_B4 //o
#include <mmcsd.c>
#define STREAM_SERIAL_INPUT   usb_cdc_put
//!//#include <input.c>
//FAT library.
#include <fat.c>
void MakeFile(char *fileName)
{
   printf(usb_cdc_putc,"\r\nMaking file '%s': ", fileName);
   if(mk_file(fileName) != GOODEC)
   {
      printf(usb_cdc_putc,"Error creating file");
      return;
   }
   printf("OK");
}

/******************************************************************************/
/******************************************************************************/
/*--------------------- Espacio de codigo principal --------------------------*/
/******************************************************************************/ 
char buffer[255] = "Log.txt";
#zero_ram
void main(){
mcu_init();                                // Inicializa microcontrolador
usb_cdc_init();                            // Inicializa servicios USB clase CDC
usb_init();    // Inicializa modulo USB
for(;;){
if (usb_enumerated()) {              // Espera que PUERTO COM sea abierto
        usb_task();                       // Actualiza FSM USB-CDC
   
   
   if (fat_init())
   {
      printf(usb_cdc_putc,"Could not init the MMC/SD!!!!");
      while(TRUE);
   }else{
   MakeFile(buffer);
   //printf(usb_cdc_putc,"\r\n\nDirectory created successful\r\n");
   }

     }
}
             
}

void println(int32 x,unsigned int8 system=0){
if(!system){
 printf(usb_cdc_putc," %ld \n\r", x);}
else{
if(x<=255){
 printf(usb_cdc_putc," %X \n\r", x);}else if(x<=65536){
  printf(usb_cdc_putc," %4X \n\r", x);
 }else{
 printf(usb_cdc_putc," %lX \n\r", x);
 }
}
}


