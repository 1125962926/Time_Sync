#ifndef __DELAY_H__
#define __DELAY_H__
#include <main.h>




void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
#endif

