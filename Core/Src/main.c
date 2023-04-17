/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "fir.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_SPI_MspInit(&hspi2);
  uint8_t buf[40];
  HAL_TIM_PWM_Start(&htim3 , TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3 , TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3 , TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3 , TIM_CHANNEL_4);
  


  uint16_t ADC_ch1[4]={0};
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_ch1,4);
  uint16_t ADC_ch2[4]={0};
  HAL_ADC_Start_DMA(&hadc2,(uint32_t *)ADC_ch2,4);
  uint16_t ADC_ch3[4]={0};
  HAL_ADC_Start_DMA(&hadc3,(uint32_t *)ADC_ch3,4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t encoder_cmd , encoder_deg;
  
  
  int i=0;
  float k=3.3/4096;
  while (1)
  {
    i++;
    encoder_cmd=0x7fff;
    uint16_t temp[3]={0};
    float voltage[3]={0};

    temp[2]=avg_fliter16_t(ADC_ch3,4);
    temp[0]=avg_fliter16_t(ADC_ch1,4);
    temp[1]=avg_fliter16_t(ADC_ch2,4);
    

    voltage[0]=temp[0]*k;
    voltage[1]=temp[1]*k;
    voltage[2]=temp[2]*k;
    sprintf((char*)buf,"0:%.3f 1:%.3f 2:%.3f\r\n",voltage[0],
           voltage[1],voltage[2]);
    HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
    
    HAL_SPI_TransmitReceive(&hspi2,(uint8_t*)&encoder_cmd,(uint8_t*)&encoder_deg,1,10);
    HAL_Delay(10);

    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
    
    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);

    //int i = 0;
    /*
    while (i<100)
    {
      i++;
      __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,i*10);
      HAL_Delay(10);
    }
    while(i>0){
      i--;

      __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,i*10);
      HAL_Delay(10);
    }
    */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 128;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
