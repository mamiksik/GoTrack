//
// Created by Martin Miksik on 24/12/2017.
//

#include "Gyroscope.h"
#include <sstream>


void Gyroscope::readValues( bool notify )
{
	_mpu.readGyroData( _mpu.gyroCount );  // Read the x/y/z adc values
	_mpu.getGres();

	// Calculate the gyro value into actual degrees per second
	// This depends on scale being set
	_mpu.gx = ( float ) _mpu.gyroCount[ 0 ] * _mpu.gRes;
	_mpu.gy = ( float ) _mpu.gyroCount[ 1 ] * _mpu.gRes;
	_mpu.gz = ( float ) _mpu.gyroCount[ 2 ] * _mpu.gRes;

	if ( notify ) {
		this->notify();
	}
}


void Gyroscope::notify( )
{
	std::stringstream ss;

	ss << _mpu.gx;
	_storage.addLoad(Storage::GYRO_X, ss.str());
	_xCharacteristic->setValue(ss.str());

	ss.flush();
	ss << _mpu.gy;
	_storage.addLoad(Storage::GYRO_Y, ss.str());
	_yCharacteristic->setValue(ss.str());

	ss.flush();
	ss << _mpu.gz;
	_storage.addLoad(Storage::GYRO_Z, ss.str());
	_zCharacteristic->setValue(ss.str());

	_xCharacteristic->notify();
	_yCharacteristic->notify();
	_zCharacteristic->notify();
	_storage.flush();
}


float Gyroscope::getValue( MPU::Axes axe )
{
	switch ( axe ) {
		default:
		case Axes::X:
			return _mpu.gx;
			break;
		case Axes::Y:
			return _mpu.gy;
			break;
		case Axes::Z:
			return _mpu.gz;
			break;
	}
}