/*
 * AHT20.h
 *
 *	AHT20 Temperature and Humidity Sensor I2C Driver
 *
 *	Author: Jake Wahl
 *  Created: Oct 7, 2021
 *
 */

#ifndef INC_AHT20_H_
#define INC_AHT20_H_

#include "stm32f4xx_hal.h" /* Needed for I2C */

/* DEFINES */

#define AHT20_I2C_ADD ( 0x38 << 1) /* Left shifted, LSB is R/W */
#define AHT20_TRIG_MEASURE 0xAC
#define AHT20_STATUS_BUSY 0X80




/* Sensor Struct */

typedef struct {

	/* I2C Handle */
	I2C_HandleTypeDef *i2cHandle;

	/* Relative Humidity Data in percent (%) */
	float humPercent;

	/* Temperature Data in degrees Celsius */
	float tempC;

} AHT20;

uint8_t AHT20_Init( AHT20 *dev, I2C_HandleTypeDef *i2cHandle );

uint8_t AHT20_GetState( AHT20 *dev, uint8_t *data);

HAL_StatusTypeDef AHT20_ReadTempHum_Data( AHT20 *dev );

HAL_StatusTypeDef AHT20_Read( AHT20 *dev, uint8_t *data, uint8_t length );

HAL_StatusTypeDef AHT20_Write( AHT20 *dev, uint8_t *data, uint8_t length);


#endif /* INC_AHT20_H_ */
