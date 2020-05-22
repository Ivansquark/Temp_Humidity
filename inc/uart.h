#ifndef UART_H_
#define UART_H_

#include "main.h"

class Uart2
{
public:
    Uart2()
    {
        uart2_ini();
    }
    void sendByte(uint8_t byte)
    {
        while(!(USART2->SR & USART_SR_TXE))
        USART2->DR=byte;
    }
    void sendStr(uint8_t* str)
    {
        uint8_t i=0;
        while(str[i]!='\0')
        {
            sendByte(str[i++]);
        }
    }
private:
    void uart2_ini()
    {
        //------------------- pin A ------------------
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN; // тактирование на порт А
        //**************************Tx portA-2
        GPIOA->CRH|=GPIO_CRH_MODE9; //11: Output mode, max speed 50 MHz.
        GPIOA->CRL|=GPIO_CRL_CNF2_1;
        GPIOA->CRL&=~GPIO_CRL_CNF2_0; //10: Alternate function output Push-pull
        //**************************Rx portA-3
        GPIOA->CRL&=~GPIO_CRL_MODE3; //00: Input mode (reset state).
        GPIOA->CRL|=GPIO_CRL_CNF3_0;
        GPIOA->CRL&=~GPIO_CRL_CNF3_1; //01: Floating input (reset state)
        //------------------- USART -------------------------------------
        //настраиваем альтернативные пины usart1
        RCC->APB1ENR|=RCC_APB1ENR_USART2EN; //подаем тактирование
        //AFIO->MAPR&=~AFIO_MAPR_USART1_REMAP; // 0: No remap (TX/PA9, RX/PA10)
        USART2->CR1&=~USART_CR1_M; //0 - 8 Data bits
        USART2->CR1|=USART_CR1_TE; // Transmission enabled
        USART2->CR1|=USART_CR1_RE; // Recieving enabled
        USART2->CR1|=USART_CR1_RXNEIE; //enable interrupt on Rx from usart1
        //USART1->BRR=0x445C; //2400 (APB2CLK - baudrate/2)/baudrate
        //USART2->BRR=0x9C3; //9600 //(48000000-4800)/9600 !!! какогото хрена частота шины в ДВА раза МЕНЬШЕ!!!!
	    USART2->BRR=0xEA5; //9600 //(36000000-4800)/9600        
        //USART1->BRR=0x16C; //115200
        USART2->CR1|=USART_CR1_UE; //USART EN
        NVIC_EnableIRQ(USART1_IRQn);
    }
};

#endif //UART_H_