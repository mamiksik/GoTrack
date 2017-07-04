#include <Arduino.h>
#include <string>
#include <SPI.h>
#include "Wire.h"
#include <quaternionFilters.h>
#include <MPU9250.h>
#include <../lib/atoms/include/atoms/communication/avakar.h>

using namespace atoms;

const int SERIAL_LED = 15;
//const int SYNC_LED = 14;

HardwareSerial SerialBLE ( 1 );
MPU9250 mpu;


void send ( AvakarPacket packet )
{
	for ( int i = 0; i != packet.raw_size ( ); i++ ) {
//		Serial.write ( packet.raw ( )[ i ] );
		SerialBLE.write ( packet.raw ( )[ i ] );
	}
}


void setup ( )
{
	initArduino ( );

	pinMode ( SERIAL_LED, OUTPUT );

	Serial.begin ( 115200 );
	SerialBLE.begin(115200, SERIAL_8N1, 2, 4);

	Wire.begin ( 21, 22, 400000 );

	byte c = mpu.readByte ( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
	if ( c == 0x71 ) {
		Serial.println ( "Successful connection to MPU9250" );
	} else {
		Serial.println ( "Failed connection to MPU9250" );
		exit ( 1 );
	}

	// Start by performing self test and reporting values
	mpu.MPU9250SelfTest ( mpu.SelfTest );
	Serial.print ( "x-axis self test: acceleration trim within : " );
	Serial.print ( mpu.SelfTest[ 0 ], 1 );
	Serial.println ( "% of factory value" );
	Serial.print ( "y-axis self test: acceleration trim within : " );
	Serial.print ( mpu.SelfTest[ 1 ], 1 );
	Serial.println ( "% of factory value" );
	Serial.print ( "z-axis self test: acceleration trim within : " );
	Serial.print ( mpu.SelfTest[ 2 ], 1 );
	Serial.println ( "% of factory value" );
	Serial.print ( "x-axis self test: gyration trim within : " );
	Serial.print ( mpu.SelfTest[ 3 ], 1 );
	Serial.println ( "% of factory value" );
	Serial.print ( "y-axis self test: gyration trim within : " );
	Serial.print ( mpu.SelfTest[ 4 ], 1 );
	Serial.println ( "% of factory value" );
	Serial.print ( "z-axis self test: gyration trim within : " );
	Serial.print ( mpu.SelfTest[ 5 ], 1 );
	Serial.println ( "% of factory value" );

	// Calibrate gyro and accelerometers, load biases in bias registers
	mpu.calibrateMPU9250 ( mpu.gyroBias, mpu.accelBias );

	mpu.initMPU9250 ( );

	byte d = mpu.readByte ( AK8963_ADDRESS, WHO_AM_I_AK8963 );
	if ( d == 0x48 ) {
		Serial.println ( "Successful connection to AK8963" );
	} else {
		Serial.println ( "Failed connection to AK8963" );
		exit ( 1 );
	}


	mpu.initAK8963 ( mpu.magCalibration );
	Serial.print ( "X-Axis sensitivity adjustment value " );
	Serial.println ( mpu.magCalibration[ 0 ], 2 );
	Serial.print ( "Y-Axis sensitivity adjustment value " );
	Serial.println ( mpu.magCalibration[ 1 ], 2 );
	Serial.print ( "Z-Axis sensitivity adjustment value " );
	Serial.println ( mpu.magCalibration[ 2 ], 2 );

//	Serial.print("Press any button to begin!");
//	while (!Serial.available() || !SerialBLE.available()) {
//
//	}

}


void loop ( )
{
//	SerialBLE.println ("TEst");
//	Serial.println ("TEst");
//	delay (100);
//	return;
//	Serial.println (mpu.readByte ( MPU9250_ADDRESS, INT_STATUS ) & 0x01 );
	if ( mpu.readByte ( MPU9250_ADDRESS, INT_STATUS ) & 0x01 ) {
		mpu.readAccelData ( mpu.accelCount );  // Read the x/y/z adc values
		mpu.getAres ( );

		// Now we'll calculate the accleration value into actual g's
		// This depends on scale being set
		mpu.ax = ( float ) mpu.accelCount[ 0 ] * mpu.aRes; // - accelBias[0];
		mpu.ay = ( float ) mpu.accelCount[ 1 ] * mpu.aRes; // - accelBias[1];
		mpu.az = ( float ) mpu.accelCount[ 2 ] * mpu.aRes; // - accelBias[2];

		mpu.readGyroData ( mpu.gyroCount );  // Read the x/y/z adc values
		mpu.getGres ( );

		// Calculate the gyro value into actual degrees per second
		// This depends on scale being set
		mpu.gx = ( float ) mpu.gyroCount[ 0 ] * mpu.gRes;
		mpu.gy = ( float ) mpu.gyroCount[ 1 ] * mpu.gRes;
		mpu.gz = ( float ) mpu.gyroCount[ 2 ] * mpu.gRes;

		mpu.readMagData ( mpu.magCount );  // Read the x/y/z adc values
		mpu.getMres ( );

		// User environmental x-axis correction in milliGauss, should be
		// automatically calculated
		mpu.magbias[ 0 ] = +470.;
		// User environmental x-axis correction in milliGauss TODO axis??
		mpu.magbias[ 1 ] = +120.;
		// User environmental x-axis correction in milliGauss
		mpu.magbias[ 2 ] = +125.;

		// Calculate the magnetometer values in milliGauss
		// Include factory calibration per data sheet and user environmental
		// corrections
		// Get actual magnetometer value, this depends on scale being set
		mpu.mx = ( float ) mpu.magCount[ 0 ] * mpu.mRes * mpu.magCalibration[ 0 ] -
		         mpu.magbias[ 0 ];
		mpu.my = ( float ) mpu.magCount[ 1 ] * mpu.mRes * mpu.magCalibration[ 1 ] -
		         mpu.magbias[ 1 ];
		mpu.mz = ( float ) mpu.magCount[ 2 ] * mpu.mRes * mpu.magCalibration[ 2 ] -
		         mpu.magbias[ 2 ];

		Serial.println ( "test2" );
	}
	// Must be called before updating quaternions!
	mpu.updateTime ( );

	// Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
	// the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
	// (+ up) of accelerometer and gyro! We have to make some allowance for this
	// orientationmismatch in feeding the output to the quaternion filter. For the
	// MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
	// along the x-axis just like in the LSM9DS0 sensor. This rotation can be
	// modified to allow any convenient orientation convention. This is ok by
	// aircraft orientation standards! Pass gyro rate as rad/s
	// MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
	MahonyQuaternionUpdate ( mpu.ax, mpu.ay, mpu.az, mpu.gx * DEG_TO_RAD,
	                         mpu.gy * DEG_TO_RAD, mpu.gz * DEG_TO_RAD, mpu.my,
	                         mpu.mx, mpu.mz, mpu.deltat );
	Serial.println ( "test3" );
	SerialBLE.println ( "test3" );
	mpu.yaw = atan2 ( 2.0f * ( * ( getQ ( ) + 1 ) * * ( getQ ( ) + 2 ) + * getQ ( ) *
	                                                                     * ( getQ ( ) + 3 ) ),
	                  * getQ ( ) * * getQ ( ) + * ( getQ ( ) + 1 ) * * ( getQ ( ) + 1 )
	                  - * ( getQ ( ) + 2 ) * * ( getQ ( ) + 2 ) - * ( getQ ( ) + 3 ) * * ( getQ ( ) + 3 ) );
	mpu.pitch = -asin ( 2.0f * ( * ( getQ ( ) + 1 ) * * ( getQ ( ) + 3 ) - * getQ ( ) *
	                                                                       * ( getQ ( ) + 2 ) ) );
	mpu.roll = atan2 ( 2.0f * ( * getQ ( ) * * ( getQ ( ) + 1 ) + * ( getQ ( ) + 2 ) *
	                                                              * ( getQ ( ) + 3 ) ),
	                   * getQ ( ) * * getQ ( ) - * ( getQ ( ) + 1 ) * * ( getQ ( ) + 1 )
	                   - * ( getQ ( ) + 2 ) * * ( getQ ( ) + 2 ) + * ( getQ ( ) + 3 ) * * ( getQ ( ) + 3 ) );
	mpu.pitch *= RAD_TO_DEG;
	mpu.yaw *= RAD_TO_DEG;
	// Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
	// 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
	// - http://www.ngdc.noaa.gov/geomag-web/#declination
	mpu.yaw -= 8.5;
	mpu.roll *= RAD_TO_DEG;

	mpu.delt_t = millis ( ) - mpu.count;
	if ( mpu.delt_t > 100 ) {

		AvakarPacket acc;
		acc.set_command ( 1 );
		acc.push < float > ( 1000 * mpu.ax ); // mg
		acc.push < float > ( 1000 * mpu.ay );
		acc.push < float > ( 1000 * mpu.az );

		send ( acc );

		AvakarPacket gyro;
		gyro.set_command ( 2 );
		gyro.push < float > ( mpu.gx ); // deg/s
		gyro.push < float > ( mpu.gy );
		gyro.push < float > ( mpu.gz );

		send ( gyro );

		AvakarPacket mag;
		mag.set_command ( 3 );
		mag.push < float > ( mpu.mx ); // mG
		mag.push < float > ( mpu.my );
		mag.push < float > ( mpu.mz );

		send ( mag );

		AvakarPacket quaternion;
		quaternion.set_command ( 4 );
		quaternion.push < float > ( * ( getQ ( ) + 1 ) );
		quaternion.push < float > ( * ( getQ ( ) + 2 ) );
		quaternion.push < float > ( * ( getQ ( ) + 3 ) );

		send ( quaternion );

		SerialBLE.print ( "=====2" );
		SerialBLE.print ( mpu.temperature, 1 );
		SerialBLE.print ( "=====" );

		AvakarPacket movments;
		movments.set_command ( 5 );
		movments.push < float > ( mpu.yaw );
		movments.push < float > ( mpu.pitch );
		movments.push < float > ( mpu.roll );

		send ( movments );

		mpu.tempCount = mpu.readTempData();
		mpu.temperature = ( ( float ) mpu.tempCount ) / 333.87 + 21.0;


		AvakarPacket data;
		data.set_command ( 6 );
		data.push < float > ( * getQ ( ) ); //q0
		data.push < float > ( ( float ) mpu.sumCount / mpu.sum ); //Hz
		data.push < float > ( ( float ) mpu.temperature );

		send ( data );

		mpu.count = millis ( );
		digitalWrite (SERIAL_LED, !digitalRead ( SERIAL_LED ) );  // toggle led
	}
//	delay ( 100 );

}
