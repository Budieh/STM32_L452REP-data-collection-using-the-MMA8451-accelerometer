/*
 * QualityOfLife.c
 *
 *  Created on: Aug 28, 2023
 *      Author: User
 */

// Includes

#include "QualityOfLife.h"

// Function implementations


// CAN setup functions

// Filter configuration

void Filter_Configuration(CAN_HandleTypeDef * hcan, MASK mask, ID id, FLTR_NR fltr)
{

// variable that stores the value of the filter

CAN_FilterTypeDef FilterConfigInit;

// field that stores the id value

FilterConfigInit.FilterIdHigh = (uint32_t) (id << 5);

FilterConfigInit.FilterIdLow  = 0;

// field that stores the mask value

FilterConfigInit.FilterMaskIdHigh = (uint32_t) (mask << 5);

FilterConfigInit.FilterMaskIdLow = 0;
FilterConfigInit.FilterFIFOAssignment =  CAN_FILTER_FIFO0;

//field that stores the filter number ( 0x00 to 0x0E )

FilterConfigInit.FilterBank = fltr;

FilterConfigInit.FilterMode = CAN_FILTERMODE_IDMASK;
FilterConfigInit.FilterScale = CAN_FILTERSCALE_32BIT;
FilterConfigInit.FilterActivation = CAN_FILTER_ENABLE;
FilterConfigInit.SlaveStartFilterBank = 0;

// enabling the filter

if( HAL_CAN_ConfigFilter(hcan, &FilterConfigInit) != HAL_OK)
{
	Error_Handler();
}
}

// Interrupt enable function

void CAN_Interrupt_Enabler(CAN_HandleTypeDef * hcan, uint32_t flags)
{
  if( HAL_CAN_ActivateNotification(hcan, flags) != HAL_OK)
  {
	 Error_Handler();
  }
}

// Startup message function

void CAN_Wait_For_Startup(CAN_HandleTypeDef * hcan, UART_HandleTypeDef * huart, TIM_HandleTypeDef * htim)
{
	char msg[100];
    memset(msg,0,sizeof(msg));
	sprintf(msg,"Waiting for master startup sequence... \r\n\n");
	HAL_UART_Transmit(huart, msg, strlen(msg), HAL_MAX_DELAY);

	// starting the timer that monitors the time waited for the master to send the startup message

	// a message is sent every 20 seconds through UART to signify that the board is approaching a local restart sequence
    // unless the master sends the startup message

    // after 60 seconds pass, the local reset function is called and the board resets

    if(HAL_TIM_Base_Start_IT(htim) != HAL_OK)
	{
	  	  Error_Handler();
	}

	while(HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0 ) == 0)
	{
	}
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t dummy[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, dummy);
	if( RxHeader.StdId != 0x400 )
	{
	   memset(msg,0,sizeof(msg));
	   sprintf(msg,"Startup transmitted on the wrong ID...\r\nRestarting sequence enabled...\r\n\n");
	   HAL_UART_Transmit(huart, msg, strlen(msg), HAL_MAX_DELAY);
	   System_reset_sequence(huart);
	}
	else
	{
		memset(msg,0,sizeof(msg));
		sprintf(msg,"Startup successful...\r\nLet the fun begin!\r\n\n");
	    HAL_UART_Transmit(huart, msg, strlen(msg), HAL_MAX_DELAY);

	    // if the program execution reaches this point of the CAN_Wait_For_Startup() function, that means that the startup message has
	    // been received and so the timer that monitors the time waited for the master to send the startup message has to be stopped
	    // and de-initialized

	    if(HAL_TIM_Base_Stop_IT(htim) != HAL_OK)
	      {
	      	  Error_Handler();
	      }

	    HAL_TIM_Base_MspDeInit(htim);
	}
}


// System status functions

// Start function

void System_start_sequence(UART_HandleTypeDef * huart)
{
	char msg[60];
	memset(msg,0,sizeof(msg));
    sprintf(msg,"System starting...\r\n1...\r\n2...\r\n3...\r\nSystem online!\r\n\n");
	HAL_UART_Transmit(huart, msg, strlen(msg), HAL_MAX_DELAY);
}

// Master reset function

void System_reset_sequence(UART_HandleTypeDef * huart)
{
	char msg[60];
	memset(msg,0,sizeof(msg));
    sprintf(msg,"System reseting...\r\n1...\r\n2...\r\n3...\r\nReset time!\r\n\n");
	HAL_UART_Transmit(huart, msg, strlen(msg), HAL_MAX_DELAY);

	// setting of this pin creates a bridge between GND and the reset pin, thus reseting the board

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}
