#include <math.h>
#define SAMPLES 16 //Número de muestras
float Fs=0;
struct waveStruct{
unsigned int16 values[SAMPLES];
int16 period;
}wave;

               
void senox(float freq){
static float x;
static unsigned char j;
wave.period =(((1/freq) - 0.000870f )*1e+6) / SAMPLES;
Fs = (float)(1/(double)(SAMPLES*freq)); 
for(j=0;j<SAMPLES;j++){
 x=2*pi*freq*j*Fs;
 wave.values[j]= 2047*sin(x)+ 2047;
 if(wave.values[j] > 4095 ) wave.values[j] = 4095;
}
}
void sawTooth(float freq){
static int16 saw;
static unsigned char j;
wave.period =(((1/freq) - 0.000870f )*1e+6) / SAMPLES;
for(j=0;j<SAMPLES;j++){
   if(saw>4095) saw = 0;
   else saw+=(4095/SAMPLES);
   wave.values[j] = saw;
}
}
void triangular(float freq){
static int16 trian=0;
static unsigned char j;
wave.period =(((1/freq) - 0.000870f )*1e+6) / SAMPLES;
for(j=0;j<(SAMPLES/2);j++)
{
wave.values[trian] = j*(4095/(SAMPLES*0.5));
trian++;
}
for(j=(SAMPLES/2);j>0;j--)
{
wave.values[trian] = j*(4095/(SAMPLES*0.5));
trian++;
}
}
void square(float freq){
static unsigned char j;
wave.period =(((1/freq) - 0.000870f )*1e+6) / SAMPLES;
static int16 sq=0;
for(j=0;j<(SAMPLES/2);j++)
{
wave.values[sq] = 4095;
sq++;
}
for(j=0;j<(SAMPLES/2);j++)
{
wave.values[sq] = 0;
sq++;
}
}
