#include "dht22.h"

DHT22_interrupt* DHT22_interrupt::pThis = nullptr;

DHT22_interrupt::DHT22_interrupt() {
    pThis = this;
    //init();
}

void DHT22_interrupt::init() {
    // B10 data PIN (push-pull)
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH |= GPIO_CRH_CNF10_0; //0:1 open Drain output
    GPIOB->CRH |= GPIO_CRH_MODE10; // max speed
    GPIOB->ODR |= GPIO_ODR_ODR10; // high level
    receiving_init();
}

uint8_t DHT22_interrupt::getData() {
    pin_off(); pin_on(); delay_us(100000); //start condition not always needed
    pin_off(); delay_us(18000); pin_on();
    uint32_t timeout = 0x1000000;
    delay_us(39); // wait for answer
    if(isPinHigh) {
        return 0;
    }
    delay_us(80); // wait for answer
    if(isPinHigh) {
        return 0;
    }
    delay_us(80); // wait for answer
    for(int j=0;j<5;j++) {
        data[4-j]=0;
        for(int i=0; i<8; i++) {
            timeout = 1000000;
            while (!isPinHigh || timeout--); //wait for start of bit receiving condition
            delay_us(30);
            if(isPinHigh) {
                data[4-j] |= (1<<(7 - i));
                timeout = 1000000;
                while (isPinHigh || timeout--); //wait for end of bit
            }            
        }
    }
    temperature = (float)((*(uint16_t*)(data+1)) & 0x3FFF) / 10;
    if((*(uint16_t*)(data+1)) & 0x8000) {
        temperature *= -1.0;
    }
    humidity = (float)(*(int16_t*)(data+3)) / 10;
    return 1;
}

void DHT22_interrupt::receiving_init() {
    pin_off();
    delay_us(200000);
    pin_on();
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

//! FreeRtos class
void DHT22_FR::run() {
    InterruptSubject<TIM2_IRQn> us;
    Timer tim_us(2);
    DHT22_interrupt dht_i;
    us.setInterrupt(&dht_i);
	us.SetVector();
    dht_i.init();

    while(1) {

    } 
}