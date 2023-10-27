#include "DHT11.h"

uint8_t dht_data[5]={0x00,0x00,0x00,0x00,0x00};

void DHT_GPIO_SET_OUT(void)     //����GPIOxΪ���ģʽ��MCU��IO����DHT11�������źţ�
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
   //��GPIO_InitTypeDef�ṹ�����޸�IO�ڲ������ṹ���Ա��
	// ��ʼ��GPIO_InitStruct
	GPIO_InitStruct.Pin = GPIO_PIN_10;              // Ҫ��ʼ����GPIO�ڱ�ţ�GPIOB_PIN_10��
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // �������ģʽ
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;   // GPIO��Ƶ������Ϊ����
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);         // ��ʼ��GPIOB_PIN_10
}

void DHT_GPIO_SET_IN(void)     //����GPIOxΪ����ģʽ��DHT11��MUC��IO����ƽ�źţ��ź����������ʪ����Ϣ��
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin=GPIO_PIN_10;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}
//΢�뼶��ʱ����

void delay_us(uint16_t time)
{
	uint16_t us_cont=10000-time;
	uint16_t t=10000;
	__HAL_TIM_SET_COUNTER(&htim1,10000);
	HAL_TIM_Base_Start(&htim1);//������ʱ��
	while(t>us_cont)
	{
		t=__HAL_TIM_GET_COUNTER(&htim1);//��ȡ��ʱ���ļ���ֵ
	}
	HAL_TIM_Base_Stop(&htim1);
}
void delay_ms(uint32_t time)
{
	for(unsigned int i=0;i<time;i++)
	{
		delay_us(1000);
	}
}
//��ƽ��ȡ
uint8_t DHT_read_byte(void)
{
	uint16_t i;
	uint16_t res=0;
	uint16_t temp;
	uint8_t read_byte=0;//��λ�洢����
	for(i=0;i<8;i++)
	{
		//�ȴ��ߵ�ƽ
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0&&res<100)
		{
			delay_us(1);//һ΢���ȡһ��
			res++;//��ֹһֱѭ������
		}
		res=0;
		delay_us(40);
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==1)
		{
			temp=1;
		}
		else
		{
			temp=0;
		}
		//����һ�ζ�ȡ
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==1&&res<100)
		{
			delay_us(1);//һ΢���ȡһ��
			res++;//��ֹһֱѭ������
		}
		res=0;
		read_byte<<=1;
		read_byte|=temp;
	}
	return read_byte;
}
//��ȡ��ʪ��
uint8_t DHT_read(void)
{
	uint8_t res=0;
	uint8_t i;
	//18ms�ĵ͵�ƽ
	DHT_GPIO_SET_OUT();
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);
	HAL_Delay(18);
	//20us�ߵ�ƽ
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET);
	delay_us(20);
	
	//�ȴ�������Ϣ
	//����Ϊ����ģʽ
	DHT_GPIO_SET_IN();
	delay_us(20);
	//��ʼ���
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==0&&res<100)
		{
			delay_us(1);//һ΢���ȡһ��
			res++;//��ֹһֱѭ������
		}
		res=0;
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==1&&res<100)
		{
			delay_us(1);//һ΢���ȡһ��
			res++;//��ֹһֱѭ������
		}
		res=0;
		//��ʼ����DHT11������
	}
	for(i=0;i<5;i++)
	{
		dht_data[i]=DHT_read_byte();
	}
	delay_us(50);
	//У��
	if((dht_data[0]+dht_data[1]+dht_data[2]+dht_data[3])==dht_data[4])
		return 1;
	else
		return 0;
}


