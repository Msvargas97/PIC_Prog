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




#define LOADER_END   0xFFF                        
#build(reset=LOADER_END+1, interrupt=LOADER_END+9)   //Protege posiciones de memoria desde la 0x0000 hasta la 0x1000   
#org 0, LOADER_END {}

#bit PLLEN = 0xf9b.6

#use  rs232(baud=38400,parity=N,UART1,bits=8,timeout=30)

#use standard_io(d)

 int16 temper;
char valor;
#int_rda
void serial_isr(){
valor=getc();//recibe el dato del pc y lo guarda en valor
/////////////////////codigo del pc


switch (valor)
      {
      case 'A':
      Output_low(PIN_D0);   //APAGO LED
      printf("A");
      break;
      
      case 'B':
      Output_high(PIN_D0);   //PRENDO
       printf("B"); //saca un letrero en el hiperterminal
      break;
      
        
     while(valor=='C')
     {
     output_high(pin_B3);
     delay_us(600);
     output_low(pin_B3);
     delay_us(19400);
     
} 

while(valor=='D')
     {
     output_high(pin_B3);
     delay_us(19400);
     output_low(pin_B3);
     delay_us(600);
     
} 

      break;
      
      case '1':
      Output_low(PIN_D1);   //se APAGA LED
       printf("1");//saca un letrero en el hiperterminal
      break;
      
      case '2':
      Output_high(PIN_D1);   
       printf("2"); //saca un letrero en el hiperterminal
      break;
     }

}
void main()
{
enable_interrupts(global);
enable_interrupts(int_rda);//HABILITO LAS INTERRUPCIONES PARA DETECTAR SERIAL
Output_low(PIN_D0); 
Output_low(PIN_D1); 

while(true) 
{
setup_adc (adc_clock_internal);//temperatura
setup_adc_ports (all_analog);//temperatura
set_adc_channel (0)  ;             //temperatura
temper=read_adc () ; //temperatura

       printf("%Lu\n", temper);//envia temperatura
       
          
     }
     
      
    
}


  
      
