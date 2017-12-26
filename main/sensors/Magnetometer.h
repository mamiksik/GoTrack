#pragma once

#include <Arduino.h>
#include "UUID.h"
#include "BLEServer.h"
#include "MPU9250.h"

#include "BLE2902.h"
#include "Storage.h"
#include "MPU.h"


class Magnetometer final : public MPU
{
public:

	Magnetometer( BLEServer *bleServer, Storage & storage, MPU9250 & mpu ) : MPU( bleServer, storage, mpu,
	                                                                              MAG_SERVICE_UUID,
	                                                                              MAG_CHARACTERISTIC_UUID_X,
	                                                                              MAG_CHARACTERISTIC_UUID_Y,
	                                                                              MAG_CHARACTERISTIC_UUID_Z ) { }


	void readValues( bool notify = false );

	void notify( );

	float getValue( Axes axe );
};


