#ifndef ADC_H_
#define ADC_H_

#include "main.h"

class Adc
{
public:
    Adc(){adc_ini();}
    uint16_t readADC()
    {
        ADC1->CR2|=ADC_CR2_SWSTART;
        while(!(ADC1->SR&ADC_SR_EOC)){;}
        return ADC1->DR;
    }   
    float Temp()
    {
        uint16_t V = readADC();
        return float((1177100.F)/(3950.F + 298*log(1.F/(10.F*((4095.F/float(V)) -1.F) ))) - 275.F);
    }
private:
    void adc_ini()
    {
        //-------- PIN init --------------------
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOB->CRL&=~GPIO_CRL_CNF0;
        GPIOB->CRL&=~GPIO_CRL_MODE0; //00::00 analog mode

        //------- ADC init -------------------
        RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
        RCC->APB2ENR|=RCC_CFGR_ADCPRE_DIV2; /*!< PCLK2 divided by 2 no less value*/

        ADC1->CR2|=ADC_CR2_CAL;
        while(!(ADC1->CR2 & ADC_CR2_CAL)){;}   // waiting for calibration ends
        //ADC1->CR2|=ADC_CR2_SWSTART; // by software
        ADC1->CR2|=ADC_CR2_EXTTRIG; //on external trigger
        ADC1->CR2|=ADC_CR2_EXTSEL;  // on SWSTART trigger
        ADC1->SMPR2|=ADC_SMPR2_SMP8; // 1:1:1 239 samples
        ADC1->SQR3|=ADC_SQR3_SQ1_3; //8-eight channel one conversion
        ADC1->CR2|=ADC_CR2_ADON;
        for(uint8_t i=0;i<100;i++){;}

        ADC1->CR2|=ADC_CR2_CAL;
        while(!(ADC1->CR2 & ADC_CR2_CAL)){;}   // waiting for calibration ends
    }
};


#endif //ADC_H_