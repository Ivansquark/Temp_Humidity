#include "dht22.h"

DHT22_interrupt* DHT22_interrupt::pThis = nullptr;

DHT22_interrupt::DHT22_interrupt() {
    pThis = this;
    init();
}

void DHT22_interrupt::init() {
    // B10 data PIN (push-pull)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH |= GPIO_CRH_CNF10_0; //0:1 open Drain output
    GPIOB->CRH |= GPIO_CRH_MODE10; // max speed
    GPIOB->ODR |= GPIO_ODR_ODR10; // high level
}

void DHT22_interrupt::InterruptHandle() {
    us_counter++;
}

//! on TIM2 interrupt
void DHT22_interrupt::delay_us(uint32_t us) {
    OS::scheduler_suspend();
    if(us_counter < us) {
        __ASM("nop");
    } else {
        us_counter = 0;
        OS::scheduler_resume();    
    }
}


void DHT22_interrupt::pin_on() {
    GPIOB->BSRR |= GPIO_BSRR_BS10;
}
void DHT22_interrupt::pin_off() {
    GPIOB->BSRR |= GPIO_BSRR_BR10;
}


void DHT22_FR::run() {
    InterruptSubject<TIM2_IRQn> us;
    Timer tim_us(2);
    DHT22_interrupt dht_i;
    us.setInterrupt(&dht_i);
	us.SetVector();

    while(1) {

    } 
}