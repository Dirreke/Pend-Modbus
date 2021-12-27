#include "modbus_uart.h"
#include "modbus.h"
extern u16 modcount;

void my_usart_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* ʹ�� USART1 ʱ��*/
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE);

  /* USART1 ʹ��IO�˿����� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);                //��ʼ��GPIOA

  /* USART1 ����ģʽ���� */
  USART_InitStructure.USART_BaudRate = 115200;                                      //9600
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //����λ�����ã�8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //ֹͣλ���ã�1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;                             //�Ƿ���żУ�飺��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ģʽ���ã�û��ʹ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�����뷢�Ͷ�ʹ��
                                                                                  // USART_Init(USART1, &USART_InitStructure);  //��ʼ��USART1
                                                                                  // USART_Cmd(USART1, ENABLE);// USART1ʹ��
  USART_DeInit(USART3);
  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART3, ENABLE);
  USART_ClearFlag(USART3, USART_FLAG_TC);

  MY_NVIC_Init(0, 0, USART3_IRQn, 2);
}

void my_usart_byte(u8 d) //����һ���ֽ�
{
  USART_SendData(USART3, d);
  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    ;
  USART_ClearFlag(USART3, USART_FLAG_TC);
}

void USART3_IRQHandler() //MODBUS�ֽڽ����ж�
{
  u8 st, sbuf;
  st = USART_GetITStatus(USART3, USART_IT_RXNE);
  if (st == SET) //
  {
    sbuf = USART3->DR;
    if (modbus.reflag == 1) //�����ݰ����ڴ���
    {
			if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)
				USART_ClearITPendingBit(USART3,USART_IT_RXNE);
      return;
    }
		modbus.timout = 0;
    modbus.rcbuf[modbus.recount++] = sbuf;
    if (modbus.recount == 1) //�յ�����������һ֡���ݵĵ�һ�ֽ�
    {//modbus.timout = 0;
      modbus.timrun = 1; //������ʱ
			//modcount=0;
			
    }
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
  }
}
