#ifndef FRWRAPPER_H_
#define FRWRAPPER_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "portmacro.h"
//#include "main.h"

class iTaskFR
{
public: 
    virtual ~iTaskFR(){}
    virtual void run()=0;
    TaskHandle_t handle=nullptr;   //pointer to task control structure of freertos
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
class OS {
public:
/*************************- task -****************************************************************/
    static void taskCreate(iTaskFR* obj, const char *pName,const uint16_t stackDepth,uint8_t priority);
    //{ xTaskCreate(reinterpret_cast<TaskFunction_t>(Run),pName,stackDepth,&obj,priority,NULL);}
    
/*************************- queue -**************************************************************/    
    static QueueHandle_t queueCreate(uint8_t length, uint8_t size);    
    static bool queueSend(QueueHandle_t &queue, const void *pItemToQueue,uint16_t timeout);
    static bool queueSendISR(QueueHandle_t &queue, const void *pItemToQueue);
    static bool queueRecieve(QueueHandle_t &queue,void* pItem, uint32_t timeout);
    static void queueDelete(QueueHandle_t &queue);
/********************************- semaphore -*************************************************/
    static SemaphoreHandle_t semaphoreCreate();
    static void semFromIsr(SemaphoreHandle_t handle);
    static bool semRecieve(SemaphoreHandle_t handle, uint16_t timeout);
    static void deleteSemaphore(SemaphoreHandle_t handle);    
/********************************- critical section -********************************************/
    static void scheduler_suspend();
    static void scheduler_resume();
    static void criticalSectionEnter(); // prohibition interrupts and sysTick    
    static void criticalSectionLeave();
    static void criticalSectionEnterISR();
    static void criticalSectionLeaveISR(uint32_t x);
/********************************- scheduller -**************************************************/
    static void startScheduler();
    static void sleep(uint16_t x);
private:
    //static TaskFunction_t Run(void* x)  // void(*Run)(void*)    
    static void Run(void* x);    
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T,uint16_t length>
class QueueOS
{
public:
    QueueOS();
    ~QueueOS();
    bool queueFrom(const T &item,uint16_t timeout);
    bool queueFromIsr(const T &item);
    bool queueRecieve(T &item, uint32_t timeout);
private:
    QueueHandle_t queueHandle; //указатель на структуру очереди
};
class SemaphoreOS
{
public:
    SemaphoreOS();
    ~SemaphoreOS();
    void giveFromIsr();
    bool takeSem(uint16_t timeout);
private:
    SemaphoreHandle_t semHandle;
    //StaticSemaphore_t mutex;
};
/*******************************************************************************************/
#endif //FRWRAPPER_H_
