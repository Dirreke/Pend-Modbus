#ifndef _modbusuart_
#define _modbusuart_


// #include "stm32f10x_conf.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "sys.h"
void my_usart_Init(void);
void my_usart_byte(u8 d); 
#endif

