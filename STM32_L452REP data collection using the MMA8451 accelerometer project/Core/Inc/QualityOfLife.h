/*
 * QualityOfLife.h
 *
 *  Created on: Aug 28, 2023
 *      Author: User
 */

#ifndef INC_QUALITYOFLIFE_H_
#define INC_QUALITYOFLIFE_H_

// Includes

#include "main.h"


// General-purpose macros


// Filter configuration macros

// CAN filter mask macro ( from 0x000 to 0x7FF )

#define MASK   uint16_t

// CAN filter id macro ( from 0x000 to 0x7FF )

#define ID     uint16_t

// Filter number macro ( from 0x00 to 0x0E )

#define FLTR_NR  uint8_t


// General-purpose function definitions


// CAN setup functions

// Filter configuration function

// mask - value of the address mask ( 11 bits resolution - max: 0x7FF ) ; make the bit 1 if you want to check if the bit
// on the corresponding position in the id variable matches the bit on the corresponding position in the ID of the message
// sent through CAN, if it does for all the bits on the positions marked with 1 in the mask variable the message passes
// through and populates FIFO0, otherwise it gets rejected.

// id - value of the check id ( 11 bits resolution - max: 0x7FF ) ; its use is explained alongside the description of the
// mask variable 2 rows above

void Filter_Configuration(CAN_HandleTypeDef * hcan, MASK mask, ID id, FLTR_NR fltr);


// Interrupt enabling function

// this function is used to activate certain flags of the CAN peripheral ( flags specified in the flags variable ), so that
// the interrupts for said events specified by the flags can be addressed

void CAN_Interrupt_Enabler(CAN_HandleTypeDef * hcan, uint32_t flags);


// Startup message await function

// this function is meant to keep the program in an infinite loop until the startup message is sent through CAN

void CAN_Wait_For_Startup(CAN_HandleTypeDef * hcan, UART_HandleTypeDef * huart, TIM_HandleTypeDef * htim);

// System status functions

// System start function

// this function is exclusively made to signify the beginning of the program, consisting of a transmission through UART of
// a suggestive message
// NOTE: to be used after the peripheral initialization segment of the program and before the starting sequences of said
// peripherals, especially timers, so that it doesn't interfere with any interrupts

void System_start_sequence(UART_HandleTypeDef * huart);

// System master reset function

// this function acts as a system master reset, transmitting a suggestive message through UART and then setting a GPIO pin
// that opens a transistor, creating a bridge that connects a ground pin with the NTRS pin ( master reset pin, by default
// pulled high ; a low signal on this pin activates the reset interrupt, thus reseting the board ). The system restarts with
// the default settings, a successful reset being signified by the message transmitted by the System start function through
// UART

void System_reset_sequence(UART_HandleTypeDef * huart);

#endif /* INC_QUALITYOFLIFE_H_ */
