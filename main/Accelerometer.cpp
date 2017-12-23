//
// Created by Martin Miksik on 21/12/2017.
//

#include "Accelerometer.h"
#include <sstream>


void Accelerometer::notify( )
{
	std::stringstream ss;

	ss << _mpu.ax;
	_storage.addLoad(Storage::ACC_X, ss.str());
	_xCharacteristic->setValue(ss.str());

	ss.flush();
	ss << _mpu.ay;
	_storage.addLoad(Storage::ACC_Y, ss.str());
	_yCharacteristic->setValue(ss.str());

	ss.flush();
	ss << _mpu.az;
	_storage.addLoad(Storage::ACC_Z, ss.str());
	_zCharacteristic->setValue(ss.str());

	_xCharacteristic->notify();
	_yCharacteristic->notify();
	_zCharacteristic->notify();
	_storage.flush();
}


void Accelerometer::readValues( bool notify )
{
	_mpu.readAccelData( _mpu.accelCount );

	_mpu.ax = ( float ) _mpu.accelCount[ 0 ] * _mpu.aRes; // - accelBias[0];
	_mpu.ay = ( float ) _mpu.accelCount[ 1 ] * _mpu.aRes; // - accelBias[1];
	_mpu.az = ( float ) _mpu.accelCount[ 2 ] * _mpu.aRes; // - accelBias[2];

	if (notify) {
		this->notify();
	}
}


float  Accelerometer::getValue( Accelerometer::Axes axe )
{
	switch (axe) {
		default:
		case Axes::X:
			return _mpu.ax;
			break;
		case Axes::Y:
			return _mpu.ay;
			break;
		case Axes::Z:
			return _mpu.az;
			break;
	}

}
