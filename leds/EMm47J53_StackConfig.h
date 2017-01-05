/*
** ##########################################################################
**     Filename  : EMm47J53_StackConfig.h
**     Processor : PIC18F47J53
**     FileFormat: V1.00
**     DataSheet : PIC18F47J53
**     Compiler  : CCS Compiler V4.114
**     Date/Time : 14.12.2011, 11:00
**
**
**
**
**     (c) Copyright OCTOPLUS.2008-2011
**     OCTOPLUS. 
**     Ernesto Andres Rincon Cruz
**     Republica de Colombia
**     http      : www.octoplusaz.com
**     mail      : soporte@octoplusaz.com
** 
**
**     Nota:  Todas las funciones estan debidamente probadas usando el 
**            modulo EMm47J53 corriendo a una frecuencia de bus interno 
**            de 12MHz. Para mas informacion www.octoplusaz.com/foros
** ###########################################################################
*/
#ifndef _INC_OCTOPLUS_STACK_CONFIG_H
#define _INC_OCTOPLUS_STACK_CONFIG_H
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

#pin_select U2TX=PIN_D2 //Selecciona hardware UART2
#pin_select U2RX=PIN_D3 //Selecciona hardware UART2


#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6

#define mcu_init()\
do{\
 PLLEN = 1;          /*Habilita PLL para generar 48MHz de oscilador*/\
}while(0)

#endif /* _INC_OCTOPLUS_STACK_CONFIG_H */
