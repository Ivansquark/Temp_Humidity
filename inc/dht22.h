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
    uint8_t getData();
    void init();
    uint8_t data[5] = {0};
    float temperature = 0;
    float humidity = 0;
private:
    void InterruptHandle();
    void delay_us(uint32_t us);
    void receiving_init();
    uint32_t us_counter = 0;
    inline void pin_on() __attribute__((__always_inline__));
    inline void pin_off() __attribute__((__always_inline__));
    inline bool isPinHigh() __attribute__((__always_inline__)) {return GPIOB->IDR & GPIO_IDR_IDR11;}
};

class DHT22_FR: public iTaskFR, DHT22_interrupt {
public:
    void run() override;
private:
    void init();
};

#endif //DHT22_H