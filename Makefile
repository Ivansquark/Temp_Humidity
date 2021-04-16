.PHONY: all clean load

CC = arm-none-eabi-gcc
OBJC = arm-none-eabi-objcopy
OBJD = arm-none-eabi-objdump
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

TARGET = src/main.cpp
INC = inc/
SRC = src/
LIB = lib/
BLD = build/
FRS = freeRTOS/src/
FRH = freeRTOS/inc/

CPPFLAGS = -c -g -O2 -Wall -nostartfiles -lstdc++ \
	-mcpu=cortex-m3 -mthumb -march=armv7-m -Wno-pointer-arith -mfloat-abi=soft \
	-u_printf_float -ffast-math -fno-math-errno -ffunction-sections -fdata-sections \
	-fno-rtti -fno-exceptions -specs=nosys.specs -specs=nano.specs -fno-common -D"assert_param(x)=" \
	-L/usr/lib/arm-none-eabi/newlib/ 
CFLAGS = -Wall -g -O2 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wno-pointer-arith 
LCPPFLAGS = -mcpu=cortex-m3 -mthumb -nostartfiles -lm -lc -lgcc \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -u_printf_float -mfloat-abi=soft -fno-use-cxa-atexit \
		 -L/usr/lib/arm-none-eabi/newlib/thumb/v7-m/ -L/usr/lib/arm-none-eabi/newlib/ \
		 	-Xlinker -Map=$(BLD)main.map -z muldefs 
LDFLAGS =  -marmelf --gc-sections -lgcc -lm -lc \
	-L/usr/lib/gcc/arm-none-eabi/7.3.1/ -L/usr/lib/gcc/arm-none-eabi/7.3.1/thumb/v7-m/
	
load: $(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32f1x.cfg -c "program $(BLD)main.bin \
	verify exit reset 0x08000000"
	//st-flash write main.bin 0x08000000

all: $(BLD)main.bin $(BLD)main.elf $(BLD)main.lst
$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary
$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst
$(BLD)main.elf: $(BLD)main.o $(BLD)startup.o $(BLD)temp.o $(BLD)lcdpar.o $(BLD)malloc.o  
$(BLD)main.elf: $(BLD)freertos/queue.o $(BLD)freertos/list.o $(BLD)freertos/timers.o
$(BLD)main.elf: $(BLD)freertos/heap_2.o $(BLD)freertos/tasks.o $(BLD)freertos/port.o
$(BLD)main.elf: $(BLD)dht22.o $(BLD)timer.o $(BLD)frwrapper.o $(BLD)irq.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32f103.ld $(BLD)startup.o $(BLD)main.o $(BLD)temp.o $(BLD)malloc.o \
	$(BLD)freertos/tasks.o $(BLD)freertos/heap_2.o $(BLD)freertos/timers.o $(BLD)freertos/list.o \
	$(BLD)freertos/port.o $(BLD)freertos/queue.o $(BLD)lcdpar.o $(BLD)dht22.o $(BLD)timer.o \
	$(BLD)frwrapper.o $(BLD)irq.o \
	-I$(LIB) -I$(FRH) $(LCPPFLAGS)
	arm-none-eabi-size $(BLD)main.elf
$(BLD)startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o $(BLD)startup.o $(CPPFLAGS)
$(BLD)malloc.o: src/malloc.cpp
	$(CC) src/malloc.cpp -o $(BLD)malloc.o -I$(INC) -I$(FRH) $(CPPFLAGS)
	
$(BLD)freertos/port.o: freeRTOS/src/port.c 
	$(CC) freeRTOS/src/port.c -o $(BLD)freertos/port.o -I$(FRH) -I$(INC) $(CFLAGS)
$(BLD)freertos/tasks.o: freeRTOS/src/tasks.c
	$(CC) freeRTOS/src/tasks.c -o $(BLD)freertos/tasks.o -I$(FRH) -I$(INC) $(CFLAGS)
$(BLD)freertos/queue.o: freeRTOS/src/queue.c
	$(CC) freeRTOS/src/queue.c -o $(BLD)freertos/queue.o -I$(FRH) -I$(INC) $(CFLAGS)	
$(BLD)freertos/list.o: freeRTOS/src/list.c
	$(CC) freeRTOS/src/list.c -o $(BLD)freertos/list.o -I$(FRH) -I$(INC) $(CFLAGS)	
$(BLD)freertos/timers.o: freeRTOS/src/timers.c
	$(CC) freeRTOS/src/timers.c -o $(BLD)freertos/timers.o -I$(FRH) -I$(INC) $(CFLAGS)	
$(BLD)freertos/heap_2.o: freeRTOS/src/heap_2.c
	$(CC) freeRTOS/src/heap_2.c -o $(BLD)freertos/heap_2.o -I$(FRH) -I$(INC) $(CFLAGS)	
	
$(BLD)main.o: $(TARGET)
	$(CC) $(TARGET) -o $(BLD)main.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)temp.o: $(SRC)temp.cpp
	$(CC) $(SRC)temp.cpp -o $(BLD)temp.o -I$(INC) $(CPPFLAGS)	
$(BLD)lcdpar.o: $(SRC)lcdpar.cpp
	$(CC) $(SRC)lcdpar.cpp -o $(BLD)lcdpar.o -I$(INC) -I$(LIB) $(CPPFLAGS)
$(BLD)timer.o: $(SRC)timer.cpp
	$(CC) $(SRC)timer.cpp -o $(BLD)timer.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)dht22.o: $(SRC)dht22.cpp
	$(CC) $(SRC)dht22.cpp -o $(BLD)dht22.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)frwrapper.o: $(SRC)frwrapper.cpp
	$(CC) $(SRC)frwrapper.cpp -o $(BLD)frwrapper.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)	
$(BLD)irq.o: $(SRC)irq.cpp
	$(CC) $(SRC)irq.cpp -o $(BLD)irq.o -I$(INC) -I$(LIB) $(CPPFLAGS)		
	
clean:
	rm -rf $(BLD)*.o $(BLD)freertos/*.o $(BLD)*.elf $(BLD)*.lst $(BLD)*.bin $(BLD)*.map 