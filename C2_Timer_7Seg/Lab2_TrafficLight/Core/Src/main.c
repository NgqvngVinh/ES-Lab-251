/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
static void MX_GPIO_Init(void);
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int counter = 0 ;
  #define ON 0
  #define OFF 1


  #define RED 0
  #define YELLOW 1
  #define GREEN 2
  int led_status = YELLOW;

    while (1)
    {
      /* USER CODE END WHILE */
  //	  //ex1
  //	  HAL_GPIO_WritePin ( LED3_GPIO_Port , LED3_Pin , 1) ;
  //	  HAL_Delay (2000) ;
  //	  HAL_GPIO_WritePin ( LED3_GPIO_Port , LED3_Pin , 0) ;
  //	  HAL_Delay (4000) ;
  //	  //ex2
  //	   switch (led_status) {
  //	    case ON:
  //	        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
  //	        counter++;
  //	        if (counter >= 2) {           // sáng đủ 2 giây
  //	            counter = 0;
  //	            led_status = OFF;
  //	        }
  //	        break;
  //
  //	    case OFF:
  //	        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
  //	        counter++;
  //	        if (counter >= 4) {           // tắt đủ 4 giây
  //	            counter = 0;
  //	            led_status = ON;
  //	        }
  //	        break;
  //	    }
  //
  //	    HAL_Delay(1000); // duy nhất 1 delay ở cuối vòng lặp


  	      switch (led_status){
  	  	  case RED: //red 2 side green up down
  	  			  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);    // led3 red
  	  		  	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port,OUTPUT_Y0_Pin,0); //Y0 yellow
  	  		  	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port,OUTPUT_Y1_Pin,1);  //y1 green


  	  		  	  counter++;

  	  		  	  if(counter == 5){
  	  		  		  led_status = GREEN;
  	  		  		  counter = 0;
  	  		  	  }
  	  		  	  break;
  	  	  case GREEN:
  	  		  //red 2 side -> green, yellow up down -> red

  				  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);    // led3 red
  				  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port,OUTPUT_Y0_Pin,1); //Y0 yellow
  				  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port,OUTPUT_Y1_Pin,0);  //y1 green



  	  			  counter++;

  	  			  if(counter == 3){
  	  				  led_status = YELLOW;
  	  				  counter =0;
  	  			  }
  	  			  break;
  	  	  case YELLOW:
  	  		  //red 2 side -> yellow, red up down same
  				  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);    // led3 red
  				  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port,OUTPUT_Y0_Pin,0); //Y0 yellow
  				  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port,OUTPUT_Y1_Pin,1);  //y1 green
//  				  HAL_GPIO_WritePin(INPUT_X1_GPIO_Port,INPUT_X1_Pin,1);  //y1 green



  	  			  counter++;


  	  			  if(counter >= 2){
  	  			  led_status = RED;
  	  			  counter = 0;
  	  			  }
  	  			  break;
  	  	  default:
  	  		  	  break;
  	  	  }
  	  		  	  HAL_Delay(1000);


      /* USER CODE BEGIN 3 */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, testp_Pin|LED3_Pin|OUTPUT_Y0_Pin|OUTPUT_Y1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(INPUT_X1_GPIO_Port, INPUT_X1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : testp_Pin LED3_Pin OUTPUT_Y0_Pin OUTPUT_Y1_Pin */
  GPIO_InitStruct.Pin = testp_Pin|LED3_Pin|OUTPUT_Y0_Pin|OUTPUT_Y1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT_X1_Pin */
  GPIO_InitStruct.Pin = INPUT_X1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(INPUT_X1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT_X2_Pin */
  GPIO_InitStruct.Pin = INPUT_X2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INPUT_X2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INPUT_X3_Pin */
  GPIO_InitStruct.Pin = INPUT_X3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INPUT_X3_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
