

#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#include <xc.h>
#define _XTAL_FREQ 4000000



void duty_gir(int deger1){
CCP1X = deger1 & 2;   //duty nin dusuk degerlikli bitleri bu register lara yazilacak.
CCP1Y = deger1 & 1;    
CCPR1L = deger1>>2;   //duty nin yüksek degerlikli 8 biti bu register'a yazilacak.
}  

PWM1_TMR2_giris()
{
  CCP1M3 = 1;     // capture   pwm modunda. Mod secme bitleri
  CCP1M2 = 1;
   // Prescaler 4 
    T2CKPS0 = 1;
    T2CKPS1 = 0;
  
  TMR2ON = 1;  // TM2 açma biti
  TRISC2 = 0;  // PWM çikisini kullanabilmek için CCP ucunu çikis yaptim.
}

void ADC_Deger()
{
  ADCON1=0b11000000; // AN ler analog, ADFM=1,ADcs2=1
  ADCON0=0b00001001; //fosc/4, AN1 giris , GODONE=0, ADON=1
}

void main(void) {
    unsigned int adc_deger,deger=0;
    double volt_degeri;
PR2=249;     // PWM periyodu= (PR2+1)*4*Tosc*(TMR2Prescaler degeri) 

// PWM sinyali a?a??daki ad?mlarda olu?ur;
//- TMR2, PR2?ye e?it oldu?unda TMR2 temizlenir,
//- CCPx pini set edilir (Duty cycle=%0 ise set edilmez),
//- CCPRxL?den CCPRxH?a yükleme gerçekle?ir.
//- Duty cycle, PWM periyodundan büyük olamaz.


duty_gir(0);
PWM1_TMR2_giris();

TRISA = 0xFF; // PORTA input

ADC_Deger();

while(1){
ADCON0bits.GO=1; // çevrime basla
while (ADCON0bits.GO); //bitene kadar bekle
adc_deger=(ADRESH<<8)+ADRESL; //ADC de?eri de?i?kene at

volt_degeri=adc_deger*0.004882; //Degeri ölçülen volta çevir. ADC den okunan deger* adim degeri

deger=(volt_degeri*1000/5);   // volt degeri duty e göndermek icin max. 1000 olan duty seviyesine gore hesaplamasi
// MAX duty= (Kaydediciler)* Tosc * (TMR2 Prescaler degeri)

if(deger<80){
    deger=0;
}
if(deger>930){
    deger=1000;
}
duty_gir(deger);
}
return;
}