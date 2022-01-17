/*
 * AHT20.h
 *
 *	AHT20 Temperature and Humidity Sensor I2C Driver
 *
 *	Author: Jake Wahl
 *  Created: Oct 7, 2021
 *
 */

#include "AHT20.h"


/* Read AHT20 Sensor */

HAL_StatusTypeDef AHT20_Read( AHT20 *dev, uint8_t *data, uint8_t length ) {

	return HAL_I2C_Master_Receive(dev->i2cHandle, AHT20_I2C_ADD, data, length, HAL_MAX_DELAY );

}

/* Write to AHT20 Sensor */

HAL_StatusTypeDef AHT20_Write( AHT20 *dev, uint8_t *data, uint8_t length ) {

	return HAL_I2C_Master_Transmit(dev->i2cHandle, AHT20_I2C_ADD, data, length, HAL_MAX_DELAY );

}

/* Get State Data Byte of AHT20 Sensor */

uint8_t AHT20_GetState( AHT20 *dev, uint8_t *data) {

	if ( HAL_I2C_Master_Receive( dev->i2cHandle, AHT20_I2C_ADD, data, 1, HAL_MAX_DELAY ) == HAL_OK ) {

		return *data;

	} else {

		return 0;
	}
}

/* Initialize AHT20 Sensor */

uint8_t AHT20_Init( AHT20 *dev, I2C_HandleTypeDef *i2cHandle ) {

	dev->i2cHandle		= i2cHandle;

	dev->humPercent		= 0.0f;
	dev->tempC			= 0.0f;

	uint8_t errNum = 0;
	HAL_StatusTypeDef status;

	/* Check if Device is ready to communicate (no Part ID) */
	status = HAL_I2C_IsDeviceReady( i2cHandle, AHT20_I2C_ADD, 1, HAL_MAX_DELAY );

	/* Add errors to return */
	errNum += ( status != HAL_OK );

	/* Initialize the sensor */
	uint8_t cmd[3];
	cmd[0] = 0xBE;
	cmd[1] = 0x08;
	cmd[2] = 0x00;
	status = AHT20_Write( dev, cmd, 3 );

	/* Add errors to return */
	errNum += ( status != HAL_OK );

	/* Return error number, 0 if successful */
	return errNum;

}

/* Read & Transform Temperature and Humidity Data from AHT20 Sensor */

HAL_StatusTypeDef AHT20_ReadTempHum_Data( AHT20 *dev ) {

	//uint8_t status;
	uint8_t devData[6];
	uint32_t humData;
	uint32_t tempData;
	uint8_t state;

	/* Get Current State of Sensor (if initialized correctly) */
	AHT20_GetState( dev, state );

	/* Check if BIT3 of state is set, continue if it is, error if not */
	if ( state & (1 << 3) == 0 ) {

		return HAL_ERROR;

	}

	/* Send Trigger Measurement Command */
	uint8_t cmd[3];
	cmd[0] = AHT20_TRIG_MEASURE;
	cmd[1] = 0x33;
	cmd[2] = 0x00;
	AHT20_Write( dev, cmd, 3 );

	/* Wait */
	HAL_Delay(50);

	/* Read Sensor Data */
	AHT20_Read( dev, devData, 6 );

	/* devData = State, Hum1, Hum2, Hum3, Temp1, Temp2 */

	/* Humidity Calculations */
	humData = devData[1];
	humData = humData << 8;
	humData |= devData[2];
	humData = humData << 4;
	humData |= ( devData[3] >> 4 );
	dev->humPercent = ( (float) humData * 100 ) / 0x100000;

	/* Temperature Calculations */
	tempData = devData[3] & 0x0F;
	tempData = tempData << 8;
	tempData |= devData[4];
	tempData = tempData << 8;
	tempData |= devData[5];
	dev->tempC = ( ( (float) tempData / 0x100000 ) * 200 ) - 50;

	return HAL_OK;
}


