//
// Created by Martin Miksik on 22/12/2017.
//

#include <sstream>
#include "Common.h"


void Common::readValues( bool notify )
{
	_pressure = _bmp.readPressure();
	_temperature = _bmp.readTemperature();

	if ( notify ) {
		this->notify();
	}
}


void Common::notify( )
{
	std::stringstream ss;

	ss << _pressure;
	_storage.addLoad( Storage::BMP_PRESSURE, ss.str() );
	_pressureCharacteristic->setValue( ss.str() );

	ss.flush();
	ss << _temperature;
	_storage.addLoad( Storage::BMP_TEMPERATURE, ss.str() );
	_temperatureCharacteristic->setValue( ss.str() );

	_pressureCharacteristic->notify();
	_temperatureCharacteristic->notify();
}


float Common::getValue( Common::Value value )
{
	switch ( value ) {
		case Value::PRESSURE: {
			return _pressure;
			break;
		}
		case Value::TEMPERATURE: {
			return _temperature;
			break;
		}
	}
}

