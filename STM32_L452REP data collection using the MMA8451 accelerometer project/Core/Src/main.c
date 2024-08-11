
/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Global handle variables ---------------------------------------------------------*/

// CAN peripheral handle variable

CAN_HandleTypeDef hcan1;

// I2C peripheral handle variable

I2C_HandleTypeDef hi2c2;

// Timer 6 (timer that handles the CAN transmission messages)

TIM_HandleTypeDef htim6;

// Timer 16 (timer that handles the count down that resets the chip if the startup message isn't sent

TIM_HandleTypeDef htim16;

// UART2 ( UART to USB port ) peripheral handle variable

UART_HandleTypeDef huart2;


/* Global multi-purpose variables ---------------------------------------------------------*/

// array used to store the message sent through UART2 throughout the program

uint8_t msg[100];

// variables use to store the upper margins for tilt ( margin_o ) and slip ( margin_s ) values detected by the accelerometer

uint16_t margin_o = 0;
uint16_t margin_s = 0;

// variables used to store previous measurements for acceleration on the X-axis and Y-axis for the accelerometer

int8_t X_past = 0;
int8_t Y_past = 0;

// variables used within the timers to count the time period till the slip ( nr_slip_period ), reset due to lack of transmission on the CAN
// peripheral ( nr_reset_period ) and reset due to lack of startup message through CAN ( nr_startup_wait ) functions are called

uint8_t nr_slip_period  = 0;
uint16_t nr_reset_period = 0;
uint8_t nr_startup_wait = 1;

// variables used to store the slip flag, having the following meaning:
// 1 - the vehicle started slipping
// 0 - the vehicle is not slipping

uint8_t slip = 0;


/* Peripherals initialization function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM16_Init(void);


/* Entry point for the main() function ---------------------------------------------------------*/

int main(void)
{
  // HAL initialization

  HAL_Init();

  // Configuring the system clock

  SystemClock_Config();

  // Peripherals initialization

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_I2C2_Init();
  MX_TIM6_Init();
  MX_TIM16_Init();

  // CAN filter configuration

  Filter_Configuration(&hcan1, 0x5E6, 0x400, 0x000);

  // calling System_start_sequence() to make sure everything is fine till this step

  System_start_sequence(&huart2);

  // starting the CAN peripheral

  if(HAL_CAN_Start(&hcan1) != HAL_OK)
  {
   Error_Handler();
  }

  // waiting in blocking mode for startup sequence

  CAN_Wait_For_Startup(&hcan1,&huart2,&htim16);

  // enabling CAN interrupts ( for transmission, reception and possible errors )

  CAN_Interrupt_Enabler(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF);

  // starting the timer that handles the transmission of data through CAN

  if(HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
  {
   Error_Handler();
  }

  // the infinite loop within main() function

  while (1)
  {
  }

}

/* Function implementations ---------------------------------------------------------*/

// system clock configuration function

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

// CAN1 peripheral initialization function

static void MX_CAN1_Init(void)
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 10;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }


}

// I2C peripheral initialization function

static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0xD010BDBD;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

// TIM6 peripheral initialization function

static void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Timer 16 initialization function

static void MX_TIM16_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig2 = {0};
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = HAL_RCC_GetPCLK2Freq()/500 - 1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 56000;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig2.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig2.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim16, &sMasterConfig2) != HAL_OK)
  {
    Error_Handler();
  }

}

// UART2 initialization function

static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

// GPIO initialization function

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SMPS_EN_Pin|SMPS_V1_Pin|SMPS_SW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SMPS_EN_Pin SMPS_V1_Pin SMPS_SW_Pin */
  GPIO_InitStruct.Pin = SMPS_EN_Pin|SMPS_V1_Pin|SMPS_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SMPS_PG_Pin */
  GPIO_InitStruct.Pin = SMPS_PG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SMPS_PG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 LD4_Pin */
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

// CAN transmission completion callback

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	memset(msg,0,sizeof(msg)); // clear the msg array contents
	sprintf(msg,"Message sent succesfully!\r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
}

// CAN error callback

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	memset(msg,0,sizeof(msg)); // clear the msg array contents
	sprintf(msg,":( Something went wrong...\r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
}

// CAN receive message on FIFO0 callback

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	  CAN_RxHeaderTypeDef RxHeader;

      uint8_t margin[8];

	  if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, margin) != HAL_OK)
	  {
		  Error_Handler();
	  }
      if(RxHeader.StdId == 0x619)
      {

	  margin_o = margin[0];
	  margin_s = margin[1];
	  memset(msg,0,sizeof(msg));
	  sprintf(msg,"Margin_overturn: %d\r\n",margin_o);
	  HAL_UART_Transmit(&huart2, msg, strlen(msg), HAL_MAX_DELAY);
	  memset(msg,0,sizeof(msg));
	  sprintf(msg,"Margin_slip: %d\r\n",margin_s);
	  HAL_UART_Transmit(&huart2, msg, strlen(msg), HAL_MAX_DELAY);
      }
}

// timer period elapsed callback

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM6 )
  {
    uint32_t TxMailbox;
    ACTIVE(hi2c2);
    if(nr_slip_period < 10)
    {
     slip = Slip_detection(hi2c2, _8bit, _4g, &X_past, &Y_past, margin_s);
     nr_slip_period++;
    }
    else
    	{
    	nr_slip_period++;
    	slip = 0;
    	}
    uint8_t tilt_slip = (Overturn_detection(hi2c2, _8bit, _4g, margin_o) << 4) | slip;
    int8_t info[4] = { X_Read_8BitRes(hi2c2), Y_Read_8BitRes(hi2c2), Z_Read_8BitRes(hi2c2), tilt_slip};
	STANDBY(hi2c2);
    CAN_TxHeaderTypeDef TxHeader;
	TxHeader.DLC = 4;
	TxHeader.StdId = 0x666;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	if(HAL_CAN_AddTxMessage(&hcan1,&TxHeader,info,&TxMailbox) != HAL_OK)
	{
		if( nr_reset_period == 100)
		{
			nr_reset_period = 0;
			System_reset_sequence(&huart2);
		}
		else
		{
			nr_reset_period++;
		}
	}
	else
		nr_reset_period = 0;
  }
  else
  {
	  if( nr_startup_wait != 4)
	  {
	  memset(msg,0,sizeof(msg));
	  sprintf(msg,"Startup attempt %d / 3\r\n", nr_startup_wait);
	  nr_startup_wait ++ ;
	  HAL_UART_Transmit(&huart2, msg, strlen(msg), HAL_MAX_DELAY);
	  }
	  else
	  {
		 memset(msg,0,sizeof(msg));
		 sprintf(msg,"Startup not received. Initializing local restart sequence! \r\n\n");
		 HAL_UART_Transmit(&huart2, msg, strlen(msg), HAL_MAX_DELAY);
		 System_reset_sequence(&huart2);
	  }
  }
}

// error handler function

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
