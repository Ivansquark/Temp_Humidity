#ifndef TIMER_H
#define TIMER_H

#include "irq.h"
//#include "main.h"
//#include "stm32f10x.h"
//---------------------------------------------------------------
extern "C" void TIM2_IRQHandler(void); // 1MHz
extern "C" void TIM3_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
//----------------------------------------------------------------------

//---------------- General purpose timers 2-3-4---------------
class Timer
{
public:
    Timer(uint8_t timerNum);
    static Timer* pThis[4];    
    void delay_us(uint32_t us);
private:
    void timer_ini(uint8_t timerNum);
    uint32_t us_counter = 0;
};

#endif //TIMER_H