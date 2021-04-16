#ifndef DHT22_H
#define DHT22_H

#include "stm32f10x.h"
#include "frwrapper.h"
#include "irq.h"
#include "timer.h"

class DHT22_interrupt:public Interruptable {
public:
    DHT22_interrupt();
    static DHT22_interrupt* pThis;
     
    void setActiveIrq(void) {pThis=this;}
    uint8_t getData_DHT11();
    uint8_t getData_DHT22();
    void init();
    uint8_t data[5] = {0};
    float temperature = 0;
    float humidity = 0;
private:
    volatile void InterruptHandle();
    void delay_us(uint32_t us);
    void receiving_init();
    volatile uint32_t us_counter = 0;
    inline void pin_on() __attribute__((__always_inline__));
    inline void pin_off() __attribute__((__always_inline__));
    inline void pin_on_B0() __attribute__((__always_inline__));
    inline void pin_off_B0() __attribute__((__always_inline__));
    inline bool isPinB10High() __attribute__((__always_inline__)) {return GPIOB->IDR & GPIO_IDR_IDR10;}
    inline bool isPinB0High() __attribute__((__always_inline__)) {return GPIOB->IDR & GPIO_IDR_IDR0;}
};

class DHT22_FR: public iTaskFR, DHT22_interrupt {
public:
    DHT22_FR(QueueOS<float,2>* q = nullptr);
    void run() override;
    QueueOS<float,2>* queue_float = nullptr;
private:
    //void init();
};

#endif //DHT22_H