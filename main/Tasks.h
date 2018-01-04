#pragma once


#include <Gyroscope.h>
#include <Accelerometer.h>
#include <Common.h>
#include <SSD1306.h>
//#include <OLEDDisplayUi.h>
#include <Frames.h>
#include <examples/SSD1306ClockDemo/images.h>
#include <Magnetometer.h>

#define BTN_PIN 32

class Tasks final
{
private:
	Tasks( Gyroscope & gyroscope, Accelerometer & accelerometer, Magnetometer & magnetometer, Common & common,
	       OLEDDisplayUi & uiO, DisplayContext & displayContext, SSD1306 & display/*, Logger & logger */) :
			_gyroscope( gyroscope ),
			_accelerometer( accelerometer ),
			_magnetometer( magnetometer ),
			_common( common ),
			_ui( uiO ),
			_displayContext( displayContext ),
			_display( display )/*,
            _logger( logger )*/
	{
	}


public:
	static void
	createInstance( Gyroscope & gyroscope, Accelerometer & accelerometer, Magnetometer & magnetometer, Common & common,
	                OLEDDisplayUi & ui,
	                DisplayContext & displayContext, SSD1306 & display/*, Logger& logger*/);


	static void destroyInstance( );


	static Tasks *getInstance( );


	static void staticUpdate( void *pvParameter );


	static void staticUiUpdate( void *pvParameter );


	static void BLEHandler( bool connected );


	void update( );


	void uiUpdate( );

	void uiInit( );

private:

	Gyroscope & _gyroscope;
	Accelerometer & _accelerometer;
	Magnetometer & _magnetometer;
	Common & _common;

	OLEDDisplayUi & _ui;
	DisplayContext & _displayContext;
	SSD1306 & _display;

	FrameCallback _frames[5];
	uint8_t _frameCount;

	OverlayCallback _overlays[2];
	uint8_t _overlaysCount;

	bool _session;

/*	Logger& _logger;*/

	mutable std::mutex _mutex;

	static Tasks *_instance;

};


