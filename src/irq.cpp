#include "irq.h"

void InterruptManager::DefaultHandler(void) {
    asm volatile ("nop"::);
}

volatile void InterruptManager:: AddHandler(void (*IsrVec)(), IRQn_Type irq) {
    IsrVectors[irq] = IsrVec;     // IsrVectors[TIM3_IRQn] = blink  func pointer
}

void InterruptManager::RemoveHandler(IRQn_Type irq) {
    IsrVectors[irq] = DefaultHandler;    
}
void InterruptManager::Call(IRQn_Type irq) {
    IsrVectors[irq]();
}
void (*InterruptManager::IsrVectors[76])(void)={InterruptManager::DefaultHandler};

