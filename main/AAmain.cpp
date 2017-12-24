//#pragma once

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
#include "ESPLogSink.h"


//#include "SerialLogSink.h"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"



const uint8_t SERIAL_PINS[2] = { 16, 4 };
const uint8_t WIRE_PINS[2] = { 25, 33 };
const uint8_t SPI_PINS[3] = { 5, 4, 18};
const uint8_t SD_CS = 19;

MPU9250 mpu;

//Wire begin had to be commented out -> i2cInitFix(): Busy at initialization!
SSD1306 display( 0x3c, WIRE_PINS[ 0 ], WIRE_PINS[ 1 ] );


void setup(){
	//Device address
	pinMode(26, OUTPUT);
	pinMode(26, LOW);

	Wire.begin( WIRE_PINS[ 0 ], WIRE_PINS[ 1 ], 400000 );
	Wire.reset();

	Logger logger;

	display.init();
	display.flipScreenVertically();
	display.setTextAlignment( TEXT_ALIGN_LEFT );
	display.setFont( ArialMT_Plain_10 );

	logger.addSink( ALL, std::unique_ptr < LogSink >( new DisplayLogSink( display, 140, 64, 10 ) ) );
	logger.addSink( ALL, std::unique_ptr < LogSink >( new ESPLogSink( ) ) );

	logger.logInfo("SETUP", "Init BLE: %", CONFIG_DEVICE_NAME);

	BLEDevice::init( CONFIG_DEVICE_NAME );
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks( new ServerCallbacks() );

	Common common = Common();
	Storage storage = Storage(common);

	logger.logInfo("SETUP", "Creating Accelerometer");
	Accelerometer accelerometer = Accelerometer( pServer, storage, mpu);

	logger.logInfo("SETUP", "Starting BLE advertising");
	pServer->getAdvertising()->start();

//	ui.setTargetFPS( 23 );
//
//	ui.setActiveSymbol( activeSymbol );
//	ui.setInactiveSymbol( inactiveSymbol );
//
//	ui.setIndicatorPosition( LEFT );
//	ui.setIndicatorDirection( LEFT_RIGHT );
//	ui.setFrameAnimation( SLIDE_UP );
//
//
//	ui.setFrames( frames, frameCount );
//	ui.setOverlays( overlays, overlaysCount );
//
//	ui.init();
//	display.flipScreenVertically();
//	ui.disableAutoTransition();
//
//	ui.getUiState()->userData = & displayContext;
//	displayContext.newDataMPU = false;

	for(;;){
		if ( ServerCallbacks::deviceConnected) {
			logger.logInfo("SETUP", "Connected");
			accelerometer.readValues(true);
			logger.logInfo("SETUP", "Values: %", accelerometer.getValue(Accelerometer::X));
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