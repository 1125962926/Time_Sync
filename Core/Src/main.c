#include "main.h"
#include "tim.h"
#include "oled.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "tim.h"
#include "lora.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	delay_init(64);
	
	TIM1_Init(12800 - 1, 1 - 1);		//0.1ms

	USART1_UART_Init(115200);
	USART2_UART_Init(115200);//配置模式时，需要更改为9600
	
	LED_Init();
	KEY_Init();
	
	CS_OLED_Init();
	OLED_Clear();
	OLED_ShowString(16, 0, (uint8_t *)"LoRa"); 
	OLED_ShowCHinese(0, 4, 0);//本
	OLED_ShowCHinese(16, 4, 1);//地
	OLED_ShowCHinese(32, 4, 2);//时
	OLED_ShowCHinese(48, 4, 3);//间
	
#if MASTER
	OLED_ShowCHinese(75, 0, 4);//主
#else 
	OLED_ShowCHinese(75, 0, 5);//从
#endif

	OLED_ShowChar(64, 4, ':');

	//时间间隔
	OLED_ShowNum(0, 6, 0, 2, 16);//时
	OLED_ShowChar(16, 6, ':');
	OLED_ShowNum(24, 6, 0, 2, 16);//分
	OLED_ShowChar(40, 6, ':');
	OLED_ShowNum(48, 6, 0, 2, 16);//秒

	LoRa_Init();

	while (1)
	{
		if(globel_time % 500 == 0)//50ms
		{
			LED1_ON();
			LED1_OFF();
			//System_TIME_to_oled();//屏幕刷新，耗时2ms，可以抵消tim1一个周期0.1ms内多次进入条件的问题
		}

#if MASTER
		
		//主机广播发送同步请求，循环发送给每一个从机
		if(Master_broad_STA && handle_OK_STA)
		{	
			handle_OK_STA = 0;
			Master_Send_sync_request();//主机发送同步请求
		}

		//从机回应
		if(USART2_RX_STA & 0x8000)
		{
			Master_Send_gap_time();
			USART2_RX_STA = 0;
		}

		//外部串口发送时间给主机，主机通过串口校时。格式：小时:分钟:秒，例： 12:30:00
		if(USART_RX_STA & 0x8000)
		{
			Refresh_System_TIME_uart();//更新系统时间
			USART_RX_STA = 0;
			Master_broad_STA = 1;//更新时间后同步一次
		}

		//按键也可以开启一次同步
		if(KEY_Scan())
			Master_broad_STA = 1;

		
#else//从机等待主机广播
		if(USART2_RX_STA & 0x8000)
		{
			USART2_RX_STA = 0;
			if(LORA_receive_buff[0] == SLAVE_ADDR)//是否与自己通信
				Slave_Receive_period();
		}
#endif

	}	

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	*/
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 16;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
