//Definicion de instruccion de la LCD
short bits_8_4;
#define LCD_LINEA2 0xC0    // Direcci�n de memoria para la segunda l�nea
#define clear_display            1
#define home                     0b10
#define entry_mode_set           0b100
#define display_on_off_control   0b1100
#define cursor_or_display_shift  0b1100
#define function_set             0b101000

#define RS_LCD        PIN_B0
#define RW_LCD        PIN_B1
#define E_LCD         PIN_B3
#define LCD_D4        PIN_D4
#define LCD_D5        PIN_D5
#define LCD_D6        PIN_D6
#define LCD_D7        PIN_D7

#define DATOS        PORTD

//-------------- Funcion que envia byte a pantalla lcd --------------------
void lcd_envia_byte_4bits(int1 operacion, char dato ) {
output_low(E_LCD);
output_low(RW_LCD);
output_bit(RS_LCD,operacion);

if(bit_test(dato,4)==1)output_high (LCD_D4);else output_low (LCD_D4);
if(bit_test(dato,5)==1)output_high (LCD_D5);else output_low (LCD_D5);
if(bit_test(dato,6)==1)output_high (LCD_D6);else output_low (LCD_D6);
if(bit_test(dato,7)==1)output_high (LCD_D7);else output_low (LCD_D7);
delay_us(10);

output_high(E_LCD);
delay_ms(1);
output_low(E_LCD);
delay_ms(1);

if(bit_test(dato,0)==1)output_high (LCD_D4);else output_low (LCD_D4);
if(bit_test(dato,1)==1)output_high (LCD_D5);else output_low (LCD_D5);
if(bit_test(dato,2)==1)output_high (LCD_D6);else output_low (LCD_D6);
if(bit_test(dato,3)==1)output_high (LCD_D7);else output_low (LCD_D7);
delay_us(10);

output_high(E_LCD);
delay_ms(1);
output_low(E_LCD);
delay_ms(1);
}
//--------- Funcion que establece la posicion del cursor en la lcd ------------
void lcd_ubicaxy_4bits(char columna,char fila) {
char posicion;
     switch (fila) {
     case 1   : posicion=(0x7F+columna);
     break;
     case 2   : posicion=(0xBF+columna);
     break;
     case 3   : posicion=(0x8F+columna);
     break;
     case 4   : posicion=(0xCF+columna);
     break;
   }
   lcd_envia_byte_4bits( 0,posicion);
}
//----------- Funcion que envia secuencia de inicializaciona lcd --------------
void lcd_init_4bits() {
    delay_ms(50);
    lcd_envia_byte_4bits(0,0B00110011);
    lcd_envia_byte_4bits(0,0B00110010);
    lcd_envia_byte_4bits(0,function_set);
    lcd_envia_byte_4bits(0,display_on_off_control);
    lcd_envia_byte_4bits(0,clear_display);
    lcd_ubicaxy_4bits(1,1);
}
//---------------------- Escribe en la lcd la letra dada ----------------------
void lcd_putc_4bits( char letra) {

     switch (letra) {
     case '\f'   : lcd_envia_byte_4bits(0,1);       //Borra la pantalla LCD
                   lcd_ubicaxy_4bits( 1,1);         //Ubica cursor en fila 1
     break;

     case '\n'   : lcd_ubicaxy_4bits(0,2);          //Busca la fila 2
     break;

     default     : lcd_envia_byte_4bits(1,letra);
     break;
   }

}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
// Comandos para el funcionamiento a 8 bits
#define LCD_DATO    1
#define LCD_COMANDO 0

// instruccion de inicializacion
#define LCD_CLEAR          0x01
#define LCD_HOME          0x02

// Opciones para el display
#define LCD_DISPLAY_OFF  0x08
#define LCD_DISPLAY_ON   0x0C
#define LCD_CURSOR_ON    0x0E
#define LCD_CURSOR_BLINK 0x0F

// opciones epeciales del cursor
#define LCD_CURSOR_SHIFT_RIGHT  0x10
#define LCD_CURSOR_SHIFT_LEFT   0x14
#define LCD_DISPLAY_SHIFT_RIGHT 0x18
#define LCD_DISPLAY_SHIFT_LEFT  0x1C

// Configurar el hardware
#define LCD_DISPLAY_8_BITS   0x38  //0b00111000
#define LCD_DISPLAY_4_BITS   0x28  //0b00111000



void lcd_envia_8bits(int dir, int valor)
{
   
       
    output_low(RW_LCD);
    output_bit(RS_LCD,dir);
    output_high(E_LCD);
    output_d(valor);  // !!!!!Cambiar al puerto donde esten conectados los datos!!!!!!
    delay_us(350);
    output_low(E_LCD);
    delay_us(350);
   
     
}


void lcd_init_8bits()
{
  int dir,valor;
  dir=LCD_COMANDO;
 
  valor=LCD_DISPLAY_8_BITS;   //0x38 Inicia el modulo a 8 bits
  bits_8_4=1;
 
  lcd_envia_8bits(dir,valor);
  delay_ms(1);   
  lcd_envia_8bits(dir,valor);

  valor=LCD_DISPLAY_ON;    //0x0c activa display
  lcd_envia_8bits(dir,valor);
  valor=LCD_CLEAR;         //0x01 borra display
  lcd_envia_8bits(dir,valor); 
  valor=0x06;               //selecciona mensaje fijo
  lcd_envia_8bits(dir,valor);

}


void lcd_ubicaxy_8bits( char x, char y) {
   char dir;

   if(y!=1)
     dir=LCD_LINEA2;
   else
     dir=0;
   dir+=x-1;
   lcd_envia_8bits(LCD_COMANDO,0x80|dir);
}


void lcd_putc_8bits( char c)
{
   switch (c)
   {
     case '\f':
         lcd_envia_8bits(LCD_COMANDO,0x01);
         delay_ms(2);
     break;
     case '\n':
         lcd_ubicaxy_8bits(1,2);
     break;
     case '\b':
        lcd_envia_8bits(LCD_COMANDO,0x10);
     break;
     default :
        lcd_envia_8bits(LCD_DATO,c);
     break;
   }
}
