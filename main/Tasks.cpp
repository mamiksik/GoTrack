//
// Created by Martin Miksik on 26/12/2017.
//

#include "Tasks.h"

Tasks *Tasks::_instance = nullptr;


void Tasks::createInstance( Gyroscope & gyroscope, Accelerometer & accelerometer, Common & common, OLEDDisplayUi & ui,
                            DisplayContext & displayContext, SSD1306 & display )
{
	assert( !Tasks::_instance );
	Tasks::_instance = new Tasks( gyroscope, accelerometer, common, ui, displayContext, display/*, logger*/);
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
	while ( true ) {
		if ( _mutex.try_lock() ) {

			_accelerometer.readValues( true );
			_gyroscope.readValues( true );

			Wire.reset();
			_common.readValues( true );
			Wire.reset();

			_mutex.unlock();
		} else {
			ESP_LOGI( "update", "Locked Update" );
		}
		delay( 50 );
	}
}


void Tasks::uiUpdate( )
{
	if ( _mutex.try_lock() ) {
		_ui.setTargetFPS( 25 );
		_ui.setActiveSymbol( activeSymbol );
		_ui.setInactiveSymbol( inactiveSymbol );
		_ui.setIndicatorPosition( BOTTOM );
		_ui.setIndicatorDirection( LEFT_RIGHT );
		_ui.setFrameAnimation( SLIDE_LEFT );


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
		_mutex.unlock();

	} else {
		uiUpdate();
	}

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

//			_displayContext.magX = _ma.getValue( Accelerometer::Z );;
//			_displayContext.magY = mpu.my;
//			_displayContext.magZ = mpu.mz;

			_displayContext.temp = _common.getValue( Common::TEMPERATURE );
			_displayContext.pressure = _common.getValue( Common::PRESSURE );

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

