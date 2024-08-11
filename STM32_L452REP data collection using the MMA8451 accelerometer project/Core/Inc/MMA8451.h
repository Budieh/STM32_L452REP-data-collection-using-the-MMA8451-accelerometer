/*
 * MMA8451.h
 *
 *  Created on: Aug 19, 2023
 *      Author: User
 */

#ifndef INC_MMA8451_H_
#define INC_MMA8451_H_

/* NOTE:
Slave address: 0x1C <=> 0b00011100

Note:
-> The address of the slave was determined by using an infinite loop that tries to address from 0x00 to 0x7F
(shifted to the left by 1), loop that stops when the transmit function returns the status HAL_OK, a redundant
process with the sole purpose of gathering information that should have been written directly on the PCB in my opinion.

-> Initially it didn't work because the frequency at which the I2C was set was too high, I don't know how the value of
the pull-up resistors placed on the SDA and SCL lines of the sensor but the maximum frequency accepted for I2C communication
is around 15 kHz, which is dubious to say the least. ( try to reduce the fan-off current at frequencies approaching 15 kHz
tough, the sensor might jam)

-> The value of the WHO_AM_I register is 0x2A <=> 0b00101010, which is definitely not 0x1C. Now the question arises, how
was I supposed to read the address of the WHO_AM_I register without knowing the address of the slave which is stored in
the WHO_AM_I register and, ironically, it's not even the right one.
*/

// Includes

#include "main.h"
#include <math.h>

// common useful macros

#define ENABLE  1
#define DISABLE 0

#define SET     ENABLE
#define RESET   DISABLE

// axis macros

#define axis uint8_t

#define X 0
#define Y 1
#define Z 2

// resolution macros

#define resolution uint8_t

#define _8bit  0
#define _12bit 1

// range macros

#define range uint8_t
#define _2g 0
#define _4g 1
#define _8g 2

// user limit macros

#define margin uint16_t

// device slave address

#define SLAVE_ADDR 0x1C

// register addresses

#define STATUS           0x00
#define OUT_X_MSB        0x01
#define OUT_X_LSB        0x02
#define OUT_Y_MSB        0x03
#define OUT_Y_LSB        0x04
#define OUT_Z_MSB        0x05
#define OUT_Z_LSB        0x06
#define SYSMODE          0x0B
#define INT_SOURCE       0x0C
#define WHO_AM_I         0x0D
#define XYZ_DATA_CFG     0x0E
#define HP_FILTER_CUTOFF 0x0F
#define PL_STATUS        0x10
#define PL_CFG           0x11
#define PL_COUNT         0x12
#define PL_BF_ZCOMP      0x13
#define P_L_THS_REG      0x14
#define FF_MT_CFG        0x15
#define FF_MT_SRC        0x16
#define FF_MT_THS        0x17
#define FF_MT_COUNT      0x18
#define TRANSIENT_CFG    0x1D
#define TRANSIENT_SRC    0x1E
#define TRANSIENT_THS    0x1F
#define TRANSIENT_COUNT  0x20
#define PULSE_CFG        0x21
#define PULSE_SRC        0x22
#define PULSE_THSX       0x23
#define PULSE_THSY       0x24
#define PULSE_THSZ       0x25
#define PULSE_TMLT       0x26
#define PULSE_LTCY       0x27
#define PULSE_WIND       0x28
#define ASLP_COUNT       0x29
#define CTRL_REG1        0x2A
#define CTRL_REG2        0x2B
#define CTRL_REG3        0x2C
#define CTRL_REG4        0x2D
#define CTRL_REG5        0x2E
#define OFF_X            0x2F
#define OFF_Y            0x30
#define OFF_Z            0x31

// function prototypes

// helper functions

void EnableFastRead(I2C_HandleTypeDef);
void DisableFastRead(I2C_HandleTypeDef);
void _12bit_to_16bit_c2_representation(int16_t *);

// gyroscope active/standby mode enablers

void ACTIVE(I2C_HandleTypeDef);
void STANDBY(I2C_HandleTypeDef);

// x/y/z read (8 and 12 bit)

int8_t X_Read_8BitRes(I2C_HandleTypeDef);
int8_t Y_Read_8BitRes(I2C_HandleTypeDef);
int8_t Z_Read_8BitRes(I2C_HandleTypeDef);

int16_t X_Read_12BitRes(I2C_HandleTypeDef);
int16_t Y_Read_12BitRes(I2C_HandleTypeDef);
int16_t Z_Read_12BitRes(I2C_HandleTypeDef);

// range manipulation functions

void _2g_range(I2C_HandleTypeDef);
void _4g_range(I2C_HandleTypeDef);
void _8g_range(I2C_HandleTypeDef);
void Reset_range(I2C_HandleTypeDef);

// measurement display function

float Acceleration_read(I2C_HandleTypeDef, axis, resolution, range);

// X-Y axis overturn function

// 1 - overturn is detected
// 2 - no overturn is detected

uint8_t Overturn_detection(I2C_HandleTypeDef, resolution, range, margin);

// slip detection function

// 1 - slip is detected
// 2 - no slip is detected

// NOTE: this function requires X and Y axis measurements as arguments to compare with the newly-calculated
// measurements due to uncertainty in measurement frequency, this function being meant to be used within a
// timer. X and Y past measurements are passed as pointers so that they can be modified for the next call of
// the function.

uint8_t Slip_detection(I2C_HandleTypeDef, resolution, range, int16_t *, int16_t *, margin);

#endif /* INC_MMA8451_H_ */
