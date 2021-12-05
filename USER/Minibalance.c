#include "sys.h"
/**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop = 1, delay_50, delay_flag;											  //ֹͣ��־λ 50ms��׼��ʾ��־λ
int Encoder, Position_Zero = 10000;												  //���������������
int Moto;																		  //���PWM���� Ӧ��Motor�� ��Moto�¾�
int Voltage;																	  //��ص�ѹ������صı���
float Angle_Balance;															  //��λ�ƴ���������
float Balance_KP = 400, Balance_KD = 400, Position_KP = 20, Position_KD = 300;	  //PIDϵ��
float Menu = 1, Amplitude1 = 5, Amplitude2 = 20, Amplitude3 = 1, Amplitude4 = 10; //PID������ز���

u16 Reg[20];
//{0x0000,   //���豸�Ĵ����е�ֵ
//           0x0001,
//           0x0002,
//           0x0003,
//           0x0004,
//           0x0005,
//           0x0006,
//           0x0007,
//           0x0008,
//           0x0009,
//           0x000A,	
//          };	


// void delay(u32 x)
// {

//  while(x--);
// }


// void TIM2_IRQHandler()//��ʱ��2���жϷ����Ӻ���  1msһ���ж�
// {
//   u8 st;
//   st= TIM_GetFlagStatus(TIM2, TIM_FLAG_Update);	
// 	if(st==SET)
// 	{
// 	  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
// 		if(modbus.timrun!=0)
// 		{
// 		  modbus.timout++; 
// 		  if(modbus.timout>=8)  //���ʱ��ﵽ��ʱ��
// 			{
// 				modbus.timrun=0;//�رն�ʱ��--ֹͣ��ʱ
// 				modbus.reflag=1;  //�յ�һ֡����
// 			}
// 		}  		
// 	}	
// }
	u8 st2;
int main(void)
{
	Stm32_Clock_Init(9);		   //=====ϵͳʱ������
	delay_init(72);				   //=====��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);	   //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);		   //=====��SWD�ӿ� �������������SWD�ӿڵ���
	delay_ms(1000);				   //=====��ʱ�������ȴ�ϵͳ�ȶ�
	delay_ms(1000);				   //=====��ʱ�������ȴ�ϵͳ�ȶ� ��2s
	LED_Init();					   //=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
	EXTI_Init();				   //=====������ʼ��(�ⲿ�жϵ���ʽ)
	OLED_Init();				   //=====OLED��ʼ��
	MiniBalance_PWM_Init(7199, 0); //=====��ʼ��PWM 10KHZ�������������
	Encoder_Init_TIM4();		   //=====��ʼ����������TIM2�ı������ӿ�ģʽ��
	Angle_Adc_Init();			   //=====��λ�ƴ�����ģ�����ɼ���ʼ��
	Baterry_Adc_Init();			   //=====��ص�ѹģ�����ɼ���ʼ��
	Timer1_Init(9, 7199);		   //=====��ʱ�жϳ�ʼ��
	// uart_init(72, 128000);		   //=====��ʼ������1
	Mosbus_Init();
	while (1)
	{
		st2 = USART_GetFlagStatus(USART1,USART_FLAG_ORE);
		Mosbus_Event(); //����MODbus����
		//DataScope();	//===��λ��
		delay_flag = 1; //===50ms�жϾ�׼��ʱ��־λ
		oled_show();	//===��ʾ����
		while (delay_flag); //===50ms�жϾ�׼��ʱ  ��Ҫ�ǲ�����ʾ��λ����Ҫ�ϸ��50ms��������
	}
}

