#ifndef LCDPAR_H
#define LCDPAR_H

#include "math.h"
#include "stdio.h"
#include "string.h"

class LcdParInterface
{
public:
    LcdParInterface()
    {}
    void fillScreen(uint16_t color)
    {
        setRow(0, 240);
        setColumn(0,320);        
        send_command(0x2C);        
        for(volatile uint32_t i=0; i<76800; i++)
        {  
            //send_word(color);      
            GPIOA->BSRR|=0xffff0000|color>>8;//sendByte(data);        
            GPIOB->BSRR|=GPIO_BSRR_BR13;GPIOB->BSRR|=GPIO_BSRR_BS13;//wr_strobe();     
            GPIOA->BSRR|=0xffff0000|color;//sendByte(data);        
            GPIOB->BSRR|=GPIO_BSRR_BR13;GPIOB->BSRR|=GPIO_BSRR_BS13;//wr_strobe();     
        }
    }     
    //----------- reset B15 -----------------------
    const void reset_on(){GPIOB->BSRR|=GPIO_BSRR_BR15;}    // low
    const void reset_off(){GPIOB->BSRR|=GPIO_BSRR_BS15;}   // high
    //----------  DC B14 --------------------------------
    const void dc_data(){GPIOB->BSRR|=GPIO_BSRR_BS14;}     // High
    const void dc_command(){GPIOB->BSRR|=GPIO_BSRR_BR14;}  // Low
    //----------  WR B13 -----------------------------------
    inline const void wr_on(){GPIOB->BSRR|=GPIO_BSRR_BR13;}    //low
    inline const void wr_idle(){GPIOB->BSRR|=GPIO_BSRR_BS13;}  //high
    //----------  RD B12 -----------------------------------
    const void rd_on(){GPIOB->BSRR|=GPIO_BSRR_BR12;}    //low
    const void rd_idle(){GPIOB->BSRR|=GPIO_BSRR_BS12;}  //high
    //----------  CS B11 -----------------------------------
    const void cs_on(){GPIOB->BSRR|=GPIO_BSRR_BR11;}   //low
    const void cs_idle(){GPIOB->BSRR|=GPIO_BSRR_BS11;} //high
    inline const void wr_strobe(){wr_on();wr_idle();}   
    
    void reset()
    {
        cs_idle(); wr_idle(); rd_idle(); reset_on();
        delay(2*7200000/1000);
        reset_off(); cs_on();
        send_command(0x01); // software reset
        delay(20); wr_strobe(); cs_idle();
        delay(7200000);
    }
    void write(uint8_t byte)
    {
        send_data(byte); wr_strobe();
    }
    void send_command(uint8_t com)
    {
        dc_command();
        //rd_idle();
        cs_on();
        sendByte(com);
        wr_strobe();
        cs_idle();
        dc_data();
        GPIOB->BSRR|=GPIO_BSRR_BR11; //cs_on();
    }
    void send_data(uint8_t data)
    {
        GPIOA->BSRR|=0xffff0000|data;//sendByte(data);        
        GPIOB->BSRR|=GPIO_BSRR_BR13;GPIOB->BSRR|=GPIO_BSRR_BS13;//wr_strobe();        
        //GPIOB->BSRR|=GPIO_BSRR_BS11; //cs_idle();
    }
    void sendByte(uint8_t byte)
    {
        uint32_t mask=0xffff0000;
        GPIOA->BSRR|=mask|byte;
    }
    void send_word(uint16_t data)
    {        
        send_data(data>>8);
        send_data(data&0x00FF);
    }
    void setColumn(uint16_t StartCol, uint16_t EndCol)
    {
        send_command(0x2A);                                                     // Column Command address
        send_word(StartCol);
        send_word(EndCol);
    }
    void setRow(uint16_t StartRow, uint16_t EndRow)
    {
        send_command(0x2B);                                                  // Column Command address
        send_word(StartRow);
        send_word(EndRow);
    }
    void setXY(int poX, int poY)
    {
        setColumn(poX, poX);
        setRow(poY, poY);
        send_command(0x2C);
    }
    void setPixel(int poX, int poY, int color)
    {
        setXY(poX, poY);
        send_word(color);
        send_word(color);
    }
       
    void swap(uint16_t x1, uint16_t x2)
    {
        if(x2>x1){uint16_t z=x2;x2=x1;x1=z;}
    }
    void checkXYswap(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
    {
        swap(x1,x2); swap(y1,y2);
    }

    void delay(uint32_t x)
    {   while(x>0){x--;}    }
};

class LcdParIni:LcdParInterface
{
public:
    LcdParIni(){lcd_ini();fillScreen(0x0000);}
private:
    void lcd_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        GPIOB->CRH&=~GPIO_CRH_CNF15;
	    GPIOB->CRH|=GPIO_CRH_MODE15;//50MHz
        GPIOB->CRH&=~GPIO_CRH_CNF14;
	    GPIOB->CRH|=GPIO_CRH_MODE14;//50MHz
        GPIOB->CRH&=~GPIO_CRH_CNF13;
	    GPIOB->CRH|=GPIO_CRH_MODE13;//50MHz        
	    GPIOB->CRH&=~GPIO_CRH_CNF12;
	    GPIOB->CRH|=GPIO_CRH_MODE12;//50MHz
        GPIOB->CRH&=~GPIO_CRH_CNF11;
	    GPIOB->CRH|=GPIO_CRH_MODE11;//50MHz
        //--------- DB-0 DB-7 -----------
        GPIOA->CRL&=~GPIO_CRL_CNF0; //DB-0
	    GPIOA->CRL|=GPIO_CRL_MODE0;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF1; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE1;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF2; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE2;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF3; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE3;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF4; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE4;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF5; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE5;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF6; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE6;//50MHz
        GPIOA->CRL&=~GPIO_CRL_CNF7; //DB-1
	    GPIOA->CRL|=GPIO_CRL_MODE7;//50MHz
        tft_ini();
    }
    void tft_ini()
    {
        rd_idle();
        reset();
        //---------------------------------------------------
        send_command(0xEF);
        send_data(0x03);
        send_data(0x80);
        send_data(0x02);
        //Power Control A
        send_command(0xCB);  
        send_data(0x39);
        send_data(0x2C);
        send_data(0x00);
        send_data(0x34);
        send_data(0x02); 
        //Power Control B
        send_command(0xCF);
        send_data(0x00);
        send_data(0xC1);
        send_data(0x30);
        //Power on Sequence control
        send_command(0xED);
        send_data(0x64);
        send_data(0x03);
        send_data(0x12);
        send_data(0x81);        
        //Driver timing control A
        send_command(0xE8);
        send_data(0x85);
        send_data(0x00);
        send_data(0x78);        
        //Driver timing control B
        send_command(0xEA);
        send_data(0x00);
        send_data(0x00);                
        //Pump ratio control
        send_command(0xF7);
        send_data(0x20);
        //Power Control,VRH[5:0]
        send_command(0xC0);//Power contro
        send_data(0x23);  //VRH[5:0
        //Power Control,SAP[2:0];BT[3:0]
        send_command(0xC1);
        send_data(0x10);   //SAP[2:0];BT[3:0
        //VCOM Control 1
        send_command(0xC5);//VCM control
        send_data(0x3E);  //Contrast
        send_data(0x28);
        //VCOM Control 2
        send_command(0xC7);//VCM control2
        send_data(0x86);
        send_command(0x37);
        send_data(0x00);
        //Memory Acsess Control - rotation
        send_command(0x36);
        send_data(0xf8);  // 1-полубайт ориентация (через 2) - 2-ой цветовая схема (0 или 8)
        //Pixel Format Set
        send_command(0x3A);
        send_data(0x55);//16bit
        //Frame Rratio Control, Standard RGB Color
        send_command(0xB1);
        send_data(0x00);
        send_data(0x18);
        send_command(0xB6);      // Display Function Control
        send_data(0x08); 
        send_data(0x82); 
        send_data(0x27);
        //Enable 3G (пока не знаю что это за режим)
        send_command(0xF2);
        send_data(0x00);//не включаем        
        //Gamma set
        send_command(0x26);
        send_data(0x01);//Gamma Curve (G2.2) (Кривая цветовой гаммы)
        //Positive Gamma  Correction
        send_command(0xE0);
        send_data(0x0F);
        send_data(0x31);
        send_data(0x2B);
        send_data(0x0C);
        send_data(0x0E);
        send_data(0x08);
        send_data(0x4E);
        send_data(0xF1);
        send_data(0x37);
        send_data(0x07);
        send_data(0x10);
        send_data(0x03);
        send_data(0x0E);
        send_data(0x09);
        send_data(0x00);
        //Negative Gamma  Correction
        send_command(0xE1);
        send_data(0x00);
        send_data(0x0E);
        send_data(0x14);
        send_data(0x03);
        send_data(0x11);
        send_data(0x07);
        send_data(0x31);
        send_data(0xC1);
        send_data(0x48);
        send_data(0x08);
        send_data(0x0F);
        send_data(0x0C);
        send_data(0x31);
        send_data(0x36);
        send_data(0x0F);

        send_command(0x2B); //page set
        send_data(0x00);
        send_data(0x00);
        send_data(0x00);
        send_data(0xEF);
        
        send_command(0x2A); // column set
        send_data(0x00);
        send_data(0x00);
        send_data(0x01);
        send_data(0x3F);
        
        send_command(0x34); //tearing effect off
        send_command(0xB7); //entry mode set
        send_data(0x07);
        
        send_command(0x11);//Выйдем из спящего режима
        delay(720000); //100 ms
        //Display ON
        
        send_command(0x29);//display on
        //send_data(TFT9341_ROTATION);        
        
        delay(720000); //100 ms       
        send_command(0x13);
    }
};

//*********** Класс обработки цифр и букв ********************
class Font_interface:public LcdParInterface
{
public:
    Font_interface(){}
    uint32_t char_to_int(char* str,uint8_t size)
   {
       uint32_t x;
       for(uint8_t i=0;i<size;i++)
       {
           uint8_t dec;
           if (str[i]==48){dec=0;} if (str[i]==49){dec=1;}
           if (str[i]==50){dec=2;} if (str[i]==51){dec=3;}
           if (str[i]==52){dec=4;} if (str[i]==53){dec=5;}
           if (str[i]==54){dec=6;} if (str[i]==55){dec=7;}
           if (str[i]==56){dec=8;} if (str[i]==57){dec=9;}
           x+=dec*pow(10,size-i);           
       }
       return x;
   }
    char arr[20];
    char arrFloat[20]{0};
    volatile uint8_t arrSize=0;

    void intToChar(uint32_t x) //int to char*
    {
        uint32_t y=x;        
        uint8_t count=0;
        while (y>0)
        {  y=y/10;  count++; }//считаем количество цифр
        y=x;
        
        arrSize=count;
        if(x==0) {arrSize=1;arr[arrSize-1]='0';arr[arrSize]='\0';return;} 
        for(uint8_t i=0;i<arrSize;i++)
        {            
            int x=y%10;
            if(x==0) {arr[arrSize-1-i]='0';} if(x==1) {arr[arrSize-1-i]='1';}
            if(x==2) {arr[arrSize-1-i]='2';} if(x==3) {arr[arrSize-1-i]='3';}
            if(x==4) {arr[arrSize-1-i]='4';} if(x==5) {arr[arrSize-1-i]='5';}
            if(x==6) {arr[arrSize-1-i]='6';} if(x==7) {arr[arrSize-1-i]='7';}
            if(x==8) {arr[arrSize-1-i]='8';} if(x==9) {arr[arrSize-1-i]='9';}
            y=y/10;        
        }
        if(arrSize+1<10)
        {
            //strcat(arr+arrSize,'\0'); 
            arr[arrSize]='\0';
        }
    }
    void floatTochar(float x)
    {        
        sprintf(arrFloat, "%.3f", x);           
    }
};
class Font_16x16: public Font_interface
{
public:
    Font_16x16(){}
    
    void symbol(uint16_t x, uint16_t y, uint16_t col,const uint16_t* data)
    {        
        setColumn(x,x+16);
        setRow(y,y+16);
        send_command(0x2C);
        for(uint16_t i=0; i <y+16-y;i++)   //заполняем 8-битную матрицу
        {
            //send_word(col);
            for(uint16_t j=0; j <x+16-x+1;j++)
            {
                if((j==0)&&(data[i]&0x8000)) {send_word(col);}
                else if((j==1)&&(data[i]&0x4000)) {send_word(col);}
                else if((j==2)&&(data[i]&0x2000)) {send_word(col);}
                else if((j==3)&&(data[i]&0x1000)) {send_word(col);}
                else if((j==4)&&(data[i]&0x0800)) {send_word(col);}
                else if((j==5)&&(data[i]&0x0400)) {send_word(col);}
                else if((j==6)&&(data[i]&0x0200)) {send_word(col);}
                else if((j==7)&&(data[i]&0x0100)) {send_word(col);}
                else if((j==8)&&(data[i]&0x0080)) {send_word(col);}
                else if((j==9)&&(data[i]&0x0040)) {send_word(col);}
                else if((j==10)&&(data[i]&0x0020)) {send_word(col);}
                else if((j==11)&&(data[i]&0x0010)) {send_word(col);}
                else if((j==12)&&(data[i]&0x0008)) {send_word(col);}
                else if((j==13)&&(data[i]&0x0004)) {send_word(col);}
                else if((j==14)&&(data[i]&0x0002)) {send_word(col);}
                else if((j==15)&&(data[i]&0x0001)) {send_word(col);}
                else send_word(0x0000);
            }
        }       
    }    
    void clearString(uint16_t x, uint16_t y,uint8_t size)
    {
        setColumn(x,x+16*size);
        setRow(y,y+16);
        send_command(0x2C);
        for(uint16_t i=0; i <(y+16+1)*(x+16+1);i++)   //заполняем 8-битную матрицу
        {
            send_word(0x0000);
        }
    }
    void print(uint16_t x, uint16_t y,uint16_t col,const char* str,uint8_t numDigits)
    {        
        for(uint8_t i=0;i<strlen(str);i++)
        {
            if (str[i]==48){symbol(x, y, col, data0);}
            if (str[i]==49){symbol(x, y, col, data1);}
            if (str[i]==50){symbol(x, y, col, data2);}
            if (str[i]==51){symbol(x, y, col, data3);}
            if (str[i]==52){symbol(x, y, col, data4);}
            if (str[i]==53){symbol(x, y, col, data5);}
            if (str[i]==54){symbol(x, y, col, data6);}
            if (str[i]==55){symbol(x, y, col, data7);}
            if (str[i]==56){symbol(x, y, col, data8);}
            if (str[i]==57){symbol(x, y, col, data9);}
            if (str[i]==58){symbol(x, y, col, dataColon);}
            if (str[i]==46){symbol(x, y, col, dataPoint);x+=17;}
            else{x+=17;}                        
        }
        for(uint8_t i=strlen(str);i<strlen(str)+numDigits-1;i++)
        {
            {symbol(x, y, col, dataSpace);}
            x+=17;            
        }
    }    
    const uint16_t data0[16]={0x0FE0,0x1FF0,0x3FF8,0x7C7C,0x783C,0xF01E,0xF01E,0xF01E,          
                              0xF01E,0xF01E,0x783C,0x7C7C,0x3FF8,0x1FF0,0x0FE0,0x0000}; //0         
    const uint16_t data1[16]={0x07F8,0x0FF8,0x1FF8,0x3FF8,0x7FF8,0xFFF8,0xFDF8,0xF9F8,          
                              0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x0000}; //1         
    const uint16_t data2[16]={0x3FF0,0x7FF8,0xFFFC,0xF0FC,0xF0FC,0xE1F8,0x03F0,0x07E0,         
                              0x0FC0,0x1F80,0x3F00,0x7E00,0xFFFC,0xFFFC,0xFFFC,0x0000}; //2         
    const uint16_t data3[16]={0x3FF0,0x7FF8,0xFFFC,0xE07C,0x003C,0x007C,0x3FF8,0x7FF0,          
                              0x3FF8,0x007C,0x003C,0xE07C,0xFFFC,0x7FF8,0x3FF0,0x0000}; //3         
    const uint16_t data4[16]={0x03FC,0x07FC,0x0FFC,0x1E7C,0x3C7C,0x787C,0xF07C,0xFFFC,          
                              0xFFFC,0xFFFC,0x007C,0x007C,0x007C,0x007C,0x007C,0x0000}; //4         
    const uint16_t data5[16]={0xFFFC,0xFFFC,0xFFFC,0xE000,0xE000,0xFFE0,0xFFF0,0xFFF8,          
                              0x007C,0x003C,0xE03C,0xF07C,0xFFF8,0x7FF0,0x3FE0,0x0000}; //5         
    const uint16_t data6[16]={0x3FF8,0x7FFC,0xFFFC,0xF03C,0xE000,0xF000,0xFFF0,0xFFF8,          
                              0xFFFC,0xF03C,0xE01C,0xF03C,0xFFF8,0x7FF0,0x3FE0,0x0000}; //6         
    const uint16_t data7[16]={0x7FFC,0xFFFC,0xFFFC,0xFFFC,0x003C,0x007C,0x00FC,0x01F8,          
                              0x03F0,0x07E0,0x0FC0,0x1F80,0x3F00,0x7E00,0xFC00,0x0000}; //7         
    const uint16_t data8[16]={0x1FE0,0x3FF0,0x7FF8,0xF87C,0xF03C,0xF87C,0x7FF8,0x3FF0,          
                              0x7FF8,0xF87C,0xF03C,0xF87C,0x7FF8,0x3FF0,0x1FE0,0x0000}; //8         
    const uint16_t data9[16]={0x1FE0,0x3FF0,0x7FF8,0xF03C,0xF03C,0xF03C,0xFFFC,0x7FFC,          
                              0x3FFC,0x003C,0xE07C,0xF0F8,0xFFF0,0x7FE0,0x3FC0,0x0000}; //9
    const uint16_t dataColon[16]={0x0000,0x0000,0x0780,0x0780,0x0780,0x0780,0x0000,0x0000,          
                              0x0000,0x0780,0x0780,0x0780,0x0780,0x0000,0x0000,0x0000};  //:  
    const uint16_t dataSpace[16]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,          
                              0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};  //Space  
    const uint16_t dataPoint[16]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,          
                              0x0000,0x0000,0x0000,0x03C0,0x03C0,0x03C0,0x03C0,0x0000};  //.  
};

#endif //LCDPAR_H