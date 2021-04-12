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
private:
    void init();
    void InterruptHandle();
    void delay_us(uint32_t us);
    uint32_t us_counter = 0;
    inline void pin_on() __attribute__((__always_inline__));
    inline void pin_off() __attribute__((__always_inline__));
};

class DHT22_FR: public iTaskFR, DHT22_interrupt {
public:
    void run() override;
private:
    void init();
};

#endif //DHT22_H