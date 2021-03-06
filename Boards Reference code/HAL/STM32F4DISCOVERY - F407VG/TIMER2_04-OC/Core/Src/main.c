/**
  ******************************************************************************
  * @Project        : TIMER3_04-OC
  * @Autor          : Ismael Poblete
  * @Company		: -
  * @Date         	: 02-15-2021
  * @Target			: DISCOVERY-DISC1 STM32F407VG
  * @brief          : Config TIMER3 CH1-4 like a output capture (OC) in PA6,PA7,PB0,PB1
  * 				  Config TIMER2 CH2 like a input capture (IC)
  * 				  PA1 (Source) should be connected one at a time to PA6,PA7,PB0,PB1 (Destination)
  * @Lib			: CMSIS, HAL.
  * @System Clock
  * 	SYSSource:		PLL(HSE)
  * 	SYSCLK: 		84MHz
  * 	RTCSource:		None
  * 	RTCCLK: 		None
  * @Perf
  * 	*UART2
  * 		PA2			<-----> USART_TX
  * 		PA3			<-----> USART_RX
  * 	*TIM2
  * 		PA1			<-----> TIM2_PWM
  * 	*TIM3
  * 		PA6			<-----> TIM3_OC_CH1
  * 		PA7			<-----> TIM3_OC_CH2
  * 		PB0			<-----> TIM3_OC_CH3
  * 		PB1			<-----> TIM3_OC_CH4
  * @note
  *
  *		TIMER2 IC
  *			HCLK: 84Mhz
  *			f_input_timer: 84Mhz
  *			TIMER2_PRESCALER: 0
  *			TIMER2_PERIOD: 65535
  *			fout_prescaler = f_input_timer / (TIMER2_PRESCALER + 1)
  *			fout_prescaler = 84Mhz / ( 0 + 1) = 84MHz
  *			min_frec_to_measure = fout_prescaler / (TIMER2_PERIOD + 1).
  *			min_frec_to_measure = 84Mhz / (65535 + 1) = 1281.7Hz
  *
  *		TIMER3 OC CH1-4.
  *			HCLK: 84Mhz
  *			f_input_timer: 84Mhz
  *			TIMER2_PRESCALER: 0
  *			TIMER2_PERIOD: 65535
  *			frec_obj_ch1 = 5khz
  *			frec_obj_ch2 = 10Khz
  *			frec_obj_ch3 = 20Khz
  *			frec_obj_ch4 = 40Khz
  *
  *			pulse_frec_obj_ch1 = f_input_timer / frec_obj_ch1
  *			pulse_frec_obj_ch1 = 84MHz / 5000Hz = 16800
  *			signa_per_obj_ch1 = pulse_frec_obj_ch1 / 2
  *			signa_per_obj_ch1 = 8400
  *
  *			pulse_obj_ch1 = 8400
  *			pulse_obj_ch2 = 4200
  *			pulse_obj_ch3 = 2100
  *			pulse_obj_ch4 = 1050
  *
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
UART_HandleTypeDef huart2;
/*TIMER PWM*/
TIM_HandleTypeDef htim2;
/*TIMER IC*/
TIM_HandleTypeDef htim3;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void USART2_UART_Init(void);
static void GPIO_Init(void);
static void TIM2_Init(void);
static void TIM3_Init(void);
/* Private user code ---------------------------------------------------------*/
volatile uint32_t pulse1_value = PULSE1_VALUE;
volatile uint32_t pulse2_value = PULSE2_VALUE;
volatile uint32_t pulse3_value = PULSE3_VALUE;
volatile uint32_t pulse4_value = PULSE4_VALUE;
volatile uint32_t ccr_content;

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

	/* LETS REDO THE SYSTICK CONFIGURATION */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* Initialize all configured peripherals */
	GPIO_Init();
	USART2_UART_Init();
	TIM2_Init();
	TIM3_Init();

	/* User Code */
	char msg[100];

	memset(msg,0,sizeof(msg));
	sprintf(msg,"SYSCLK : %ldHz\r\n",HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"HCLK   : %ldHz\r\n",HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"PCLK1  : %ldHz\r\n",HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"PCLK2  : %ldHz\r\n",HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);

	/* Init Timers */
	/* Timer 3*/
	if( HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	if( HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}

	if( HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}

	if( HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}

	/* Timer 2*/
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

	/* Infinite loop */
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
  RCC_OscInitStruct.PLL.PLLQ = 4;

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Config MCO, signal output of SYSCLK in PC9 pin */
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_1);
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{


}


/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void TIM2_Init(void)
{

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_IC_InitTypeDef sConfigIC = {0};

	  htim2.Instance = TIM2;
	  htim2.Init.Prescaler = TIMER2_PRESCALER;
	  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim2.Init.Period = TIMER2_PERIOD;
	  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	  sConfigIC.ICFilter = 0;
	  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	  {
	    Error_Handler();
	  }

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = TIMER3_PRESCALER;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = TIMER3_PERIOD;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = PULSE1_VALUE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = PULSE2_VALUE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = PULSE3_VALUE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = PULSE4_VALUE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim3);

}


uint32_t IC_Value1 = 0;
uint32_t IC_Value2 = 0;
uint32_t Difference = 0;
uint32_t inputFrequency = 0;
uint8_t Is_First_Captured = 0;  // 0- not captured, 1- captured

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		if (Is_First_Captured==0)
		{
			IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			Is_First_Captured =1;
		}

		else if (Is_First_Captured)
		{
			IC_Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

			if (IC_Value2 > IC_Value1)
			{
				Difference = IC_Value2-IC_Value1;
			}

			else if (IC_Value2 < IC_Value1)
			{
				Difference = ((0xffff-IC_Value1)+IC_Value2) +1;
			}

			else
			{
				Error_Handler();
			}

			inputFrequency = HAL_RCC_GetPCLK1Freq()/(Difference * ((uint32_t)TIMER2_PRESCALER + 1));
 			Is_First_Captured = 0;
		}
	}
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* TIM3_CH1 toggling with frequency = 500 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	   ccr_content = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
	   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,ccr_content+pulse1_value);
  }

  /* TIM3_CH2 toggling with frequency = 1000 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
	   ccr_content = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
	   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_2,ccr_content+pulse2_value);

  }

  /* TIM3_CH3 toggling with frequency = 2000 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
	   ccr_content = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_3);
	   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_3,ccr_content+pulse3_value);

  }

  /* TIM3_CH4 toggling with frequency = 4000 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
	    ccr_content = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_4);
	   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_4,ccr_content+pulse4_value);

  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */


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
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */


