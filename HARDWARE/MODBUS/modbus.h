#ifndef _modbus_
#define _modbus_

// #include "stm32f10x_conf.h"
#include "stm32f10x_gpio.h"


typedef struct
{
 u8 myadd;//本设备的地址
 u8 rcbuf[100]; //MODBUS接收缓冲区
 u16 timout;//MODbus的数据断续时间	
 u8 recount;//MODbus端口已经收到的数据个数
 u8 timrun;//MODbus定时器是否计时的标志
 u8  reflag;//收到一帧数据的标志
 u8 Sendbuf[100]; //MODbus发送缓冲区	

}MODBUS;


extern MODBUS modbus;
void Mosbus_Init(void);
void Mosbus_Event(void);
	

#endif

