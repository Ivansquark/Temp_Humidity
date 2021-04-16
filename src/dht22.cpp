#include "dht22.h"
DHT22_interrupt* DHT22_interrupt::pThis = nullptr;

DHT22_interrupt::DHT22_interrupt() {
    pThis = this;
    //init();
}

uint8_t DHT22_interrupt::getData_DHT11() {
    pin_off(); 
    delay_us(8000);
    pin_on();
    volatile uint32_t timeout = 0;
    //delay_us(40); // wait for answer
    timeout = 10000;
    while (isPinB10High() && timeout--); //wait for first answer
    if(isPinB10High()) {
        return 0;
    }
    timeout = 10000;
    while (!isPinB10High() && timeout--); //wait for first answer
    if(!isPinB10High()) {
        return 0;
    }
    timeout = 10000;
    while (isPinB10High() && timeout--); //wait for first bit
    for(int j=0;j<5;j++) {
        data[4-j]=0;
        for(int i=0; i<8; i++) {
            timeout = 10000;
            while (!isPinB10High() && timeout--); //wait for start of bit receiving condition
            delay_us(20);
            if(isPinB10High()) {
                data[4-j] |= (1<<(7 - i));
                timeout = 10000;
                while (isPinB10High() && timeout--); //wait for end of bit
            }            
        }
    }
    uint8_t sum = data[4]+data[3]+data[2]+data[1];
    if(sum != data[0]) {
        return 0;
    }    
    temperature = (float)(*(uint8_t*)(data+2)) + (float)(*(uint8_t*)(data+1))/10;
    if((*(uint16_t*)(data+1)) & 0x8000) {
        temperature *= -1.0;
    }
    humidity = (float)(*(uint8_t*)(data+4)) + (float)(*(uint8_t*)(data+3))/10;    
    return 1;
}

uint8_t DHT22_interrupt::getData_DHT22() {
    pin_on_B0(); 
    delay_us(8000);
    pin_on_B0();
    volatile uint32_t timeout = 0;
    timeout = 10000;
    while (isPinB0High() && timeout--); //wait for first answer
    if(isPinB0High()) {
        return 0;
    }
    timeout = 10000;
    while (!isPinB0High() && timeout--); //wait for first answer
    if(!isPinB0High()) {
        return 0;
    }
    timeout = 10000;
    while (isPinB0High() && timeout--); //wait for first bit
    for(int j=0;j<5;j++) {
        data[4-j]=0;
        for(int i=0; i<8; i++) {
            timeout = 10000;
            while (!isPinB0High() && timeout--); //wait for start of bit receiving condition
            delay_us(20);
            if(isPinB0High()) {
                data[4-j] |= (1<<(7 - i));
                timeout = 10000;
                while (isPinB0High() && timeout--); //wait for end of bit
            }            
        }
    }
    uint8_t sum = data[4]+data[3]+data[2]+data[1];
    if(sum != data[0]) {
        return 0;
    }    
    temperature = (float)((*(uint16_t*)(data+1)) & 0x3FFF) / 10; //DHT22
    if((*(uint16_t*)(data+1)) & 0x8000) {
        temperature *= -1.0;
    }
    humidity = (float)(*(uint8_t*)(data+4)) + (float)(*(uint8_t*)(data+3))/10;    
    return 1;
}

volatile void DHT22_interrupt::InterruptHandle() {
    us_counter+=1; //very slow
}

//! on TIM2 interrupt
void DHT22_interrupt::delay_us(uint32_t us) {    
    us_counter = 0;
    Timer::pThis[2]->tim2 = 0;
    //while(Timer::pThis[2]->tim2 < us) {
    //    __ASM("nop");
    //}   
    while(us_counter < us) {
        __ASM("nop");
    }      
}

void DHT22_interrupt::pin_on() {
    GPIOB->BSRR |= GPIO_BSRR_BS10;
}
void DHT22_interrupt::pin_off() {
    GPIOB->BSRR |= GPIO_BSRR_BR10;
}
void DHT22_interrupt::pin_on_B0() {
    GPIOB->BSRR |= GPIO_BSRR_BS0;
}
void DHT22_interrupt::pin_off_B0() {
    GPIOB->BSRR |= GPIO_BSRR_BR0;
}

void DHT22_interrupt::init() {
    // B10 data PIN (push-pull) DHT-11
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH |= GPIO_CRH_CNF10_0; //0:1 open Drain output
    GPIOB->CRH |= GPIO_CRH_MODE10; // max speed
    GPIOB->ODR |= GPIO_ODR_ODR10; // high level
    // B10 data PIN (push-pull) DHT-11
    GPIOB->CRL |= GPIO_CRL_CNF0_0; //0:1 open Drain output
    GPIOB->CRL |= GPIO_CRL_MODE0; // max speed
    GPIOB->ODR |= GPIO_ODR_ODR0; // high level
    receiving_init();
}

void DHT22_interrupt::receiving_init() {
    pin_off(); pin_off_B0();
    delay_us(200000);
    pin_on(); pin_on_B0();
}
//!____________________ FreeRtos class _________________________
DHT22_FR::DHT22_FR(QueueOS<float,2>* q) {
    queue_float = q;
}

void DHT22_FR::run() {
    //InterruptManager interMan;
    volatile InterruptSubject<TIM2_IRQn> us;
    __disable_irq();
    Timer tim_us(2);
    DHT22_interrupt dht_i;
    us.setInterrupt(&dht_i);
    us.SetVector();    
    OS::scheduler_suspend();
    __enable_irq();
    dht_i.init();
    OS::scheduler_resume();    
    while(1) {
        OS::scheduler_suspend();
        if(dht_i.getData_DHT22()) {            
            queue_float->queueFrom(dht_i.temperature,1);
            queue_float->queueFrom(dht_i.humidity,1);
        } else if(dht_i.getData_DHT11()) {            
            queue_float->queueFrom(dht_i.temperature,1);
            queue_float->queueFrom(dht_i.humidity,1);
        }        
        OS::scheduler_resume();
        OS::sleep(100);
    } 
}