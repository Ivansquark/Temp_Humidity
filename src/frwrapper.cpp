#include "frwrapper.h"

void OS::taskCreate(iTaskFR* obj, const char *pName,const uint16_t stackDepth,uint8_t priority) {
    obj->handle = (xTaskCreate(Run,pName,stackDepth,obj,priority,&obj->handle)==pdTRUE) ? 
                   obj->handle : nullptr;}
/*************************- queue -**************************************************************/    
QueueHandle_t OS::queueCreate(uint8_t length, uint8_t size) {
    return xQueueCreate(length,size);
}
bool OS::queueSend(QueueHandle_t &queue, const void *pItemToQueue,uint16_t timeout) {
    return (xQueueSend(queue, pItemToQueue, timeout)==pdTRUE);// == returns true, if 1 equal 2, else - false 
} 
bool OS::queueSendISR(QueueHandle_t &queue, const void *pItemToQueue) {
    return (xQueueSendFromISR(queue, pItemToQueue, nullptr)==pdTRUE);
}    
bool OS::queueRecieve(QueueHandle_t &queue,void* pItem, uint32_t timeout) {
    return (xQueueReceive(queue,pItem,timeout)==pdTRUE);
}
void OS::queueDelete(QueueHandle_t &queue) {
    vQueueDelete(queue);
}
/********************************- semaphore -*************************************************/
SemaphoreHandle_t OS::semaphoreCreate() {
    return xSemaphoreCreateBinary();
}
void OS::semFromIsr(SemaphoreHandle_t handle) {
    xSemaphoreGiveFromISR(handle,nullptr);
}
bool OS::semRecieve(SemaphoreHandle_t handle, uint16_t timeout) {
    return xSemaphoreTake(handle, timeout);
}
void OS::deleteSemaphore(SemaphoreHandle_t handle) {
    vSemaphoreDelete(handle);
}
/********************************- critical section -********************************************/
void OS::scheduler_suspend() {
    vTaskSuspendAll();
}
void OS::scheduler_resume() {
    xTaskResumeAll();
}
void OS::criticalSectionEnter() {
    taskENTER_CRITICAL();// prohibition interrupts and sysTick    
} 
void OS::criticalSectionLeave() {
    taskEXIT_CRITICAL();
}
void OS::criticalSectionEnterISR() {
    taskENTER_CRITICAL_FROM_ISR();
    vTaskSuspendAll();
}
void OS::criticalSectionLeaveISR(uint32_t x) {
    taskEXIT_CRITICAL_FROM_ISR(x);
}
/********************************- scheduller -**************************************************/
void OS::startScheduler() {
    vTaskStartScheduler();
}
void OS::sleep(uint16_t x) {
    vTaskDelay(x);
}
//! __________ private ___________________________
void OS::Run(void* x) {
    static_cast<iTaskFR*>(x)->run(); // set class metod to function pointer
}                   


//!_______________ QUEUE __________________________________
template <typename T, uint16_t length>
QueueOS<T,length>::QueueOS() {
    queueHandle = OS::queueCreate(length,sizeof(T)); //create queue with creation class object
} 
template <typename T, uint16_t length>
QueueOS<T, length>::~QueueOS() {
    OS::queueDelete(queueHandle);
}
template <typename T, uint16_t length>
bool QueueOS<T, length>::queueFrom(const T &item,uint16_t timeout) {
    const void* x = reinterpret_cast<const void*>(&item);
    return OS::queueSend(queueHandle,x,timeout);
}
template <typename T, uint16_t length>
bool QueueOS<T, length>::queueFromIsr(const T &item) {
    const void* x = reinterpret_cast<const void*>(&item);
    return OS::queueSendISR(queueHandle,x);
}
template <typename T, uint16_t length>
bool QueueOS<T, length>::queueRecieve(T &item, uint32_t timeout) {
    void* x = reinterpret_cast<void*>(&item);
    return OS::queueRecieve(queueHandle,x,timeout);
}
template class QueueOS<float,2>;
template class QueueOS<uint32_t,2>;
//!_______________ Semaphore ___________________________________________
SemaphoreOS::SemaphoreOS() {
    semHandle=OS::semaphoreCreate();
}
SemaphoreOS::~SemaphoreOS() {
    OS::deleteSemaphore(semHandle);
}
void SemaphoreOS::giveFromIsr() {
    OS::semFromIsr(semHandle);
}
bool SemaphoreOS::takeSem(uint16_t timeout) {
    return OS::semRecieve(semHandle,timeout);
}
