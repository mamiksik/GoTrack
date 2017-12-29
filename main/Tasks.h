#pragma once


#include <Gyroscope.h>
#include <Accelerometer.h>
#include <Common.h>
#include <SSD1306.h>
//#include <OLEDDisplayUi.h>
#include <Frames.h>
#include <examples/SSD1306ClockDemo/images.h>

#define BTN_PIN 32

class Tasks
{
private:
	Tasks( Gyroscope & gyroscope, Accelerometer & accelerometer, Common & common,
	       OLEDDisplayUi & uiO, DisplayContext & displayContext, SSD1306 & display/*, Logger & logger */) :
			_gyroscope( gyroscope ),
			_accelerometer( accelerometer ),
			_common( common ),
			_ui( uiO ),
			_displayContext( displayContext ),
			_display( display )/*,
            _logger( logger )*/
	{
	}


public:
	static void
	createInstance( Gyroscope & gyroscope, Accelerometer & accelerometer, Common & common, OLEDDisplayUi & ui,
	                DisplayContext & displayContext, SSD1306 & display/*, Logger& logger*/);


	static void destroyInstance( );

	static Tasks *getInstance( );

	static void staticUpdate( void *pvParameter );


	static void staticUiUpdate( void *pvParameter );

	static void BLEHandler(bool connected){
		ESP_LOGI("BLEHandler", "Connected: %b", connected);
		Tasks *p = getInstance();
		if (p->_mutex.try_lock()) {
			p->_displayContext.BLEConnected = connected;
			p->_displayContext.newDataMPU = true;
			p->_mutex.unlock();
		} else {
			delay(10);
			BLEHandler(connected);
		}
	}


	void update( );


	void uiUpdate( );


	Gyroscope & _gyroscope;
	Accelerometer & _accelerometer;
	Common & _common;

	OLEDDisplayUi & _ui;
	DisplayContext & _displayContext;
	SSD1306 & _display;

/*	Logger& _logger;*/

	mutable std::mutex _mutex;

	static Tasks *_instance;

};


