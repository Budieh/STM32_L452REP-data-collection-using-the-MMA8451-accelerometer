/*
 * MMA8451.c
 *
 *  Created on: Aug 19, 2023
 *      Author: User
 */

#include "MMA8451.h"

// helper functions

void EnableFastRead(I2C_HandleTypeDef hi2c)
{
	// enable the F_READ bit in CR1 for 8 Bit Resolution Read

	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }

	  content |= 2;

	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

void _12bit_to_16bit_c2_representation(int16_t * a)
{
      // memorize the sign bit of your variable

	  uint16_t sign = (*a) & 0x8000;

	  // if the number is negative, do the following

	  if(sign == 0x8000)
	  {

	  // turn your number into 1's complement and then into size&sign representations (the last 4 bits don't matter)

	  (*a) = (*a) - 0x0010;

	  (*a) = (~(*a));

      // clear the MSB ( the sign bit )

	  (*a) &= (~0x8000);

      // move the bits of your number 4 digits to the right

	  (*a) = (*a) >> 4;

      //add back the sign of your variable

	  (*a) |= sign;

      // revert back to 2's complement representation

    	  (*a) = (~(*a));

    	  // clear and replace the sign bit just to be sure

    	  (*a) &= (~0x8000);
    	  (*a) |= sign;

    	  if((*a) != 0x8FFF)
    		  (*a) += 0x0001;
    	  else
    		  (*a) = 0x0000;
      }

	  // if the number is possitive, just move the bits 4 digits to the right

	  else

	  (*a) = (*a) >> 4;
}

void DisableFastRead(I2C_HandleTypeDef hi2c)
{
	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
		  {
			  Error_Handler();
		  }

		  content &= (~2);

		  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
		  {
			  Error_Handler();
	      }
}

// mode-switch functions

void ACTIVE(I2C_HandleTypeDef hi2c)
{
	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	     {
	        Error_Handler();
	     }
	  content |= 1;
	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

void STANDBY(I2C_HandleTypeDef hi2c)
{
	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	     {
	        Error_Handler();
	     }
	  content &= (~1);
	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), CTRL_REG1, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

// range set functions

void _2g_range(I2C_HandleTypeDef hi2c)
{
	// set the first 2 bits in the XYZ_DATA_CFG to 00 for 2g range

	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }

	  content |= 0;

	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

void _4g_range(I2C_HandleTypeDef hi2c)
{
	// set the first 2 bits in the XYZ_DATA_CFG to 01 for 4g range

	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }

	  content |= 1;

	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

void _8g_range(I2C_HandleTypeDef hi2c)
{
	// set the first 2 bits in the XYZ_DATA_CFG to 10 for 8g range

	  uint8_t content;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }

	  content |= 0;

	  if( HAL_I2C_Mem_Write(&hi2c, (SLAVE_ADDR << 1), XYZ_DATA_CFG, 1, &content, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	      Error_Handler();
	  }
}

void Reset_range(I2C_HandleTypeDef hi2c)
{
	_2g_range(hi2c);
}

// acceleration read functions

int8_t X_Read_8BitRes (I2C_HandleTypeDef hi2c)
{
	  // enable the F_READ bit in CR1 for 8 Bit Resolution Read

      EnableFastRead(hi2c);

	  // read the contents of the OUT_X_MSB register

	  int8_t x = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_X_MSB, 1, &x, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  // disable the F_READ bit in CR1

	  DisableFastRead(hi2c);

	  return x;
}

int8_t Y_Read_8BitRes (I2C_HandleTypeDef hi2c)
{
	  // enable the F_READ bit in CR1 for 8 Bit Resolution Read

      EnableFastRead(hi2c);

	  // read the contents of the OUT_Y_MSB register

	  int8_t y = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_Y_MSB, 1, &y, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  // disable the F_READ bit in CR1

	  DisableFastRead(hi2c);

	  return y;
}

int8_t Z_Read_8BitRes (I2C_HandleTypeDef hi2c)
{
	  // enable the F_READ bit in CR1 for 8 Bit Resolution Read

      EnableFastRead(hi2c);

	  // read the contents of the OUT_Z_MSB register

	  int8_t z = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_Z_MSB, 1, &z, 1, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  // disable the F_READ bit in CR1

	  DisableFastRead(hi2c);

	  return z;
}

int16_t X_Read_12BitRes (I2C_HandleTypeDef hi2c)
{

	  // read the contents of the OUT_Y_MSB and OUT_Y_LSB registers

	  int16_t x = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_X_MSB, 2, &x, 2, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  _12bit_to_16bit_c2_representation(&x);

	  return x;
}

int16_t Y_Read_12BitRes (I2C_HandleTypeDef hi2c)
{

	  // read the contents of the OUT_Y_MSB and OUT_Y_LSB registers

	  int16_t y = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_Y_MSB, 2, &y, 2, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  _12bit_to_16bit_c2_representation(&y);

	  return y;
}

int16_t Z_Read_12BitRes (I2C_HandleTypeDef hi2c)
{

	  // read the contents of the OUT_Z_MSB and OUT_Z_LSB registers

	  int16_t z = 0;
	  if( HAL_I2C_Mem_Read(&hi2c, (SLAVE_ADDR << 1), OUT_Z_MSB, 2, &z, 2, HAL_MAX_DELAY) != HAL_OK )
	  {
	  	  Error_Handler();
	  }

	  _12bit_to_16bit_c2_representation(&z);

	  return z;
}

// measurement display function

float Acceleration_read(I2C_HandleTypeDef hi2c , axis A, resolution re, range ra)
{
	if(ra == _2g) _2g_range(hi2c);
	else if (ra == _4g) _4g_range(hi2c);
	else _8g_range(hi2c);

	if (re == _8bit)
	{
		int8_t a = 0;
		if(A == X) a = X_Read_8BitRes(hi2c);
		else if (A == Y) a = Y_Read_8BitRes(hi2c);
		else a = Z_Read_8BitRes(hi2c);

		if(ra == _2g ) return (float) a/64;
		else if(ra == _4g) return (float) a/32;
		else return (float) a/16;
	}

	else if (re == _12bit)
	{
		int16_t a = 0;
		if(A == X)
		   a = X_Read_12BitRes(hi2c);
		else if (A == Y)
		   a = Y_Read_12BitRes(hi2c);
		else
		   a = Z_Read_12BitRes(hi2c);

		if(ra == _2g ) return (float) a/1024;
		else if(ra == _4g) return (float) a/512;
		else return (float) a/256;
	}

	Reset_range(hi2c);

	// in case no return has been reached
	return 0;
 }

// X-Y axis overturn functions

// 1 - overturn is detected
// 2 - no overturn is detected

uint8_t Overturn_detection(I2C_HandleTypeDef hi2c, resolution re, range ra, margin m)
{
	if(ra == _2g) _2g_range(hi2c);
	else if (ra == _4g) _4g_range(hi2c);
	else _8g_range(hi2c);

	if (re == _8bit)
	{
		int8_t x , y;
		x = X_Read_8BitRes(hi2c);
		y = Y_Read_8BitRes(hi2c);

		if( abs(x) <= m && abs(y) <= m ) return 0;
		else return 1;
	}

	else if (re == _12bit)
	{
		int16_t x , y;
		x = X_Read_12BitRes(hi2c);
		y = Y_Read_12BitRes(hi2c);

		if( abs(x) <= m && abs(y) <= m ) return 0;
		else return 1;
	}

	Reset_range(hi2c);

	// in case no return has been reached
	return 0;
 }

uint8_t Slip_detection(I2C_HandleTypeDef hi2c, resolution re, range ra, int16_t * X_past, int16_t * Y_past, margin m)
{
	if(ra == _2g) _2g_range(hi2c);
	else if (ra == _4g) _4g_range(hi2c);
	else _8g_range(hi2c);

	if (re == _8bit)
	{
		int8_t x , y;
		x = X_Read_8BitRes(hi2c);
		y = Y_Read_8BitRes(hi2c);

		if( abs(x - (int8_t)(* X_past)) <= m && abs(y - (int8_t)(* Y_past)) <= m )
		{
		   // update X_past and Y_past for the next function call

		   * X_past = x;
		   * Y_past = y;

		   return 0;
		}
		else
		{
	       // update X_past and Y_past for the next function call

	       * X_past = x;
		   * Y_past = y;

		   return 1;
		}
	}

	else if (re == _12bit)
	{
		int16_t x , y;
		x = X_Read_12BitRes(hi2c);
		y = Y_Read_12BitRes(hi2c);

		if( abs(x - (* X_past)) <= m && abs(y - (* Y_past)) <= m )
		{
			// update X_past and Y_past for the next function call

			* X_past = x;
			* Y_past = y;

			return 0;
		}
		else
		{
		   // update X_past and Y_past for the next function call

		   * X_past = x;
		   * Y_past = y;

		   return 1;
		}
	}

	Reset_range(hi2c);

	// in case no return has been reached

	return 0;
 }
