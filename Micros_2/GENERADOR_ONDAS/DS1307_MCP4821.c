#define segundos     0
#define minutos      1
#define horas        2
#define dia_semana   3
#define dia_mes      4
#define mes          5
#define anho         6
#define control      7 

char DS1307 [64],buffer_DS1307[6]; 
/********************************************************/
/*-------------- Espacio para funciones  ---------------*/
/********************************************************/
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
void init_dac()
{
   output_high (DAC_CS) ;
   output_high (DAC_LDAC) ;
   output_high (DAC_CLK) ;
   output_high (DAC_DI) ;
}
void write_dac(int16 data) 
{
   BYTE cmd[3];
   BYTE i;
   cmd[0] = data;
   cmd[1] = (data>>8);
   cmd[2] = 0x01;
   
   output_high (DAC_LDAC) ;
   output_low (DAC_CLK) ;
   output_low (DAC_CS) ;
   for (i = 0; i <= 23;  ++i)
   {
      if (i < 4|| (i > 7&&i < 12) )
      shift_left (cmd, 3, 0) ;
      else
      {
         output_bit (DAC_DI, shift_left (cmd, 3, 0));
         output_high (DAC_CLK) ;
         output_low (DAC_CLK) ;
      }
   }
   output_high (DAC_CS) ;
   output_low (DAC_LDAC) ;
   output_HIGH (DAC_LDAC) ;
}
