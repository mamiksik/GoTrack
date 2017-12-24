#pragma once

#include <Arduino.h>
//#include <logging.hpp>
#include "BLEServer.h"
#include "MPU9250.h"

#include "BLE2902.h"
#include "Storage.h"

//#include <logging.hpp>

#define ACCELEROMETER_SERVICE_UUID        "9036ca89-2ad3-46f0-a9c1-7b6811fe2bd8"
#define ACCELEROMETER_CHARACTERISTIC_UUID_X "cc29118a-2988-4c55-9f51-60c2f5110a77"
#define ACCELEROMETER_CHARACTERISTIC_UUID_Y "b5f93337-d102-4b70-a1f1-9c2732b7a62e"
#define ACCELEROMETER_CHARACTERISTIC_UUID_Z "364b4322-6704-4fdc-bac4-35055fc32a50"

class Accelerometer
{
public:
	enum Axes
	{
		X = 0,
		Y,
		Z,
	};

	Accelerometer( BLEServer *bleServer, Storage & storage, MPU9250 & mpu9250 ) : /*_logger( l ), */_bleServer(
			bleServer ),
	                                                                       _storage( storage ),
	                                                                       _mpu( mpu9250 )
	{

		_accelerometerService = _bleServer->createService( ACCELEROMETER_SERVICE_UUID );
		_xCharacteristic = _accelerometerService->createCharacteristic(
				ACCELEROMETER_CHARACTERISTIC_UUID_X,
				BLECharacteristic::PROPERTY_READ |
				BLECharacteristic::PROPERTY_NOTIFY
		);
//		_xCharacteristic->addDescriptor( new BLE2902() );

		_yCharacteristic = _accelerometerService->createCharacteristic(
				ACCELEROMETER_CHARACTERISTIC_UUID_Y,
				BLECharacteristic::PROPERTY_READ |
				BLECharacteristic::PROPERTY_NOTIFY
		);
//		_yCharacteristic->addDescriptor( new BLE2902() );

		_zCharacteristic = _accelerometerService->createCharacteristic(
				ACCELEROMETER_CHARACTERISTIC_UUID_Z,
				BLECharacteristic::PROPERTY_READ |
				BLECharacteristic::PROPERTY_NOTIFY
		);
//		_zCharacteristic->addDescriptor( new BLE2902() );

		_accelerometerService->start();

		byte c = _mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
		if ( c == 0x71 ) {
//			_logger.logInfo( "", "MPU9250 connected" );
		} else {
//			_logger.logError( "", "MPU9250 failed to connected" );
//			_logger.logDebug( "", "Wrong adress!" );
		exit( 1 );
		}

//		_mpu.MPU9250SelfTest( _mpu.SelfTest );
		_mpu.calibrateMPU9250( _mpu.gyroBias, _mpu.accelBias );

	}


	void readValues( bool notify = false );

	void notify( );

	float getValue( Axes axe );

private:
	/*BaseLogger< std::string, DummyClock > & _logger;*/

	BLEServer *_bleServer;
	BLEService *_accelerometerService;

	BLECharacteristic *_xCharacteristic;
	BLECharacteristic *_yCharacteristic;
	BLECharacteristic *_zCharacteristic;

	Storage & _storage;
	MPU9250 _mpu;
};


