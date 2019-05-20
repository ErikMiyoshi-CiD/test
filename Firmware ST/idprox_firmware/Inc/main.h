/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_INPUT_Pin GPIO_PIN_0
#define LED_INPUT_GPIO_Port GPIOA
#define B0_Pin GPIO_PIN_1
#define B0_GPIO_Port GPIOA
#define B1_Pin GPIO_PIN_2
#define B1_GPIO_Port GPIOA
#define B2_Pin GPIO_PIN_3
#define B2_GPIO_Port GPIOA
#define _125KHZ_Pin GPIO_PIN_6
#define _125KHZ_GPIO_Port GPIOA
#define ASK_IN_Pin GPIO_PIN_7
#define ASK_IN_GPIO_Port GPIOA
#define D0_DATA_Pin GPIO_PIN_0
#define D0_DATA_GPIO_Port GPIOB
#define CARD_PRES_Pin GPIO_PIN_1
#define CARD_PRES_GPIO_Port GPIOB
#define D1_TX_CLK_Pin GPIO_PIN_9
#define D1_TX_CLK_GPIO_Port GPIOA
#define MS_BUZZ_Pin GPIO_PIN_10
#define MS_BUZZ_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_3
#define LED_RED_GPIO_Port GPIOB
#define LED_GRN_Pin GPIO_PIN_4
#define LED_GRN_GPIO_Port GPIOB
#define BUZZ_Pin GPIO_PIN_5
#define BUZZ_GPIO_Port GPIOB
#define MIFARE_SCL_Pin GPIO_PIN_6
#define MIFARE_SCL_GPIO_Port GPIOB
#define MIFARE_SDA_Pin GPIO_PIN_7
#define MIFARE_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
