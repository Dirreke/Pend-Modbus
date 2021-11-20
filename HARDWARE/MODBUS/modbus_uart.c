#include "modbus_uart.h"
#include "modbus.h"

void my_usart_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* ʹ�� USART1 ʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* USART1 ʹ��IO�˿����� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                //��ʼ��GPIOA

  /* USART1 ����ģʽ���� */
  USART_InitStructure.USART_BaudRate = 115200;                                      //9600
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //����λ�����ã�8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //ֹͣλ���ã�1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;                             //�Ƿ���żУ�飺��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ģʽ���ã�û��ʹ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�����뷢�Ͷ�ʹ��
                                                                                  // USART_Init(USART1, &USART_InitStructure);  //��ʼ��USART1
                                                                                  // USART_Cmd(USART1, ENABLE);// USART1ʹ��
  USART_DeInit(USART1);
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  USART_ClearFlag(USART1, USART_FLAG_TC);

  MY_NVIC_Init(0, 0, USART1_IRQn, 2);
}

void my_usart_byte(u8 d) //����һ���ֽ�
{
  USART_SendData(USART1, d);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    ;
  USART_ClearFlag(USART1, USART_FLAG_TC);
}

void USART1_IRQHandler() //MODBUS�ֽڽ����ж�
{
  u8 st, sbuf;
  st = USART_GetITStatus(USART1, USART_IT_RXNE);
  if (st == SET) //
  {
    sbuf = USART1->DR;
    if (modbus.reflag == 1) //�����ݰ����ڴ���
    {
			if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
      return;
    }
    modbus.rcbuf[modbus.recount++] = sbuf;
    modbus.timout = 0;
    if (modbus.recount == 1) //�յ�����������һ֡���ݵĵ�һ�ֽ�
    {
      modbus.timrun = 1; //������ʱ
    }
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}
