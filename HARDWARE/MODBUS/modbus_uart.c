#include "modbus_uart.h"
#include "modbus.h"

void my_usart_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* 使能 USART1 时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* USART1 使用IO端口配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);                //初始化GPIOA

  /* USART1 工作模式配置 */
  USART_InitStructure.USART_BaudRate = 115200;                                      //9600
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //数据位数设置：8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //停止位设置：1位
  USART_InitStructure.USART_Parity = USART_Parity_No;                             //是否奇偶校验：无
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制模式设置：没有使能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //接收与发送都使能
                                                                                  // USART_Init(USART1, &USART_InitStructure);  //初始化USART1
                                                                                  // USART_Cmd(USART1, ENABLE);// USART1使能
  USART_DeInit(USART1);
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  USART_ClearFlag(USART1, USART_FLAG_TC);

  MY_NVIC_Init(0, 0, USART1_IRQn, 2);
}

void my_usart_byte(u8 d) //发送一个字节
{
  USART_SendData(USART1, d);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    ;
  USART_ClearFlag(USART1, USART_FLAG_TC);
}

void USART1_IRQHandler() //MODBUS字节接收中断
{
  u8 st, sbuf;
  st = USART_GetITStatus(USART1, USART_IT_RXNE);
  if (st == SET) //
  {
    sbuf = USART1->DR;
    if (modbus.reflag == 1) //有数据包正在处理
    {
			if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
      return;
    }
    modbus.rcbuf[modbus.recount++] = sbuf;
    modbus.timout = 0;
    if (modbus.recount == 1) //收到主机发来的一帧数据的第一字节
    {
      modbus.timrun = 1; //启动定时
    }
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}
