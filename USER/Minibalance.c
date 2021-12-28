#include "sys.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop = 1, delay_50, delay_flag;											  //停止标志位 50ms精准演示标志位
s16 Encoder, Position_Zero = 10000;												  //编码器的脉冲计数
int Moto;																		  //电机PWM变量 应是Motor的 向Moto致敬
int Voltage;																	  //电池电压采样相关的变量
s16 Angle_Balance,zhongzhi=1968;															  //角位移传感器数据

//u16 Balance_KP = 300, Balance_KD = 100, Position_KP = 15, Position_KD = 75;	  //20ms等幅摇摆PID系数，位置环是3倍时间，滤波使用0.6
//u16 Balance_KP = 300, Balance_KD = 100, Position_KP = 15, Position_KD = 75;	  //15ms等幅摇摆PID系数，位置环是4倍时间，滤波使用0.6
//u16 Balance_KP = 300, Balance_KD = 150, Position_KP = 15, Position_KD = 110;	  //10ms可用摇摆PID系数，位置环是5倍时间，滤波使用0.7
//u16 Balance_KP = 400, Balance_KD = 400, Position_KP = 20, Position_KD = 300;	  //5ms原来PID系数
u16 Balance_KP = 300, Balance_KD = 300, Position_KP = 15, Position_KD = 225;	  //5ms摇摆PID系数--指不太硬的但是能稳住
float Menu = 1, Amplitude1 = 5, Amplitude2 = 20, Amplitude3 = 1, Amplitude4 = 10; //PID调试相关参数

u16 Reg[20];
//{0x0000,   //本设备寄存器中的值
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
	Stm32_Clock_Init(9);		   //=====系统时钟设置
	delay_init(72);				   //=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);	   //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);		   //=====打开SWD接口 可以利用主板的SWD接口调试
	delay_ms(1000);				   //=====延时启动，等待系统稳定
	delay_ms(1000);				   //=====延时启动，等待系统稳定 共2s
	LED_Init();					   //=====初始化与 LED 连接的硬件接口
	EXTI_Init();				   //=====按键初始化(外部中断的形式)
//	OLED_Init();				   //=====OLED初始化
	MiniBalance_PWM_Init(7199, 0); //=====初始化PWM 10KHZ，用于驱动电机
	Encoder_Init_TIM4();		   //=====初始化编码器（TIM2的编码器接口模式）
	Angle_Adc_Init();			   //=====角位移传感器模拟量采集初始化
	Baterry_Adc_Init();			   //=====电池电压模拟量采集初始化
	Timer1_Init(9, 7199);		   //=====定时中断初始化
	Timer2_Init(1, 7199);		   //=====定时中断初始化
  uart_init(72, 128000);		   //=====初始化串口1
	Mosbus_Init();
	
	
	while (1)
	{	

		Encoder = Read_Encoder(4);				//===更新编码器位置信息
		Reg[1] = Encoder - Position_Zero;
		
		Angle_Balance = Get_Adc_Average(3, 15); //===更新姿态,消耗200*15us=3ms
		Reg[2] = Angle_Balance - zhongzhi;
		Reg[4] = zhongzhi;
		
		Voltage = Get_battery_volt();							   //===获取电池电压
		
		//从控制器读取PWMout并输出
		Moto = (s16)(Reg[3]);  //Balance_Pwm - Position_Pwm;						   //===计算电机最终PWM
		Xianfu_Pwm();											   //===PWM限幅 反正占空比100%带来的系统不稳定因素
		if (Turn_Off(Voltage) == 0)								   //===低压和倾角过大保护
			Set_Pwm(Moto);										   //===赋值给PWM寄存器
		
		if(delay_flag==0){
			delay_flag=1;
		DataScope();	//===上位机
			}
		
	}
}

