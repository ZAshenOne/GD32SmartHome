#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "tim.h"
#include <string.h>
#include "DHT11.h"
#include <stdio.h>

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void closeCurtain(void);
void openCurtain(void);
void closeWindow(void);
void openWindow(void);
void ledOn(void);
void ledOff(void);
void pwmLed(uint8_t light);
void openAir(void);
void closeAir(void);
void environmentControl(uint8_t temp,uint8_t light);

#endif
