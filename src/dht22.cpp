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
    //pin_off(); pin_on(); delay_us(10000); //start condition not always needed
    //1-й байт: относительная влажность — целая часть в %;
    //2-й байт: десятая часть относительной влажности в % (ноль для DHT11);
    //3-й байт: целая часть температуры в °C;
    //4-й байт: десятая часть температуры в °C (ноль для DHT11);
    //5-й байт: контрольная сумма (последние 8 бит {1-й байт + 2-й байт + 3-й байт + 4-й байт})
    pin_off(); 
    delay_us(18000);
    pin_on();
    volatile uint32_t timeout = 0;
    //delay_us(40); // wait for answer
    timeout = 10000;
    while (isPinHigh() && timeout--); //wait for first answer
    if(isPinHigh()) {
        return 0;
    }
    timeout = 10000;
    while (!isPinHigh() && timeout--); //wait for first answer
    if(!isPinHigh()) {
        return 0;
    }
    //delay_us(70); // wait for answer
    //delay_us(70); // wait for answer
    timeout = 10000;
    while (isPinHigh() && timeout--); //wait for first bit
    for(int j=0;j<5;j++) {
        data[4-j]=0;
        for(int i=0; i<8; i++) {
            timeout = 100000;
            while (!isPinHigh() && timeout--); //wait for start of bit receiving condition
            delay_us(30);
            if(isPinHigh()) {
                data[4-j] |= (1<<(7 - i));
                timeout = 100000;
                while (isPinHigh() && timeout--); //wait for end of bit
            }            
        }
    }
    uint8_t sum = data[4]+data[3]+data[2]+data[1];
    if(sum != data[0]) {
        return 0;
    }    
    //temperature = (float)((*(uint16_t*)(data+1)) & 0x3FFF) / 10; //DHT22
    temperature = (float)(*(uint8_t*)(data+2)) + (float)(*(uint8_t*)(data+1))/10;
    if((*(uint16_t*)(data+1)) & 0x8000) {
        temperature *= -1.0;
    }
    //humidity = (float)(*(int16_t*)(data+3)) / 10; //DHT22
    humidity = (float)(*(uint8_t*)(data+4)) + (float)(*(uint8_t*)(data+3))/10;
    
    return 1;
}

void DHT22_interrupt::receiving_init() {
    pin_off();
    delay_us(2000);
    pin_on();
}

void DHT22_interrupt::InterruptHandle() {
    us_counter+=1;
}

//! on TIM2 interrupt
void DHT22_interrupt::delay_us(uint32_t us) {    
    us_counter = 0;
    Timer::pThis[2]->tim2 = 0;
    while(Timer::pThis[2]->tim2 < us) {
        __ASM("nop");
    }         
}


void DHT22_interrupt::pin_on() {
    GPIOB->BSRR |= GPIO_BSRR_BS10;
}
void DHT22_interrupt::pin_off() {
    GPIOB->BSRR |= GPIO_BSRR_BR10;
}

//! FreeRtos class
DHT22_FR::DHT22_FR(QueueOS<float,2>* q) {
    queue_float = q;
}

void DHT22_FR::run() {
    InterruptSubject<TIM2_IRQn> us;
    __disable_irq();
    Timer tim_us(2);
    DHT22_interrupt dht_i;
    //us.setInterrupt(&dht_i);
	//us.SetVector();
    __enable_irq();
    OS::scheduler_suspend();
    dht_i.init();
    OS::scheduler_resume();    
    while(1) {
        //dht_i.temperature +=0.01;
        OS::scheduler_suspend();
        if(dht_i.getData()) {            
            queue_float->queueFrom(dht_i.temperature,1);
            queue_float->queueFrom(dht_i.humidity,1);
        }        
        //dht_i.humidity += 2;
        //queue_float->queueFrom(dht_i.humidity,1);
        OS::scheduler_resume();
        OS::sleep(1000);
    } 
}

void DHT22_FR::init() {

}