#include "sys.h"
/**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop = 1, delay_50, delay_flag;											  //ֹͣ��־λ 50ms��׼��ʾ��־λ
s16 Encoder, Position_Zero = 10000;												  //���������������
int Moto;																		  //���PWM���� Ӧ��Motor�� ��Moto�¾�
int Voltage;																	  //��ص�ѹ������صı���
s16 Angle_Balance,zhongzhi=1968;															  //��λ�ƴ���������

//u16 Balance_KP = 300, Balance_KD = 100, Position_KP = 15, Position_KD = 75;	  //20ms�ȷ�ҡ��PIDϵ����λ�û���3��ʱ�䣬�˲�ʹ��0.6
//u16 Balance_KP = 300, Balance_KD = 100, Position_KP = 15, Position_KD = 75;	  //15ms�ȷ�ҡ��PIDϵ����λ�û���4��ʱ�䣬�˲�ʹ��0.6
//u16 Balance_KP = 300, Balance_KD = 150, Position_KP = 15, Position_KD = 110;	  //10ms����ҡ��PIDϵ����λ�û���5��ʱ�䣬�˲�ʹ��0.7
//u16 Balance_KP = 400, Balance_KD = 400, Position_KP = 20, Position_KD = 300;	  //5msԭ��PIDϵ��
u16 Balance_KP = 300, Balance_KD = 300, Position_KP = 15, Position_KD = 225;	  //5msҡ��PIDϵ��--ָ��̫Ӳ�ĵ�������ס
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
//	OLED_Init();				   //=====OLED��ʼ��
	MiniBalance_PWM_Init(7199, 0); //=====��ʼ��PWM 10KHZ�������������
	Encoder_Init_TIM4();		   //=====��ʼ����������TIM2�ı������ӿ�ģʽ��
	Angle_Adc_Init();			   //=====��λ�ƴ�����ģ�����ɼ���ʼ��
	Baterry_Adc_Init();			   //=====��ص�ѹģ�����ɼ���ʼ��
	Timer1_Init(9, 7199);		   //=====��ʱ�жϳ�ʼ��
	Timer2_Init(1, 7199);		   //=====��ʱ�жϳ�ʼ��
  uart_init(72, 128000);		   //=====��ʼ������1
	Mosbus_Init();
	
	
	while (1)
	{	

		Encoder = Read_Encoder(4);				//===���±�����λ����Ϣ
		Reg[1] = Encoder - Position_Zero;
		
		Angle_Balance = Get_Adc_Average(3, 15); //===������̬,����200*15us=3ms
		Reg[2] = Angle_Balance - zhongzhi;
		Reg[4] = zhongzhi;
		
		Voltage = Get_battery_volt();							   //===��ȡ��ص�ѹ
		
		//�ӿ�������ȡPWMout�����
		Moto = (s16)(Reg[3]);  //Balance_Pwm - Position_Pwm;						   //===����������PWM
		Xianfu_Pwm();											   //===PWM�޷� ����ռ�ձ�100%������ϵͳ���ȶ�����
		if (Turn_Off(Voltage) == 0)								   //===��ѹ����ǹ��󱣻�
			Set_Pwm(Moto);										   //===��ֵ��PWM�Ĵ���
		
		if(delay_flag==0){
			delay_flag=1;
		DataScope();	//===��λ��
			}
		
	}
}

