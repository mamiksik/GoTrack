//#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Gyroscope.h>
#include <Magnetometer.h>
#include <Adafruit_BMP280.h>
#include <Common.h>
#include <Frames.h>
#include <examples/SSD1306UiDemo/images.h>
#include <esp_heap_trace.h>
#include "SSD1306.h"
#include "OLEDDisplayUi.h"
#include "MPU9250.h"
#include "sdkconfig.h"

#include "ServerCallback.h"

#include "logging.hpp"
#include "Accelerometer.h"
#include "DisplayLogSink.h"
#include "ESPLogSink.h"


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
#define SPI_PIN_MOSI 18

#define BTN_PIN 32
#define GND_PIN 26

//const uint8_t SD_CS = 19;

class MainC
{

private:

public:
	MainC( Gyroscope & gyroscope, Accelerometer & accelerometer, Common & common,
	       OLEDDisplayUi & uiO, DisplayContext & displayContext, SSD1306 & display/*, Logger & logger */) : _gyroscope(
			gyroscope ),
	                                                                                                        _accelerometer(
			                                                                                                        accelerometer ),
	                                                                                                        _common(
			                                                                                                        common ),
	                                                                                                        _ui( uiO ),
	                                                                                                        _displayContext(
			                                                                                                        displayContext ),
	                                                                                                        _display(
			                                                                                                        display )/*,
	                                                                                                  _logger( logger )*/
	{
	}


public:

	static MainC *getInstance( )
	{
		return MainC::_instance;
	}


	static void
	createInstance( Gyroscope & gyroscope, Accelerometer & accelerometer, Common & common, OLEDDisplayUi & ui,
	                DisplayContext & displayContext, SSD1306 & display/*, Logger& logger*/)
	{
		assert( !MainC::_instance );
		MainC::_instance = new MainC( gyroscope, accelerometer, common, ui, displayContext, display/*, logger*/);
	}


	static void destroyInstance( )
	{
		assert( MainC::_instance );
		delete MainC::_instance;
	}


	static void staticUpdate( void *pvParameter )
	{
		MainC::getInstance()->update();
	}


	static void staticUiUpdate( void *pvParameter )
	{
		MainC::getInstance()->uiUpdate();
	}


	void update( )
	{
		ESP_LOGE( "test", "UPDATE" );
		while ( true ) {
			if ( _mutex.try_lock() ) {

				_accelerometer.readValues( true );
				_gyroscope.readValues( true );
//				_common.notify();
				_mutex.unlock();
			} else {
				ESP_LOGI( "update", "Locked Update" );
			}
			delay( 100 );
		}
	}


	void uiUpdate( )
	{
		_ui.setTargetFPS( 25 );
		_ui.setActiveSymbol( activeSymbol );
		_ui.setInactiveSymbol( inactiveSymbol );
		_ui.setIndicatorPosition( LEFT );
		_ui.setIndicatorDirection( LEFT_RIGHT );
		_ui.setFrameAnimation( SLIDE_UP );


		FrameCallback frames[] = { introFrame, gyroFrame, accFrame, magFrame, dataFrame };
		uint8_t frameCount = 5;

		OverlayCallback overlays[] = { packetOverlay, savedOverlay };
		uint8_t overlaysCount = 2;

		_ui.setFrames( frames, frameCount );
		_ui.setOverlays( overlays, overlaysCount );
		_ui.init();
		_ui.disableAutoTransition();
		_display.flipScreenVertically();
		_ui.getUiState()->userData = & _displayContext;
		_displayContext.newDataMPU = false;

		ESP_LOGE( "test", "UI Update" );
		while ( true ) {
			_displayContext.newDataMPU = true;
			if ( _mutex.try_lock() ) {

				//In next version Display should be on second physical wire so update could go in parallel
				_ui.update();

				_displayContext.gyroX = _gyroscope.getValue( Gyroscope::X );
				_displayContext.gyroY = _gyroscope.getValue( Gyroscope::Y );
				_displayContext.gyroZ = _gyroscope.getValue( Gyroscope::Z );

				_displayContext.accX = _accelerometer.getValue( Accelerometer::X );
				_displayContext.accY = _accelerometer.getValue( Accelerometer::Y );
				_displayContext.accZ = _accelerometer.getValue( Accelerometer::Z );

				//displayContext.magX = mpu.mx;
				//displayContext.magY = mpu.my;
				//displayContext.magZ = mpu.mz;

				_displayContext.temp = _common.getValue( Common::TEMPERATURE );

				_mutex.unlock();
			} else {
				ESP_LOGI( "update", "Locked UI" );
			}
			if ( digitalRead( BTN_PIN ) == 1 ) {
				_ui.nextFrame();
			}

			delay( 10 );
		}
	}


	Gyroscope & _gyroscope;
	Accelerometer & _accelerometer;
	Common & _common;

	OLEDDisplayUi & _ui;
	DisplayContext & _displayContext;
	SSD1306 & _display;

/*	Logger& _logger;*/

	mutable std::mutex _mutex;

	static MainC *_instance;

};

MainC *MainC::_instance = nullptr;


void setup( )
{
	pinMode( GND_PIN, OUTPUT );
	pinMode( GND_PIN, LOW );
	pinMode( BTN_PIN, INPUT );

	Wire.begin( WIRE_PIN_SDA, WIRE_PIN_SCL, WIRE_FREQ );
	Wire.reset();

	SPI.begin( SPI_PIN_SCK, SPI_PIN_MISO, SPI_PIN_MOSI );
	SD.begin( SD_CS, SPI );

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

	MPU9250 mpu;
	mpu.initMPU9250();
	mpu.calibrateMPU9250( mpu.gyroBias, mpu.accelBias );
	// Must be called before updating quaternions!
	// mpu.updateTime(); ???

	Adafruit_BMP280 bmp;
	bmp.begin();

	logger.logInfo( "SETUP", "Creating Accelerometer" );
	Accelerometer accelerometer = Accelerometer( pServer, storage, mpu );

	logger.logInfo( "SETUP", "Creating gyroscope" );
	Gyroscope gyroscope = Gyroscope( pServer, storage, mpu );

	logger.logInfo( "SETUP", "Creating magnetometer" );
	Magnetometer magnetometer = Magnetometer( pServer, storage, mpu );

	logger.logInfo( "SETUP", "Creating common" );
	Common common = Common( pServer, storage, bmp );

	display.clear();
	DisplayContext d{ };

	MainC::createInstance( gyroscope, accelerometer, common, ui, d, display/*, logger*/);


	xTaskCreate( & MainC::staticUpdate, "Update", 4048, NULL, 1, NULL );
	xTaskCreate( & MainC::staticUiUpdate, "UiUpdate", 4048, NULL, 2, NULL );


	for ( ;; ) {
		uint32_t free = esp_get_free_heap_size();
		ESP_LOGI( "core", "Free: %i", free );
		delay( 5000 );
	}
};


void loop( )
{

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