//#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

#include <BLEDevice.h>
//#include <BLEUtils.h>
//#include <BLEServer.h>
#include <Gyroscope.h>
#include <Magnetometer.h>
#include <Adafruit_BMP280.h>
#include <Common.h>
//#include <examples/SSD1306UiDemo/images.h>
#include <esp_heap_trace.h>
#include "SSD1306.h"
//#include "OLEDDisplayUi.h"
//#include "MPU9250.h"
#include "sdkconfig.h"

#include "ServerCallback.h"

#include "logging.hpp"
#include "Accelerometer.h"
#include "DisplayLogSink.h"
#include "Tasks.h"




//#include "SerialLogSink.h"



//const uint8_t SERIAL_PINS[2] = { 16, 4 };
//const uint8_t WIRE_PINS[2] = { 25, 33 };
//const uint8_t SPI_PINS[3] = { 5, 4, 18 };
//const uint8_t BTN = 32;

#define WIRE_PIN_SDA 25
#define WIRE_PIN_SCL 33
#define WIRE_FREQ 700000

#define SPI_PIN_SCK 5
#define SPI_PIN_MISO 4
#define SPI_PIN_MOSI 19

#define GND_PIN 26

//const uint8_t SD_CS = 19;
void setup( )
{
	pinMode( GND_PIN, OUTPUT );
	pinMode( GND_PIN, LOW );
	pinMode( BTN_PIN, INPUT );

	Wire.begin( WIRE_PIN_SDA, WIRE_PIN_SCL, WIRE_FREQ );
	Wire.reset();

	SPI.begin( SPI_PIN_SCK, SPI_PIN_MISO, SPI_PIN_MOSI );
	SD.begin( SD_CS, SPI );
	ESP_LOGE('test', "%i", SD.cardType());

	SSD1306Wire display = SSD1306Wire( 0x3c, WIRE_PIN_SDA, WIRE_PIN_SCL );
	OLEDDisplayUi ui = OLEDDisplayUi( & display );

	display.init();
	display.flipScreenVertically();
	display.setTextAlignment( TEXT_ALIGN_LEFT );
	display.setFont( ArialMT_Plain_10 );

	Logger logger;
	logger.addSink( ALL, std::unique_ptr < LogSink >( new DisplayLogSink( display, 140, 64, 10 ) ) );

	BLEDevice::init( CONFIG_DEVICE_NAME );
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks( new ServerCallbacks() );


	Storage storage = Storage();

	Adafruit_BMP280 bmp;
	bmp.begin(0x76);

	MPU9250 mpu;
	mpu.initMPU9250();
	mpu.calibrateMPU9250( mpu.gyroBias, mpu.accelBias );
	ESP_LOGE("test", "test: %f", bmp.readTemperature());
	// Must be called before updating quaternions!
	// mpu.updateTime(); ???

//	byte c = mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
//	if ( c == 0x71 ) {
//
//	} else {
//		exit( 1 );
//	}

	mpu.initAK8963(mpu.magCalibration);

	display.drawProgressBar(10, 50, 100, 5, 00);
	display.display();
	delay(100);

	logger.logInfo( "SETUP", "Creating Accelerometer" );
	Accelerometer accelerometer = Accelerometer( pServer, storage, mpu );

	display.drawProgressBar(10, 50, 100, 5, 20);
	display.display();
	delay(100);

	logger.logInfo( "SETUP", "Creating gyroscope" );
	Gyroscope gyroscope = Gyroscope( pServer, storage, mpu );

	display.drawProgressBar(10, 50, 100, 5, 40);
	display.display();
	delay(100);

	logger.logInfo( "SETUP", "Creating magnetometer" );
	Magnetometer magnetometer = Magnetometer( pServer, storage, mpu );

	display.drawProgressBar(10, 50, 100, 5, 60);
	display.display();
	delay(100);

	logger.logInfo( "SETUP", "Creating common" );
	Common common = Common( pServer, storage, bmp );


	DisplayContext d{ };
	Tasks::createInstance( gyroscope, accelerometer, common, ui, d, display/*, logger*/);

	display.drawProgressBar(10, 50, 100, 5, 80);
	display.display();
	delay(100);

	TaskHandle_t tasks[2] = {};

	xTaskCreate( & Tasks::staticUpdate, "Update", 4048, NULL, 1, &tasks[0] );
	xTaskCreate( & Tasks::staticUiUpdate, "UiUpdate", 4048, NULL, 2, &tasks[1] );

	pServer->getAdvertising()->start();

	for ( ;; ) {
		uint32_t free = esp_get_free_heap_size();
		ESP_LOGI( "core", "Free: %i", free );
		delay( 5000 );
//		vTaskDelete(tasks[0]);
//		vTaskDelete(tasks[1]);
	}
};


void loop( )
{

}
