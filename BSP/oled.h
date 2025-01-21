/*
 * @Author: Li RF
 * @Date: 2024-02-27 15:39:16
 * @LastEditTime: 2024-02-29 13:17:50
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#ifndef __OLED_H
#define __OLED_H
	
#include "main.h"
 	

#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	  
  						  
//========移植的时候，根据引脚分配进行修改==========

//使用4线串行接口时使用 
#define OLED_RST_GPIO					GPIOA
#define OLED_RST_GPIO_PIN			    GPIO_PIN_11

#define OLED_DC_GPIO					GPIOA
#define OLED_DC_GPIO_PIN			    GPIO_PIN_12

#define OLED_SCLK_GPIO				    GPIOA
#define OLED_SCLK_GPIO_PIN		        GPIO_PIN_5

#define OLED_SDIN_GPIO				    GPIOA
#define OLED_SDIN_GPIO_PIN		        GPIO_PIN_7




//RES
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_GPIO_PIN, GPIO_PIN_SET)

//DC
#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_SET)

//SCLK,D0
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_SET)

//SDIN,D1
#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_SET)

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void CS_OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t);
void OLED_Fill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char dot);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned long num,unsigned char len,unsigned char size);
void OLED_ShowString(unsigned char x,unsigned char y, unsigned char *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif  
	 



