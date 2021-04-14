#ifndef RCC_H_
#define RCC_H_

class RCCini
{
public:
    RCCini(uint8_t freq=0)
    {
        rcc_ini(freq);
    }
private:
    void rcc_ini(uint8_t freq=0)
    {   
        RCC->CR&=~RCC_CR_PLLON;  //выключаем PLL чтобы можно было бы записывать в PLLCFGR
        RCC->CFGR=0;//обнуляем регистр настройки частоты
        RCC->CR|=RCC_CR_HSEON; // включение внешнего кварца
        while(!(RCC->CR&RCC_CR_HSERDY)){} //пока не появился флаг готовности HSE
        RCC->CFGR|=RCC_CFGR_HPRE_DIV1; // делим SYSCLK на единицу      (48MHz)
        RCC->CFGR|=RCC_CFGR_PPRE1_DIV2; // APB1 clk SYSCLK делим на 2  (24MHz)
        RCC->CFGR|=RCC_CFGR_PPRE2_DIV1; // APB2 clk SYSCLK делим на 1  (48MHz)
        switch (freq)
        {
            case 72: RCC->CFGR|=RCC_CFGR_PLLMULL9; // 8MHz HSE * 9 = 72MHz SYSCLK;
            break;
            case 48: RCC->CFGR|=RCC_CFGR_PLLMULL6; // 8MHz HSE * 6 = 48MHz SYSCLK
            break;
            default: RCC->CFGR|=RCC_CFGR_PLLMULL_1;
            break;
        }
        RCC->CR|=RCC_CR_PLLON; // включаем тактирование с PLL
        while(!(RCC->CR&RCC_CR_PLLRDY)){} // пока не появится флаг включения PLL
        //--------------------------------FLASH-------------------------------------
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE; // PRFTBE включает буфер превыборки команд
        /* Flash 2 wait state */
        FLASH->ACR &=~FLASH_ACR_LATENCY; //обнуляем
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_1; //0:0:1 Для частоты 48MHz следует ставить задержку в один такт 
        //------------------------- PLL on-------------------------------------------
        RCC->CFGR|=RCC_CFGR_SW_PLL;           //переключаемся с HSI на PLL
	    while(!(RCC->CFGR&RCC_CFGR_SWS_PLL)){} //дождемся выставления бита готовности PLL
    }
};
#endif //RCC_H_