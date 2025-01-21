#include "usart.h"
#include "delay.h"
#include "tim.h"
#include "led.h"

#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
/**
 * @brief	定义_sys_exit()以避免使用半主机模式
 *
 * @param	void
 *
 * @return  void
 */
void _sys_exit(int x)
{
    x = x;
}
/**
 * @brief	重定义fputc函数
 *
 * @param	ch		输出字符量
 * @param	f		文件指针
 *
 * @return  void
 */
int fputc(int ch, FILE *f)
{
#define UART1_print 1    //选择uart1打印或uart2
	
#if UART1_print
    while((USART1->ISR & 0X40) == 0); //循环发送,直到发送完毕
    USART1->TDR = (uint8_t) ch;
#else
	while((USART2->ISR & 0X40) == 0); //循环发送,直到发送完毕
    USART2->TDR = (uint8_t) ch;
#endif
	
    return ch;
}
#endif


uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint8_t USART2_RX_BUF[USART2_REC_LEN];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA = 0;     //接收状态标记
uint16_t USART2_RX_STA = 0;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;


void USART1_UART_Init(uint32_t bound)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = bound;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

void USART2_UART_Init(uint32_t bound)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = bound;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(uartHandle->Instance==USART1)
	{
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
		HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(uartHandle->Instance==USART2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);//接收中断
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);//空闲中断
		HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
}




void USART1_IRQHandler(void)
{
	uint8_t aRxBuffer;

    if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        HAL_UART_Receive(&huart1, &aRxBuffer, 1, 1000);

        if((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if(USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if(aRxBuffer != 0x0a)USART_RX_STA = 0; //接收错误,重新开始

                else 
                {
                    USART_RX_STA |= 0x8000;	//接收完成了
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = '\0';
                }
            }
            else //还没收到0X0D
            {
                if(aRxBuffer == 0x0d)USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer;
                    USART_RX_STA++;

                    if(USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
	HAL_UART_IRQHandler(&huart1);
}

/**
 * @description: uart2连接lora模块，结尾没有回车换行，所以需要判断接收空闲标志位
 * @return {*}
 */
void USART2_IRQHandler(void)
{
	uint8_t aRxBuffer;

    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        HAL_UART_Receive(&huart2, &aRxBuffer, 1, 1000);

        //未越界，且未接收完成
        if(((USART2_RX_STA & 0X3FFF) < USART2_REC_LEN) && ((USART_RX_STA & 0x8000) == 0))
        {
            USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = aRxBuffer;
            USART2_RX_STA++;
        }
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
    }

    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)//一帧数据接收完成
    {
        USART2_RX_STA |= 0x8000;	//接收完成了
        USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = '\0';
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    }

	HAL_UART_IRQHandler(&huart2);
}



/**
 * @description: uart1发送数据
 * @param {char} *str：要发送的字符串
 * @param {uint16_t} len：发送长度
 * @return {*}
 */
void usart_send_string(const char *str, uint16_t len)
{
    /* 发送字符串 */
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, 0xffff);  /* 发送数据 */
}

/**
 * @description: uart2发送数据
 * @param {char} *str：要发送的字符串
 * @param {uint16_t} len：发送长度
 * @return {*}
 */
void usart2_send_string(const char *str, uint16_t len)
{
    /* 发送字符串 */
    HAL_UART_Transmit(&huart2, (uint8_t *)str, len, 0xffff);  /* 发送数据 */
}



