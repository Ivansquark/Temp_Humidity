#include "main.h"


void *__dso_handle = nullptr; // dummy
//int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);
//void __cxa_finalize(void *f);

// static objects must initialize in startup file with __libc_init_array() !!!//
QueueOS<uint32_t,2> queue1; // !!! create queue of int32_t (in bss near FreeRTOS heap2) 

class AdcTemp: public iTaskFR
{
public:
    void run() override    // task creation
    {		
        Adc adc;    // in freeRTOS stack
        float x=0;
		//uint32_t* y =new uint32_t; //in freeRTOS heap
		uint32_t y;	// in freeRTOS stack
        while(1) 
        {
            x=adc.Temp();
			y=*(reinterpret_cast<uint32_t*>(&x)); //float* to uint32_t*
			queue1.queueFrom(y,2); 
			OS::sleep(100);
        }
    }
};
class Display: public iTaskFR {
public:
	Display(){}		
	void run() override // task creation
	{
		LcdParIni lcd;
		Font_30x40 font;
		uint32_t x=0;
		uint32_t x1=0;
		float y=0;
		while(1)
		{
			x1++;
			font.setTemperature(x1/1.1F);
			font.drawTemperature();
			queue1.queueRecieve(x,10);
			y=*reinterpret_cast<float*>(&x);
			font.setHumidity(y*1.F);
			font.drawHumidity();
			OS::sleep(300);
		}
	}
};

int main() {	
	uint32_t* x = new uint32_t;
	*x=0x12345678;	
	gpioc_ini();
	RCCini rcc(72);
	heap_fill_zeroes();
	Timer* tim4 = new Timer(4);
	Blink* bl = new Blink;
	InterruptSubject<TIM4_IRQn>* mig = new InterruptSubject<TIM4_IRQn>;
	mig->setInterrupt(bl);
	mig->SetVector();	
	Display* disp = new Display;
	//AdcTemp* temp = new AdcTemp;
	OS::taskCreate(disp,"LCD",1000,1);
	//OS::taskCreate(temp,"TEMP",400,2);
	__enable_irq();
	OS::startScheduler();	
	
	while(1) {		
		GPIOC->ODR^=GPIO_ODR_ODR13;	
		for(uint32_t i=0;i<7200;i++){}		
	}		
    return 0;
}
