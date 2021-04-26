/**
  ******************************************************************************
  * @Project        : GeneralMath_02-FFT
  * @Autor          : Ismael Poblete V.
  * @Company		: -
  * @Date         	: 04-25-2021
  * @Target			: DISCOVERY-DISC1 STM32F407VG
  * @brief          : Example of FFT with CMSIS-DSP
  * @Lib			: CMSIS, CMSIS-DSP, SME-HAL, HAL.
  * @System Clock
  * 	SYSSource:		PLL(HSE)
  * 	SYSCLK: 		84MHz
  * 	RTCSource:		None
  * 	RTCCLK: 		None
  * @Perf
  * 	*UART2
  * 		PA2			<-----> USART_TX
  * 		PA3			<-----> USART_RX
  * 	*GPIO
  * 		PD12      	------> LED_GREEN
  * 	*ADC
  * 		PA5    	 	------> ADC1,IN5
  * @note
  *
  *		Example of FFT with CMSIS-DSP with SME-HAL.
  *
  *		------------------------------------------------------------
  * 	Download Source:
  *			https://github.com/ARM-software/CMSIS_5/releases/download/5.7.0/ARM.CMSIS.5.7.0.pack.
  *			C:\Users\user_name\AppData\Local\Arm\Packs\ARM\CMSIS\5.7.0\CMSIS\DSP\
  * 	For CMSIS Version 5.7.0 41d9e35 libarm_cortexM4lf_math.a have been changed to a three lines of code for Git LFS for large storage
  * 		Ref:
  * 			https://github.com/ARM-software/CMSIS_5/issues/573
  * 			https://github.com/ARM-software/CMSIS_5/issues/903
  *
  * 		Headers files for CMSIS DSP to use:
  * 			-Folder: 	CMSIS\DSP\Include
  * 			-File  :	libarm_cortexM4lf_math.a (Check the content, it must be a large file)
  *
  * 			https://community.st.com/s/article/configuring-dsp-libraries-on-stm32cubeide
  *		------------------------------------------------------------
  *
  * 	 ***Utility flag:
  *
  * 	 	flag_buffdata_ready: If set then the data buffer is full and ready to process.
  *
  *		****Functions:
  *
  * 	 	-SME_GeneralMath_DMA_Start: Initialize the data acquisition.
  * 	    -SME_GeneralMath_DMA_data_acquisition: Process the data acquisition in
  * 	    	ADC CallBack HAL_ADC_ConvCpltCallback.
  *
  * 		-SME_GeneralMath_rms_float32: Calculate RMS float data values from a block of data.
  * 		-SME_GeneralMath_average_float32: Calculate averages float data values from a block of data.
  *
  * 		-SME_GeneralMath_DMA_reset_request:
  * 			After to process data, reset the buffer and reinitialize ADC-DMA acquisition.
  * 			Reset flag of ready data and start DMA ADC data acquisition
  *
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

#include <arm_math.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma2;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void GPIO_Init(void);
static void USART2_UART_Init(void);
static void ADC1_Init(void);
static void DMA_Init(void);

/* Private user code ---------------------------------------------------------*/

/********************************** ADC-DMA ********************************* */

/* Defines for Store Data from ADC-DMA  */
GeneralMath_DMA_DAQ_HandleTypeDef GeneralMath_data;

/* ADC DMA */
#define NUMBER_ADC_CHANNELS 2
#define SIZE_BLOCK 2048
#define ADC_K_PARAMETER (3.0 / 4096.0)

/* Data to store RMS and average */
float output_rms[NUMBER_ADC_CHANNELS];
float output_average[NUMBER_ADC_CHANNELS];

/********************************** FFT ********************************* */

#define TEST_LENGTH_SAMPLES 2048

/* -------------------------------------------------------------------
* External Input and Output buffer Declarations for FFT Bin Example
* ------------------------------------------------------------------- */
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES/2];

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
uint32_t fftSize = 1024;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_cfft_instance_f32 varInstCfftF32;

/* Reference index at which max energy of bin ocuurs */
uint32_t refIndex = 213, testIndex = 0;
double fft_freq = 0;
float32_t maxValue;

/******************************** General ******************************* */

/* User data*/
char msg[100];


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

	/* Initialize all configured peripherals */
	GPIO_Init();
	USART2_UART_Init();
	DMA_Init();
	ADC1_Init();

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
	memset(msg,0,sizeof(msg));

	/* Start */
	SME_GeneralMath_DMA_Start(&GeneralMath_data,&hadc1,NUMBER_ADC_CHANNELS,SIZE_BLOCK,ADC_K_PARAMETER);

	while(1){
		if(GeneralMath_data.flag_buffdata_ready){
/*
			for(int i =0; i< 2048 ;i++){
				testInput_f32_10khz[i]=(float)GeneralMath_data.data_acq_buffer[1][i];
			}
*/
/*
			arm_status status;


			status = ARM_MATH_SUCCESS;

			status=arm_cfft_init_f32(&varInstCfftF32,fftSize);
*/
			/* Process the data through the CFFT/CIFFT module */
			arm_cfft_f32(&varInstCfftF32, GeneralMath_data.data_acq_buffer[1], ifftFlag, doBitReverse);

			/* Process the data through the Complex Magnitude Module for
			calculating the magnitude at each bin */
			arm_cmplx_mag_f32(GeneralMath_data.data_acq_buffer[1], testOutput, fftSize);

			/* Calculates maxValue and returns corresponding BIN value */
			arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);

			fft_freq = testIndex * 48028.169 / (fftSize-1);

			SME_GeneralMath_DMA_reset_request(&GeneralMath_data);

		}
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


  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Config MCO, signal output of SYSCLK in PC9 pin */
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_1);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = NUMBER_ADC_CHANNELS;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_GREEN */
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN_Port, &GPIO_InitStruct);
}


/**
  * Enable DMA controller clock
  */
static void DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/* Called when buffer is completely filled */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	SME_GeneralMath_DMA_data_acquisition(&GeneralMath_data);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */


}
