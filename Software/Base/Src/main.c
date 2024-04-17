/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "data.h"
#include "main.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MSG_SIZE_B INPUT_SIZE*4 // message size in bytes
#define MSG_SIZE_INT INPUT_SIZE // num of ints in message

/* Private includes ----------------------------------------------------------*/
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//~ static void MX_TIM2_Init(void);
static void MX_USART3_UART_Init(void);

void send_serial(uint8_t *data, int size)
{

  HAL_UART_Transmit(&huart3, data, size, HAL_MAX_DELAY);
}

void receive_serial(uint8_t *data, int size)
{

  HAL_UART_Receive(&huart3, data, size, HAL_MAX_DELAY);
}

/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
#define KIN1_DWT_CONTROL (*((volatile uint32_t *)0xE0001000))
/*!< DWT Control register */
#define KIN1_DWT_CYCCNTENA_BIT (1UL << 0)
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNT (*((volatile uint32_t *)0xE0001004))
/*!< DWT Cycle Counter register */
#define KIN1_DEMCR (*((volatile uint32_t *)0xE000EDFC))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_LAR (*((volatile uint32_t *)0xE0001FB0))
/*!< lock access register */
#define KIN1_TRCENA_BIT (1UL << 24)
/*!< Trace enable bit in DEMCR register */

#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT
/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */

#define KIN1_EnableLockAccess() \
  KIN1_LAR = 0xC5ACCE55;
/*!< lock access */

#define KIN1_ResetCycleCounter() \
  KIN1_DWT_CYCCNT = 0
/*!< Reset cycle counter */

#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
/*!< Enable cycle counter */

#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
/*!< Disable cycle counter */

#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT
/*!< Read cycle counter register */

// Sync controller and wrapper
void sync()
{
  float zero = 0.0;
  float one = 1.0;
  uint8_t size = sizeof(float);

  // Sync
  float val;
  receive_serial(&val, size);
  if (val == (float)0)
  {
    send_serial(&zero, size);
  }
  else
  {
    send_serial(&one, size);
  }
}

uint32_t cycles_e, cycles_d;  /* number of cycles */
int freq;

void send_app_runtime(float c)
{
  float time = (float)c / freq; 
  uint8_t size = sizeof(float);

  // Sync with script
  sync();
  // Send app runtime (seconds)
  send_serial(&time, size);
}

void send_runtime(float c)
{
  uint8_t size = sizeof(float);
  float time = (float)c / freq; 

  // Sync with script
  sync();
  send_serial(&time, size);
}

void send_output(double output)
{
  uint8_t size = sizeof(double);

  // Sync with script
  sync();
  send_serial(&output, size);
}

void send_uint32(uint32_t output)
{
  uint8_t size = sizeof(uint32_t);

  // Sync with script
  sync();
  send_serial(&output, size);
}

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

  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  KIN1_InitCycleCounter(); /* enable DWT hardware */
  KIN1_EnableLockAccess();
  freq = HAL_RCC_GetSysClockFreq();

 /*  
#if CRYPTO_KEYBYTES==16
    volatile unsigned char key[CRYPTO_KEYBYTES] = {0xDEADBEEF, 0x01234567, 0x89ABCDEF, 0xDEADBEEF};
#else 
    volatile unsigned char  key[CRYPTO_KEYBYTES] = {0xDEADBEEF, 0x01234567, 0x89ABCDEF, 0xDEADBEEF, 0xDEADBEEF, 0x01234567, 0x89ABCDEF, 0xDEADBEEF};
#endif
  volatile unsigned char nonce[CRYPTO_NPUBBYTES] = {0};
  // volatile unsigned char key[CRYPTO_KEYBYTES] = {0};
  volatile uint64_t msglen = MSG_SIZE_B;// sizeof(text) / sizeof(unsigned char);
  volatile unsigned long long ctlen = 0;
  volatile unsigned char ct[MSG_SIZE_B + CRYPTO_ABYTES] = {0};
  volatile unsigned long long adlen = 0;
	*/
  // decrypt check
  uint8_t dt[MSG_SIZE_B] = {0};
/*
  // Declare pointers
  volatile unsigned char *c;
  volatile unsigned long long *clen;
  volatile uint64_t *mlen;
  volatile unsigned char *k, *npub;
  volatile unsigned char *m;

  // Initialise pointers
  k = key;
  npub = nonce;
  clen = &ctlen;
  m = text;
  c = ct;
  mlen = &msglen;
*/
  double output;
  uint8_t sum = 0;
  HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);
  uint32_t err_c = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
 {
	#ifdef POWER_CONS
		
		for(int i=0;i<N_LOOP;i++)
			output = ENCRYPT(c, clen, m, msglen, NULL, adlen, NULL, npub, k);
      HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(1000);


	#else
    float discard;
		// Sync before app execution
		sync();

		KIN1_ResetCycleCounter();  /* reset cycle counter */
		KIN1_EnableCycleCounter(); /* start counting */
    
		// Start application
    // Encryption
		double encrypt = 0;//ENCRYPT(c, clen, m, msglen, NULL, adlen, NULL, npub, k);
		cycles_e = KIN1_GetCycleCounter(); /* get cycle counter */
    
    // Decryption
    KIN1_ResetCycleCounter();  /* reset cycle counter */
		KIN1_EnableCycleCounter(); /* start counting */
    // DECRYPT(dm, mlen, NULL, c, ctlen, NULL, adlen, npub, k);
    double decrypt =0;// DECRYPT(dt, mlen, NULL, c, *clen, NULL, adlen, npub, k);

    cycles_d = KIN1_GetCycleCounter(); /* get cycle counter */

    send_serial(&discard, 4);

    // Checksum
    uint32_t dt_int;
    for (int i=0;i<MSG_SIZE_INT;i++){
      dt_int = dt[i*4] | (dt[i*4 + 1] << 8) | (dt[i*4 +2] << 16) | (dt[i*4 +3] << 24);
      if (dt_int != text[i])
        err_c += 1;
    }

    send_serial(&discard, 4);
		send_app_runtime(cycles_e);
    send_runtime(cycles_d);
		// Send output
    send_output(encrypt);
		send_output(decrypt);
    send_uint32(err_c);
	send_uint32(dt);
	send_uint32(text);
		//~ HAL_Delay(1000);
	
	#endif
  }
   KIN1_DisableCycleCounter();
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
   */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin | LD3_Pin | LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin | LD3_Pin | LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

int __io_putchar(int ch)
{
  ITM_SendChar(ch);
  return ch;
}

int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}

void DebugLog(const char *s)
{
  printf("%s", s);
}

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

#ifdef USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
