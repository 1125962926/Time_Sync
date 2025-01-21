#ifndef __USART_H__
#define __USART_H__

#include "main.h"
#include <stdio.h>


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define USART2_REC_LEN  		200 

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern uint8_t  USART_RX_BUF[USART_REC_LEN];
extern uint8_t  USART2_RX_BUF[USART2_REC_LEN];

extern uint16_t USART_RX_STA;
extern uint16_t USART2_RX_STA;

void USART1_UART_Init(uint32_t bound);
void USART2_UART_Init(uint32_t bound);

void usart_send_string(const char *str, uint16_t len);
void usart2_send_string(const char *str, uint16_t len);
#endif
