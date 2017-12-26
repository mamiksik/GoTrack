#pragma once

#include <Arduino.h>
#include "UUID.h"
#include "BLEServer.h"
#include "MPU9250.h"

#include "BLE2902.h"
#include "Storage.h"
#include "MPU.h"

//#include <logging.hpp>


class Accelerometer final : public MPU
{
public:

	Accelerometer( BLEServer *bleServer, Storage & storage, MPU9250 & mpu ) : MPU( bleServer, storage, mpu,
	                                                                               ACCELEROMETER_SERVICE_UUID,
	                                                                               ACCELEROMETER_CHARACTERISTIC_UUID_X,
	                                                                               ACCELEROMETER_CHARACTERISTIC_UUID_Y,
	                                                                               ACCELEROMETER_CHARACTERISTIC_UUID_Z )
	{ }


	void readValues( bool notify = false );

	void notify( );

	float getValue( Axes axe );
};


