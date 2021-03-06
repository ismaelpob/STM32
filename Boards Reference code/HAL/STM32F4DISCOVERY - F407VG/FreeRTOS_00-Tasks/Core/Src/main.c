/**
  ******************************************************************************
  * @Project        : FreeRTOS_00-Tasks
  * @Autor          : Ismael Poblete
  * @Company		: -
  * @Date         	: 02-23-2021
  * @Target			: DISCOVERY-DISC1 STM32F407VG
  * @brief          : Basic FreeRTOS task programming.
  * @Lib			: CMSIS, HAL. Third party: FreeRTOS
  * @System Clock
  * 	SYSSource:		PLL(HSE)
  * 	SYSCLK: 		84MHz
  * 	RTCSource:		None
  * 	RTCCLK: 		None
  * @Perf
  * 	*None
  * @RTOS
  * 	*task
  * 		task1: Example task1
  * 		task2: Example task2
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* Private user code ---------------------------------------------------------*/

/* FreeRTOS ------------------------------------------------------------------*/

/* Tasks ----------------------------------*/
TaskHandle_t xTaskHandler1=NULL;
TaskHandle_t xTaskHandler2=NULL;
//prototipes
void vtask1_Handler(void *params);
void vtask2_Handler(void *params);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Create a taskk */
	//Tarea 1
	xTaskCreate(vtask1_Handler,"task1",configMINIMAL_STACK_SIZE,NULL,2,&xTaskHandler1);
	//tarea 2
	xTaskCreate(vtask2_Handler,"task2",configMINIMAL_STACK_SIZE,NULL,2,&xTaskHandler2);

	/* Init scheduler */
	//osKernelInitialize();

	/* Start scheduler */
	//osKernelStart();
	vTaskStartScheduler();

	while (1);
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Function of task1.
  * @retval None
  */
void vtask1_Handler(void *params){
	while(1){

	}
}

/**
  * @brief  Function of task2.
  * @retval None
  */
void vtask2_Handler(void *params){
	while(1){

	}
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	while(1);
}

