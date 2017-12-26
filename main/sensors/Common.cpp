//
// Created by Martin Miksik on 22/12/2017.
//

#include <sstream>
#include "Common.h"

void Common::notify( )
{
	std::stringstream ss;

	ss << _bmp.readPressure();
	_storage.addLoad( Storage::BMP_PRESSURE, ss.str() );
	_pressureCharacteristic->setValue( ss.str() );

	ss.flush();
	ss << _bmp.readTemperature();
	_storage.addLoad( Storage::BMP_TEMPERATURE, ss.str() );
	_temperatureCharacteristic->setValue( ss.str() );

	_pressureCharacteristic->notify();
	_temperatureCharacteristic->notify();
}


float Common::getValue( Common::Value value )
{
	switch (value){
		case Value::PRESSURE:{
			return _bmp.readPressure();
			break;
		}
		case Value::TEMPERATURE:{
			return _bmp.readTemperature();
			break;
		}
	}
}
