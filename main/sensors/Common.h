#pragma once

#include <BLEServer.h>
#include <Storage.h>
#include <MPU9250.h>
#include <UUID.h>
#include <Adafruit_BMP280.h>
#include <BLE2902.h>

//This class gathers clock, temperature and so on...
//Not sure if it is good idea, but saves memory
class Common final
{
public:
	enum Value
	{
		PRESSURE,
		TEMPERATURE
	};


	Common( BLEServer *bleServer, Storage & storage, Adafruit_BMP280 & bmp ) : _bleServer( bleServer ),
	                                                                           _storage( storage ),
	                                                                           _bmp( bmp )
	{
		_service = _bleServer->createService( COMMON_SERVICE_UUID );
		_temperatureCharacteristic = _service->createCharacteristic(
				COMMON_CHARACTERISTIC_UUID_TEMP,
				BLECharacteristic::PROPERTY_NOTIFY
		);

		_pressureCharacteristic = _service->createCharacteristic(
				COMMON_CHARACTERISTIC_UUID_PRESSU,
				BLECharacteristic::PROPERTY_NOTIFY
		);

		_temperatureCharacteristic->addDescriptor(new BLE2902());
		_pressureCharacteristic->addDescriptor(new BLE2902());

		_service->start();
	}


	void readValues( bool notify  );


	void notify( );


	float getValue( Value value );

private:
	float _pressure;
	float _temperature;

	BLEServer *_bleServer;
	BLEService *_service;

	BLECharacteristic *_temperatureCharacteristic;
	BLECharacteristic *_pressureCharacteristic;

	Storage & _storage;
	Adafruit_BMP280 _bmp;
};

