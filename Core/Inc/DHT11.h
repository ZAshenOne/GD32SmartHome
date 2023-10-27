#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32f1xx_hal.h"
#include "tim.h"

void delay_us(uint16_t time);
void delay_ms(uint32_t time);

uint8_t DHT_read_byte(void);
uint8_t DHT_read(void);
#endif
