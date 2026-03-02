/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
#include <stdio.h>
#include <stdarg.h>
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
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;
extern osMutexId_t xUartDebugMutex;


/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#ifdef UART_DEBUG
int __io_putchar(int ch)
{
	uint8_t myChar = (uint8_t)ch;
	HAL_UART_Transmit(&huart1, (uint8_t*)&myChar, 1, 1000);
	return ch;
}

int _write(int file, char *ptr, int len)
{
	(void)file;
	int DataIdx;
	if (xUartDebugMutex != NULL && osMutexAcquire(xUartDebugMutex, osWaitForever) == osOK)
	{
		for (DataIdx = 0; DataIdx < len; DataIdx++)
		{
			__io_putchar(*ptr++);
		}
		osMutexRelease(xUartDebugMutex);
	}
	return len;
}
#endif

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected. */
  (void)xTask;

  printf("FATAL: Stack overflow in task %s\r\n", pcTaskName);
  /* Force an assert. */
  //configASSERT(0);
}
/* USER CODE END Application */

