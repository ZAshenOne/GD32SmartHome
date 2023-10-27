#include "command.h"
/*该文件为对各种外设的控制函数 开关窗、窗帘、灯、空调等*/
char rxbuf[6];
char timebuf[16];

/* 开关窗/窗帘 由舵机(PB8)实现;
由TIM4 CH3 (7199,199)输出PWM 进行控制:比较值为185时舵机速度为0
*/
void closeCurtain()	//关窗帘
{
	htim4.Instance->CCR3=190;	//比较寄存器值190,舵机正向 中速旋转
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	delay_ms(4000);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);	
}

void openCurtain()	//开窗帘
{
	htim4.Instance->CCR3=180;	//比较寄存器值180,舵机逆向 中速旋转
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	delay_ms(4000);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
}
void closeWindow()	//关窗
{
	htim4.Instance->CCR3=186;	//舵机正向 低速
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	delay_ms(1500);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
}

void openWindow()	//开窗
{
	htim4.Instance->CCR3=184;	//舵机逆向 低速
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	delay_ms(1500);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
}

/*灯光控制由PB11：TIM2_Ch4 PWM控制，100Hz闪烁，比较寄存器值0-99与最大计数值99来控制占空比*/
void pwmLed(uint8_t light)	//灯 *改为PWM调光,light:0-99->比较寄存器0-99光照越暗,灯越亮
{
	htim2.Instance->CCR4=light;
}
void ledOn()
{
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
}
void ledOff()
{
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
}

/*空调控制由红外收发来控制(PB9:红外接受; PA0:红外发送)*/
void openAir()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	delay_ms(1500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}
void closeAir()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	delay_ms(3000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

//接受到ESP8266消息的回调,根据收到的命令控制家居
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{     
	HAL_UART_Transmit(&huart1,(uint8_t*)rxbuf,sizeof(rxbuf),1000);

	if(strstr(rxbuf,"led on")) //开灯 字符串查找命令
	{
		ledOn();
	}
	else if(strstr(rxbuf,"led of"))  //关灯
	{
		ledOff();
	}
	else if(strstr(rxbuf,"crt on"))  // 开窗帘
	{
		openCurtain();
	}
	else if(strstr(rxbuf,"crt of"))  //关窗帘
	{
		closeCurtain();
	}
	else if(strstr(rxbuf,"win on"))  // 开窗
	{
		openWindow();
	}
	else if(strstr(rxbuf,"win of"))  //关窗
	{ 
		closeWindow();
	}
	else if(strstr(rxbuf,"air on"))  // 开空调
	{
		openAir();
	}
	else if(strstr(rxbuf,"air of"))  //关空调
	{ 
		closeAir();
	}

	//再次准备接受命令
	HAL_UART_Receive_IT(&huart1,(uint8_t*)rxbuf, sizeof(rxbuf));//调试
	HAL_UART_Receive_IT(&huart2,(uint8_t*)rxbuf, sizeof(rxbuf));
}

//根据室内环境控制家居
void environmentControl(uint8_t temp,uint8_t light)
{
	//光照部分,控制灯光和窗帘
	pwmLed(light);
	if(light>75)		//光照大于75,关闭窗帘和灯
	{	
		ledOff();
		closeCurtain();	
	}
	else if(light<20)	//光照小于22,拉开窗帘;灯光开启并进入pwm动态调光
	{	
		ledOn();
		openCurtain();
	}

	//温度,控制空调
}
