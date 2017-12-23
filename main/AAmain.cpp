#pragma once

#include "Arduino.h"
#include "Wire.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "SSD1306.h"
#include "MPU9250.h"
#include "sdkconfig.h"

#include "ServerCallback.h"

#include "logging.hpp"
#include "Accelerometer.h"
#include "DisplayLogSink.h"


//#include "SerialLogSink.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"



const uint8_t SERIAL_PINS[2] = { 16, 4 };
const uint8_t WIRE_PINS[2] = { 25, 33 };
const uint8_t SPI_PINS[3] = { 5, 4, 18};
const uint8_t SD_CS = 19;

MPU9250 mpu;
//SSD1306 display( 0x3c, WIRE_PINS[ 0 ], WIRE_PINS[ 1 ] );


void setup(){
	pinMode(26, OUTPUT);
	pinMode(26, LOW);

	Wire.begin( WIRE_PINS[ 0 ], WIRE_PINS[ 1 ], 400000 );
	Wire.reset();

	Logger logger;

//	display.init();
//	display.flipScreenVertically();
//	display.setTextAlignment( TEXT_ALIGN_LEFT );
//	display.setFont( ArialMT_Plain_10 );
//
//	logger.addSink( ALL, std::unique_ptr < LogSink >( new DisplayLogSink( display, 140, 64, 10 ) ) );

//	logger.logInfo("Test", "Test000");

	BLEDevice::init( CONFIG_DEVICE_NAME );
	BLEServer *pServer = BLEDevice::createServer();
	BLEService *pService = pServer->createService(SERVICE_UUID);
	pServer->setCallbacks( new ServerCallbacks() );

//	logger.logInfo("Test", "Test");
//	logger.logInfo("Test", "Test2");
	ESP_LOGI("tag","+");

	Common common = Common();
	Storage storage = Storage(common);

	Accelerometer accelerometer = Accelerometer( pServer, storage, mpu);
	mpu.MPU9250SelfTest( mpu.SelfTest );
	ESP_LOGI("tag","+");

	BLECharacteristic *pCharacteristic = pService->createCharacteristic(
			CHARACTERISTIC_UUID,
			BLECharacteristic::PROPERTY_READ |
			BLECharacteristic::PROPERTY_WRITE |
			BLECharacteristic::PROPERTY_NOTIFY
	);

	pCharacteristic->addDescriptor( new BLE2902() );
	pCharacteristic->setValue("test");
	pService->start();

	pServer->getAdvertising()->start();
	ESP_LOGI("tag","Characteristic defined! Now you can read it in your phone!");
	for(;;){
		ESP_LOGI("tag","----");
		if ( ServerCallbacks::deviceConnected) {
			logger.logInfo("Test", "---");
			accelerometer.readValues(true);
			ESP_LOGI("tag","%f", accelerometer.getValue(Accelerometer::X));
		}
		delay( 1000 );
	}

};


void loop(){

}

//
//#if CONFIG_FREERTOS_UNICORE
//#define ARDUINO_RUNNING_CORE 0
//#else
//#define ARDUINO_RUNNING_CORE 1
//#endif
//
//
//extern "C" void app_main( ) {
//	initArduino();
//	micros();
////	setup();
//	xTaskCreatePinnedToCore(setup, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
//}