#include "sys.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop = 1, delay_50, delay_flag;											  //停止标志位 50ms精准演示标志位
int Encoder, Position_Zero = 10000;												  //编码器的脉冲计数
int Moto;																		  //电机PWM变量 应是Motor的 向Moto致敬
int Voltage;																	  //电池电压采样相关的变量
float Angle_Balance;															  //角位移传感器数据
float Balance_KP = 400, Balance_KD = 400, Position_KP = 20, Position_KD = 300;	  //PID系数
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


// void delay(u32 x)
// {

//  while(x--);
// }


// void TIM2_IRQHandler()//定时器2的中断服务子函数  1ms一次中断
// {
//   u8 st;
//   st= TIM_GetFlagStatus(TIM2, TIM_FLAG_Update);	
// 	if(st==SET)
// 	{
// 	  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
// 		if(modbus.timrun!=0)
// 		{
// 		  modbus.timout++; 
// 		  if(modbus.timout>=8)  //间隔时间达到了时间
// 			{
// 				modbus.timrun=0;//关闭定时器--停止定时
// 				modbus.reflag=1;  //收到一帧数据
// 			}
// 		}  		
// 	}	
// }
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
	OLED_Init();				   //=====OLED初始化
	MiniBalance_PWM_Init(7199, 0); //=====初始化PWM 10KHZ，用于驱动电机
	Encoder_Init_TIM4();		   //=====初始化编码器（TIM2的编码器接口模式）
	Angle_Adc_Init();			   //=====角位移传感器模拟量采集初始化
	Baterry_Adc_Init();			   //=====电池电压模拟量采集初始化
	Timer1_Init(9, 7199);		   //=====定时中断初始化
	// uart_init(72, 128000);		   //=====初始化串口1
	Mosbus_Init();
	while (1)
	{
		st2 = USART_GetFlagStatus(USART1,USART_FLAG_ORE);
		Mosbus_Event(); //处理MODbus数据
		//DataScope();	//===上位机
		delay_flag = 1; //===50ms中断精准延时标志位
		oled_show();	//===显示屏打开
		while (delay_flag); //===50ms中断精准延时  主要是波形显示上位机需要严格的50ms传输周期
	}
}

