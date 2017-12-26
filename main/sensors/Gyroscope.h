#pragma once


#include <BLEServer.h>
#include <Storage.h>
#include <MPU9250.h>
#include "UUID.h"
#include "MPU.h"

class Gyroscope final : public MPU
{
public:
	Gyroscope( BLEServer *bleServer, Storage & storage, MPU9250 & mpu ) : MPU( bleServer, storage, mpu,
	                                                                           GYRO_SERVICE_UUID,
	                                                                           GYRO_CHARACTERISTIC_UUID_X,
	                                                                           GYRO_CHARACTERISTIC_UUID_Y,
	                                                                           GYRO_CHARACTERISTIC_UUID_Z )
	{
	}


	void readValues( bool notify = false );

	void notify( );

	float getValue( Axes axe );
};

