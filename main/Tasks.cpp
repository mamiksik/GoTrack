//
// Created by Martin Miksik on 26/12/2017.
//

#include "Tasks.h"

Tasks *Tasks::_instance = nullptr;

#define SESSION_BTN 345

void Tasks::createInstance( Gyroscope & gyroscope, Accelerometer & accelerometer, Magnetometer & magnetometer,
                            Common & common, OLEDDisplayUi & ui,
                            DisplayContext & displayContext, SSD1306 & display )
{
	assert( !Tasks::_instance );
	Tasks::_instance = new Tasks( gyroscope, accelerometer, magnetometer, common, ui, displayContext,
	                              display/*, logger*/);
}


void Tasks::destroyInstance( )
{
	assert( Tasks::_instance );
	delete Tasks::_instance;
}


Tasks *Tasks::getInstance( )
{
	return Tasks::_instance;
}


void Tasks::staticUpdate( void *pvParameter )
{
	Tasks::getInstance()->update();
}


void Tasks::staticUiUpdate( void *pvParameter )
{
	Tasks::getInstance()->uiUpdate();
}


void Tasks::update( )
{
	ESP_LOGE( "test", "UPDATE" );
	for ( ;; ) {
		if ( _session && _mutex.try_lock() ) {
			_accelerometer.readValues( true );

			delay(100);
			_gyroscope.readValues( true );

			delay(100);
			_magnetometer.readValues( true );

			delay(100);
			Wire.reset();
			_common.readValues( true );
			Wire.reset();

			_mutex.unlock();
		} else {
			ESP_LOGI( "update", "Locked Update" );
		}
		delay( 100 );
	}
}


void Tasks::uiUpdate( )
{

	uiInit();

	ESP_LOGE( "test", "UI Update" );
	for ( ;; ) {
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

			_displayContext.magX = _magnetometer.getValue( Accelerometer::X );
			_displayContext.magY = _magnetometer.getValue( Accelerometer::Y );
			_displayContext.magZ = _magnetometer.getValue( Accelerometer::Z );

			_displayContext.temp = _common.getValue( Common::TEMPERATURE );
			_displayContext.pressure = _common.getValue( Common::PRESSURE );

			_mutex.unlock();
		} else {
			ESP_LOGI( "update", "Locked UI" );
		}

		if ( digitalRead( BTN_PIN ) == HIGH ) {
			_ui.nextFrame();
		}

		if ( digitalRead(SESSION_BTN)  == HIGH) {

		}
		delay( 100 );
	}
}


void Tasks::BLEHandler( bool connected )
{
	ESP_LOGI( "BLEHandler", "Connected: %b", connected );
	Tasks *p = getInstance();
	if ( p->_mutex.try_lock() ) {
		p->_displayContext.BLEConnected = connected;
		p->_displayContext.newDataMPU = true;
		p->_mutex.unlock();
	} else {
		delay( 10 );
		BLEHandler( connected );
	}
}


void Tasks::uiInit( )
{
	if ( _mutex.try_lock() ) {
		_ui.setTargetFPS( 5 );
		_ui.setActiveSymbol( activeSymbol );
		_ui.setInactiveSymbol( inactiveSymbol );
		_ui.setIndicatorPosition( BOTTOM );
		_ui.setIndicatorDirection( LEFT_RIGHT );
		_ui.setFrameAnimation( SLIDE_LEFT );


		_frames[0] = introFrame;
		_frames[1] = gyroFrame;
		_frames[2] = accFrame;
		_frames[3] = magFrame;
		_frames[4] = dataFrame;

		_frameCount = 5;

		_overlays[0] = bleOverlay;
		_overlays[1] = sessionOverlay;
		_overlaysCount = 2;

		_ui.setFrames( _frames, _frameCount );
		_ui.setOverlays( _overlays, _overlaysCount );
		_ui.init();
		_ui.disableAutoTransition();
		_display.flipScreenVertically();
		_ui.getUiState()->userData = & _displayContext;
		_displayContext.newDataMPU = false;
		_mutex.unlock();

	} else {
		uiInit();
	}
}

