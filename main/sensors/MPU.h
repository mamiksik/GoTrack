#pragma once


#include <BLEServer.h>
#include <Storage.h>
#include <MPU9250.h>
#include <Arduino.h>
#include <mutex>
#include "UUID.h"

class MPU
{
public:
	enum Axes
	{
		X = 0,
		Y,
		Z,
	};


	virtual void readValues( bool notify ) { };


	virtual void notify( ) { };


	virtual float getValue( Axes axe ) { };

protected:
	MPU( BLEServer *bleServer, Storage & storage, MPU9250 & mpu, std::string serviceUUID, std::string xUUID,
	     std::string yUUID, std::string zUUID ) : _bleServer( bleServer ),
	                                              _storage( storage ),
	                                              _mpu( mpu )
	{

		_service = _bleServer->createService( serviceUUID );
		_xCharacteristic = _service->createCharacteristic(
				xUUID,
				BLECharacteristic::PROPERTY_NOTIFY
		);

		_yCharacteristic = _service->createCharacteristic(
				yUUID,
				BLECharacteristic::PROPERTY_NOTIFY
		);

		_zCharacteristic = _service->createCharacteristic(
				zUUID,
				BLECharacteristic::PROPERTY_NOTIFY
		);

		_service->start();

		byte c = _mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
		if ( c == 0x71 ) {
//			_logger.logInfo( "", "MPU9250 connected" );
		} else {
//			_logger.logError( "", "MPU9250 failed to connected" );
//			_logger.logDebug( "", "Wrong adress!" );
			exit( 1 );
		}
	}


	BLEServer *_bleServer;
	BLEService *_service;

	BLECharacteristic *_xCharacteristic;
	BLECharacteristic *_yCharacteristic;
	BLECharacteristic *_zCharacteristic;

	Storage & _storage;
	MPU9250 _mpu;
};


