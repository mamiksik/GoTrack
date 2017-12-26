//
// Created by Martin Miksik on 21/12/2017.
//

#include "Magnetometer.h"
#include <sstream>


void Magnetometer::readValues( bool notify )
{
	_mpu.readMagData( _mpu.magCount );  // Read the x/y/z adc values
	_mpu.getMres();

	// User environmental x-axis correction in milliGauss, should be
	// automatically calculated
	_mpu.magbias[ 0 ] = +470.;
	// User environmental x-axis correction in milliGauss TODO axis??
	_mpu.magbias[ 1 ] = +120.;
	// User environmental x-axis correction in milliGauss
	_mpu.magbias[ 2 ] = +125.;

	// Calculate the magnetometer values in milliGauss
	// Include factory calibration per data sheet and user environmental
	// corrections
	// Get actual magnetometer value, this depends on scale being set
	_mpu.mx = ( float ) _mpu.magCount[ 0 ] * _mpu.mRes * _mpu.magCalibration[ 0 ] -
			_mpu.magbias[ 0 ];
	_mpu.my = ( float ) _mpu.magCount[ 1 ] * _mpu.mRes * _mpu.magCalibration[ 1 ] -
			_mpu.magbias[ 1 ];
	_mpu.mz = ( float ) _mpu.magCount[ 2 ] * _mpu.mRes * _mpu.magCalibration[ 2 ] -
			_mpu.magbias[ 2 ];

	if ( notify ) {
		this->notify();
	}
}


void Magnetometer::notify( )
{
	std::stringstream ss;

	ss << _mpu.mx;
	_storage.addLoad( Storage::MAG_X, ss.str() );
	_xCharacteristic->setValue( ss.str() );

	ss.flush();
	ss << _mpu.my;
	_storage.addLoad( Storage::MAG_Y, ss.str() );
	_yCharacteristic->setValue( ss.str() );

	ss.flush();
	ss << _mpu.mz;
	_storage.addLoad( Storage::MAG_Z, ss.str() );
	_zCharacteristic->setValue( ss.str() );

	_xCharacteristic->notify();
	_yCharacteristic->notify();
	_zCharacteristic->notify();
	_storage.flush();
}


float Magnetometer::getValue( Magnetometer::Axes axe )
{
	switch ( axe ) {
		default:
		case Axes::X:
			return _mpu.mx;
			break;
		case Axes::Y:
			return _mpu.my;
			break;
		case Axes::Z:
			return _mpu.mz;
			break;
	}

}
