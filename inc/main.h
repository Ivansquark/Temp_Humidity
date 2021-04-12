#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f10x.h"
#include "core_cm3.h"
#include "frwrapper.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "malloc.h"

#include "rcc.h"
#include "timer.h"
#include "irq.h"
#include "lcdpar.h"
#include "adc.h"
#include "dht22.h"




void gpioc_ini();
void heap_fill_zeroes();

//***************************************************

void gpioc_ini()
{
	RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	GPIOC->CRH&=~GPIO_CRH_CNF13;
	GPIOC->CRH|=GPIO_CRH_MODE13;//50MHz
	//GPIOC->ODR=0;//GPIO_ODR_ODR13;	
}

void heap_fill_zeroes()
{
	volatile uint32_t *top, *start;	
	start = (uint32_t*)&_heap_start;
	top = (uint32_t*)&_heap_end;
	while (start < top) { *(start++) = 0x00000000;} //fill user heap with zeros
	current_ends=0;
}

#endif //MAIN_H
