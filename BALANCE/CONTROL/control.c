#include "control.h"
/**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm, Position_Pwm;
u8 Flag_Target, Position_Target;
extern u16 Reg[];
u8 delay_20=0;
u16 modcount=0;

/**************************************************************************
函数功能：所有的控制代码都在这里面
          TIM1控制的5ms定时中断 
**************************************************************************/
int TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR & 0X0001) //4ms定时中断
	{
		TIM1->SR &= ~(1 << 0); //===清除定时器1中断标志位
		
//		if (delay_flag == 1)
//		{
//			if (++delay_50 == 2)
//				delay_50 = 0, delay_flag = 0; //===给主函数提供50ms的精准延时
//		}
		
		
//		Encoder = Read_Encoder(4);				//===更新编码器位置信息
//		Reg[1] = Encoder - Position_Zero;
//		
//		Angle_Balance = Get_Adc_Average(3, 15); //===更新姿态,消耗200*15us=3ms
//		Reg[2] = Angle_Balance - ZHONGZHI;
		
			//模拟通讯时间滞后
//		if( ++delay_20 > 4 ) { 
//			delay_20=0;
//			Balance_Pwm = balance(Angle_Balance);	//===角度PD控制
//		if (++Position_Target > 4)
//			Position_Pwm = Position(Encoder), Position_Target = 0; //===位置PD控制 25ms进行一次位置控制
//		}
//		if( delay_20 == 0	){
//		//模拟从控制器读取PWMout并输出，有12ms的延迟
//		Moto = Balance_Pwm - Position_Pwm;//Reg[3];  //Balance_Pwm - Position_Pwm;						   //===计算电机最终PWM
//		}	

		Mosbus_Event(); //处理MODbus数据
			
			
//		Voltage = Get_battery_volt();							   //===获取电池电压
//		//从控制器读取PWMout并输出
//		//Moto = (s16)(Reg[3]);  //Balance_Pwm - Position_Pwm;						   //===计算电机最终PWM
//		Xianfu_Pwm();											   //===PWM限幅 反正占空比100%带来的系统不稳定因素
//		if (Turn_Off(Voltage) == 0)								   //===低压和倾角过大保护
//			Set_Pwm(Moto);										   //===赋值给PWM寄存器
			
		Led_Flash(500);											   //===LED闪烁指示系统正常运行
		//Key();													   //===扫描按键变化
		

	}
	return 0;
}

void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//溢出中断
	{
		
 		if (modbus.timrun != 0)
		{
			modbus.timout++;
			if (modbus.timout >= 4) //间隔时间达到了时间
			{
				modbus.timrun = 0; //关闭定时器--停止定时
				modbus.reflag = 1; //收到一帧数据
			}
		}  	
		
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}

/**************************************************************************
函数功能：倾角PD控制
入口参数：角度
返回  值：倾角控制PWM
作    者：平衡小车之家
**************************************************************************/
int balance(float Angle)
{
	float Bias;											//倾角偏差
	static float Last_Bias, D_Bias;						//PID相关变量
	int balance;										//PWM返回值
	Bias = Angle - ZHONGZHI;							//求出平衡的角度中值 和机械相关
	D_Bias = Bias - Last_Bias;							//求出偏差的微分 进行微分控制
	balance = -Balance_KP * Bias - D_Bias * Balance_KD; //===计算倾角控制的电机PWM  PD控制
	Last_Bias = Bias;									//保持上一次的偏差
	return balance;
}

/**************************************************************************
函数功能：位置PD控制 
入口参数：编码器
返回  值：位置控制PWM
作    者：平衡小车之家
**************************************************************************/
int Position(int Encoder)
{
	static float Position_PWM, Last_Position, Position_Bias, Position_Differential;
	static float Position_Least;
	Position_Least = Encoder - Position_Zero; //===
	Position_Bias *= 0.8;      //滤波器5ms--0.8
	Position_Bias += Position_Least * 0.2; //===一阶低通滤波器
	Position_Differential = Position_Bias - Last_Position;
	Last_Position = Position_Bias;
	Position_PWM = Position_Bias * Position_KP + Position_Differential * Position_KD; //===速度控制
	return Position_PWM;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
	if (moto < 0)
		AIN2 = 1, AIN1 = 0;
	else
		AIN2 = 0, AIN1 = 1;
	PWMA = myabs(moto);
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{
	int Amplitude = 6900; //===PWM满幅是7200 限制在6900
	if (Moto < -Amplitude)
		Moto = -Amplitude;
	if (Moto > Amplitude)
		Moto = Amplitude;
}
/**************************************************************************
函数功能：按键修改小车运行状态  控制摆杆的位置
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{
	int Position = 1040/4; //目标位置 电机原始位置是10000  转一圈是1040 和编码器精度有关，默认是电机转一圈，输出1040个跳变沿
	static int tmp, flag, count;
	tmp = click_N_Double(100);

	if (tmp == 1)
		flag = 1; //++
	if (tmp == 2)
		flag = 2; //--

	if (flag == 1) //摆杆顺时针运动
	{
		Position_Zero += 4;
		count += 4;
		if (count == Position)
			flag = 0, count = 0;
	}
	if (flag == 2) //摆杆逆时针运动
	{
		Position_Zero -= 4;
		count += 4;
		if (count == Position)
			flag = 0, count = 0;
	}
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(int voltage)
{
	u8 temp;
	static u8 count;
	if (1 == Flag_Stop) //电池电压过低，关闭电机
	{
		Flag_Stop = 1;
		temp = 1;
		AIN1 = 0;
		AIN2 = 0;
	}
	else
		temp = 0;

	if (Angle_Balance < (ZHONGZHI - 800) || Angle_Balance > (ZHONGZHI + 800) || voltage < 700)
		count++;
	else
		count = 0;
	if (count == 120)
	{
		count = 0;
		Flag_Stop = 1;
	}
	return temp;
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{
	int temp;
	if (a < 0)
		temp = -a;
	else
		temp = a;
	return temp;
}
