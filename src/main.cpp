#include <Arduino.h>
#include <string>
#include <SPI.h>
#include "Wire.h"
#include <quaternionFilters.h>
#include <MPU9250.h>
#include <../lib/atoms/include/atoms/communication/avakar.h>

using namespace atoms;
#define SerialDebug true
#define AHRS false
int myLed  = 13;
#define SPI_CLOCK 8000000  // 8MHz clock works.

//const int SS_PIN = 21;

//const int SERIAL_LED = 15;
//const int SYNC_LED = 14;

HardwareSerial SerialBLE( 1 );
//MPU9250 mpu( SPI_CLOCK, 21 );
MPU9250 mpu;

void send(AvakarPacket packet)
{
	for ( int i = 0; i != packet.raw_size(); i++ ) {
		Serial.write( packet.raw () [ i ] );
		SerialBLE.write( packet.raw () [ i ] );
	}
}

void setup() {
	initArduino();

	Serial.begin( 115200 );
	SerialBLE.begin( 115200, SERIAL_8N1, 2, 4 );

//	SPI.begin( 16, 18, 17, 21 );

	Wire.begin( 21, 22, 400000 );


//	byte wai = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
//	if ( wai == 0x71 ){
//		Serial.println(  "Successful connection" );
//	} else {
//		Serial.println( "Failed connection" );
//	}

	byte c = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
	Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
	Serial.print(" I should be "); Serial.println(0x71, HEX);

	mpu.initMPU9250();

	byte d = mpu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
	Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
	Serial.print(" I should be "); Serial.println(0x48, HEX);


	mpu.initAK8963(mpu.magCalibration);
	Serial.print("X-Axis sensitivity adjustment value ");
	Serial.println(mpu.magCalibration[0], 2);
	Serial.print("Y-Axis sensitivity adjustment value ");
	Serial.println(mpu.magCalibration[1], 2);
	Serial.print("Z-Axis sensitivity adjustment value ");
	Serial.println(mpu.magCalibration[2], 2);

//	uint8_t wai_AK8963 = mpu.AK8963_whoami();
//	if ( wai_AK8963 == 0x48 ){
//		Serial.println("Successful connection to mag");
//	} else {
//		Serial.println(wai_AK8963);
//		Serial.println("---");
//		Serial.println("0x48");
//		Serial.print("Failed connection to mag");
//	}
//
//	mpu.calib_acc();
//	mpu.calib_mag();
//
//	Serial.print("Press any button to begin!");
//	while (!Serial.available() || !SerialBLE.available()) {
//
//	}
}

void loop(){

//	mpu.read_gyro();
//	mpu.read_acc();
//
//	AvakarPacket gyro;
//	gyro.set_command( 1 );
//	gyro.push< float >( mpu.gyro_data [ 0 ] );
//	gyro.push< float >( mpu.gyro_data [ 1 ] );
//	gyro.push< float >( mpu.gyro_data [ 2 ] );
//
//	send( gyro );
//
//	AvakarPacket acc;
//	acc.set_command( 2 );
//	acc.push< float >( mpu.accel_data [ 0 ] );
//	acc.push< float >( mpu.accel_data [ 1 ] );
//	acc.push< float >( mpu.accel_data [ 2 ] );
//
//	send( acc );
//
//	AvakarPacket mag;
//	mag.set_command( 3 );
//	mag.push< float >( mpu.mag_data [ 0 ] );
//	mag.push< float >( mpu.mag_data [ 1 ] );
//	mag.push< float >( mpu.mag_data [ 2 ] );
//
//	send( mag );
//
//	AvakarPacket data;
//	data.set_command( 4 );
//
//	data.push< float >( mpu.temperature );
//
//	send( data );
//
//
//	delay(100);
	if (mpu.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
	{
		mpu.readAccelData(mpu.accelCount);  // Read the x/y/z adc values
		mpu.getAres();

		// Now we'll calculate the accleration value into actual g's
		// This depends on scale being set
		mpu.ax = (float)mpu.accelCount[0]*mpu.aRes; // - accelBias[0];
		mpu.ay = (float)mpu.accelCount[1]*mpu.aRes; // - accelBias[1];
		mpu.az = (float)mpu.accelCount[2]*mpu.aRes; // - accelBias[2];

		mpu.readGyroData(mpu.gyroCount);  // Read the x/y/z adc values
		mpu.getGres();

		// Calculate the gyro value into actual degrees per second
		// This depends on scale being set
		mpu.gx = (float)mpu.gyroCount[0]*mpu.gRes;
		mpu.gy = (float)mpu.gyroCount[1]*mpu.gRes;
		mpu.gz = (float)mpu.gyroCount[2]*mpu.gRes;

		mpu.readMagData(mpu.magCount);  // Read the x/y/z adc values
		mpu.getMres();
		// User environmental x-axis correction in milliGauss, should be
		// automatically calculated
		mpu.magbias[0] = +470.;
		// User environmental x-axis correction in milliGauss TODO axis??
		mpu.magbias[1] = +120.;
		// User environmental x-axis correction in milliGauss
		mpu.magbias[2] = +125.;

		// Calculate the magnetometer values in milliGauss
		// Include factory calibration per data sheet and user environmental
		// corrections
		// Get actual magnetometer value, this depends on scale being set
		mpu.mx = (float)mpu.magCount[0]*mpu.mRes*mpu.magCalibration[0] -
		           mpu.magbias[0];
		mpu.my = (float)mpu.magCount[1]*mpu.mRes*mpu.magCalibration[1] -
		           mpu.magbias[1];
		mpu.mz = (float)mpu.magCount[2]*mpu.mRes*mpu.magCalibration[2] -
		           mpu.magbias[2];
	} // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

	// Must be called before updating quaternions!
	mpu.updateTime();

	// Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
	// the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
	// (+ up) of accelerometer and gyro! We have to make some allowance for this
	// orientationmismatch in feeding the output to the quaternion filter. For the
	// MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
	// along the x-axis just like in the LSM9DS0 sensor. This rotation can be
	// modified to allow any convenient orientation convention. This is ok by
	// aircraft orientation standards! Pass gyro rate as rad/s
//  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
	MahonyQuaternionUpdate(mpu.ax, mpu.ay, mpu.az, mpu.gx*DEG_TO_RAD,
	                       mpu.gy*DEG_TO_RAD, mpu.gz*DEG_TO_RAD, mpu.my,
	                       mpu.mx, mpu.mz, mpu.deltat);

	if (!AHRS)
	{
		mpu.delt_t = millis() - mpu.count;
		if (mpu.delt_t > 500)
		{
			if(SerialDebug)
			{
				// Print acceleration values in milligs!
				Serial.print("X-acceleration: "); Serial.print(1000*mpu.ax);
				Serial.print(" mg ");
				Serial.print("Y-acceleration: "); Serial.print(1000*mpu.ay);
				Serial.print(" mg ");
				Serial.print("Z-acceleration: "); Serial.print(1000*mpu.az);
				Serial.println(" mg ");

				// Print gyro values in degree/sec
				Serial.print("X-gyro rate: "); Serial.print(mpu.gx, 3);
				Serial.print(" degrees/sec ");
				Serial.print("Y-gyro rate: "); Serial.print(mpu.gy, 3);
				Serial.print(" degrees/sec ");
				Serial.print("Z-gyro rate: "); Serial.print(mpu.gz, 3);
				Serial.println(" degrees/sec");

				// Print mag values in degree/sec
				Serial.print("X-mag field: "); Serial.print(mpu.mx);
				Serial.print(" mG ");
				Serial.print("Y-mag field: "); Serial.print(mpu.my);
				Serial.print(" mG ");
				Serial.print("Z-mag field: "); Serial.print(mpu.mz);
				Serial.println(" mG");

				mpu.tempCount = mpu.readTempData();  // Read the adc values
				// Temperature in degrees Centigrade
				mpu.temperature = ((float) mpu.tempCount) / 333.87 + 21.0;
				// Print temperature in degrees Centigrade
				Serial.print("Temperature is ");  Serial.print(mpu.temperature, 1);
				Serial.println(" degrees C");
			}

#ifdef LCD
			display.clearDisplay();
      display.setCursor(0, 0); display.print("MPU9250/AK8963");
      display.setCursor(0, 8); display.print(" x   y   z  ");

      display.setCursor(0,  16); display.print((int)(1000*mpu.ax));
      display.setCursor(24, 16); display.print((int)(1000*mpu.ay));
      display.setCursor(48, 16); display.print((int)(1000*mpu.az));
      display.setCursor(72, 16); display.print("mg");

      display.setCursor(0,  24); display.print((int)(mpu.gx));
      display.setCursor(24, 24); display.print((int)(mpu.gy));
      display.setCursor(48, 24); display.print((int)(mpu.gz));
      display.setCursor(66, 24); display.print("o/s");

      display.setCursor(0,  32); display.print((int)(mpu.mx));
      display.setCursor(24, 32); display.print((int)(mpu.my));
      display.setCursor(48, 32); display.print((int)(mpu.mz));
      display.setCursor(72, 32); display.print("mG");

      display.setCursor(0,  40); display.print("Gyro T ");
      display.setCursor(50,  40); display.print(mpu.temperature, 1);
      display.print(" C");
      display.display();
#endif // LCD

			mpu.count = millis();
			digitalWrite(myLed, !digitalRead(myLed));  // toggle led
		} // if (mpu.delt_t > 500)
	} // if (!AHRS)
	else
	{
		// Serial print and/or display at 0.5 s rate independent of data rates
		mpu.delt_t = millis() - mpu.count;

		// update LCD once per half-second independent of read rate
		if (mpu.delt_t > 500)
		{
			if(SerialDebug)
			{
				Serial.print("ax = "); Serial.print((int)1000*mpu.ax);
				Serial.print(" ay = "); Serial.print((int)1000*mpu.ay);
				Serial.print(" az = "); Serial.print((int)1000*mpu.az);
				Serial.println(" mg");

				Serial.print("gx = "); Serial.print( mpu.gx, 2);
				Serial.print(" gy = "); Serial.print( mpu.gy, 2);
				Serial.print(" gz = "); Serial.print( mpu.gz, 2);
				Serial.println(" deg/s");

				Serial.print("mx = "); Serial.print( (int)mpu.mx );
				Serial.print(" my = "); Serial.print( (int)mpu.my );
				Serial.print(" mz = "); Serial.print( (int)mpu.mz );
				Serial.println(" mG");

				Serial.print("q0 = "); Serial.print(*getQ());
				Serial.print(" qx = "); Serial.print(*(getQ() + 1));
				Serial.print(" qy = "); Serial.print(*(getQ() + 2));
				Serial.print(" qz = "); Serial.println(*(getQ() + 3));
			}

// Define output variables from updated quaternion---these are Tait-Bryan
// angles, commonly used in aircraft orientation. In this coordinate system,
// the positive z-axis is down toward Earth. Yaw is the angle between Sensor
// x-axis and Earth magnetic North (or true North if corrected for local
// declination, looking down on the sensor positive yaw is counterclockwise.
// Pitch is angle between sensor x-axis and Earth ground plane, toward the
// Earth is positive, up toward the sky is negative. Roll is angle between
// sensor y-axis and Earth ground plane, y-axis up is positive roll. These
// arise from the definition of the homogeneous rotation matrix constructed
// from quaternions. Tait-Bryan angles as well as Euler angles are
// non-commutative; that is, the get the correct orientation the rotations
// must be applied in the correct order which for this configuration is yaw,
// pitch, and then roll.
// For more see
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// which has additional links.
			mpu.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ() *
			                                                        *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1) * *(getQ()+1)
			                                                                      - *(getQ()+2) * *(getQ()+2) - *(getQ()+3) * *(getQ()+3));
			mpu.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ() *
			                                                        *(getQ()+2)));
			mpu.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2) *
			                                                    *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1) * *(getQ()+1)
			                                                                  - *(getQ()+2) * *(getQ()+2) + *(getQ()+3) * *(getQ()+3));
			mpu.pitch *= RAD_TO_DEG;
			mpu.yaw   *= RAD_TO_DEG;
			// Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
			// 	8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
			// - http://www.ngdc.noaa.gov/geomag-web/#declination
			mpu.yaw   -= 8.5;
			mpu.roll  *= RAD_TO_DEG;

			if(SerialDebug)
			{
				Serial.print("Yaw, Pitch, Roll: ");
				Serial.print(mpu.yaw, 2);
				Serial.print(", ");
				Serial.print(mpu.pitch, 2);
				Serial.print(", ");
				Serial.println(mpu.roll, 2);

				Serial.print("rate = ");
				Serial.print((float)mpu.sumCount/mpu.sum, 2);
				Serial.println(" Hz");
			}

#ifdef LCD
			display.clearDisplay();

      display.setCursor(0, 0); display.print(" x   y   z  ");

      display.setCursor(0,  8); display.print((int)(1000*mpu.ax));
      display.setCursor(24, 8); display.print((int)(1000*mpu.ay));
      display.setCursor(48, 8); display.print((int)(1000*mpu.az));
      display.setCursor(72, 8); display.print("mg");

      display.setCursor(0,  16); display.print((int)(mpu.gx));
      display.setCursor(24, 16); display.print((int)(mpu.gy));
      display.setCursor(48, 16); display.print((int)(mpu.gz));
      display.setCursor(66, 16); display.print("o/s");

      display.setCursor(0,  24); display.print((int)(mpu.mx));
      display.setCursor(24, 24); display.print((int)(mpu.my));
      display.setCursor(48, 24); display.print((int)(mpu.mz));
      display.setCursor(72, 24); display.print("mG");

      display.setCursor(0,  32); display.print((int)(mpu.yaw));
      display.setCursor(24, 32); display.print((int)(mpu.pitch));
      display.setCursor(48, 32); display.print((int)(mpu.roll));
      display.setCursor(66, 32); display.print("ypr");

    // With these settings the filter is updating at a ~145 Hz rate using the
    // Madgwick scheme and >200 Hz using the Mahony scheme even though the
    // display refreshes at only 2 Hz. The filter update rate is determined
    // mostly by the mathematical steps in the respective algorithms, the
    // processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
    // an ODR of 10 Hz for the magnetometer produce the above rates, maximum
    // magnetometer ODR of 100 Hz produces filter update rates of 36 - 145 and
    // ~38 Hz for the Madgwick and Mahony schemes, respectively. This is
    // presumably because the magnetometer read takes longer than the gyro or
    // accelerometer reads. This filter update rate should be fast enough to
    // maintain accurate platform orientation for stabilization control of a
    // fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
    // produced by the on-board Digital Motion Processor of Invensense's MPU6050
    // 6 DoF and MPU9150 9DoF sensors. The 3.3 V 8 MHz Pro Mini is doing pretty
    // well!
      display.setCursor(0, 40); display.print("rt: ");
      display.print((float) mpu.sumCount / mpu.sum, 2);
      display.print(" Hz");
      display.display();
#endif // LCD

			mpu.count = millis();
			mpu.sumCount = 0;
			mpu.sum = 0;
		} // if (mpu.delt_t > 500)
	} // if (AHRS)

}
