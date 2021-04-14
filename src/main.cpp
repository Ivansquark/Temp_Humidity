#include "main.h"


void *__dso_handle = nullptr; // dummy
//int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);
//void __cxa_finalize(void *f);

// static objects must initialize in startup file with __libc_init_array() !!!//


QueueOS<uint32_t,2> queue1; // !!! create queue of int32_t (in bss near FreeRTOS heap2) 
QueueOS<float,2> queue_float;

class AdcTemp: public iTaskFR
{
public:
    void run() override {		
        Adc adc;    // in freeRTOS stack
        float x=0;
		//uint32_t* y =new uint32_t; //in freeRTOS heap
		uint32_t y;	// in freeRTOS stack
        while(1) {
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
	void run() override {
		LcdParIni lcd;
		Font_30x40 font;
		uint32_t x=0;
		uint32_t x1=0;
		float temp=0;
		float hum=0;
		while(1) {
			x1++;
			queue_float.queueRecieve(temp,1);
			queue_float.queueRecieve(hum,1);
			font.setTemperature(temp);
			font.drawTemperature();
			queue1.queueRecieve(x,10);
			//y=*reinterpret_cast<float*>(&x);
			font.setHumidity(hum);
			font.drawHumidity();
			OS::sleep(500);
		}
	}
};
class BlinkFR:public iTaskFR {
public:
    BlinkFR(){leds_ini();}
    ~BlinkFR()override{}
    void run() override {
        blink13();
    }
private:
    void blink13() {
        while(1) {
            GPIOC->ODR^=GPIO_ODR_ODR13;
            OS::sleep(1000);
        }        
    }
    void leds_ini() {
        RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	    GPIOC->CRH&=~GPIO_CRH_CNF13;
	    GPIOC->CRH|=GPIO_CRH_MODE13;//50MHz
    }
};


int main() {	
	uint32_t* x = new uint32_t;
	*x=0x12345678;	
	gpioc_ini();
	RCCini* rcc = new RCCini(72);
	//heap_fill_zeroes();
	//Timer* tim4 = new Timer(4);
	//InterruptSubject<TIM4_IRQn>* mig = new InterruptSubject<TIM4_IRQn>;
	BlinkFR* bl = new BlinkFR();
	//mig->setInterrupt(bl);
	//mig->SetVector();
	
	
		
	
	Display* disp = new Display;
	DHT22_FR* dht_FR = new DHT22_FR(&queue_float);
	//AdcTemp* temp = new AdcTemp;
	OS::taskCreate(disp,"LCD",1000,2);
	OS::taskCreate(dht_FR, "DHT",1000,1);
	OS::taskCreate(bl,"BLINK",100,1);
	//__enable_irq();
	OS::startScheduler();	
	
	while(1) {		
		GPIOC->ODR^=GPIO_ODR_ODR13;	
		for(uint32_t i=0;i<7200;i++){}		
	}		
    return 0;
}
