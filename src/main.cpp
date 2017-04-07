#include "Arduino.h"
#include "MPU9250.h"
#include "Wire.h"
#include "quaternionFilters.h"

#define AHRS true
#define SerialDebug true

//Onboard devices
#define BOARD_LED 2
#define BOARD_BUTTON 0

#define MPU9250_ADDRESS 0x69 //0x77 //0x69 AK8963_ADDRESS
//#define AK8963_ADDRESS 0x69

MPU9250 myIMU;

void setup() {
	initArduino();
	Serial.begin(115200 );
	Serial.println("Test Serial");

	//Set up port for MPU (http://platformio.org/lib/show/944/SparkFun%20MPU-9250%209%20DOF%20IMU%20Breakout/examples)
	//begin(sda, scl)
	//Wire.begin(17, 16);
	Wire.begin(17, 16);
	Serial.println("Wire");
	Serial.println(MPU9250_ADDRESS, HEX);
	Serial.println(WHO_AM_I_MPU9250, HEX);


	//myIMU.address
	//myIMU.
	byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
 	Serial.println("MPU9250 ");
	Serial.print("I AM ");
	Serial.print(c, HEX);
 	Serial.print(" I should be ");
	Serial.print(0x71, HEX);


	    Serial.println("MPU9250 is online...");

	    // Start by performing self test and reporting values
	    myIMU.MPU9250SelfTest(myIMU.SelfTest);
	    Serial.print("x-axis self test: acceleration trim within : ");
	    Serial.print(myIMU.SelfTest[0],1); Serial.println("% of factory value");
	    Serial.print("y-axis self test: acceleration trim within : ");
	    Serial.print(myIMU.SelfTest[1],1); Serial.println("% of factory value");
	    Serial.print("z-axis self test: acceleration trim within : ");
	    Serial.print(myIMU.SelfTest[2],1); Serial.println("% of factory value");
	    Serial.print("x-axis self test: gyration trim within : ");
	    Serial.print(myIMU.SelfTest[3],1); Serial.println("% of factory value");
	    Serial.print("y-axis self test: gyration trim within : ");
	    Serial.print(myIMU.SelfTest[4],1); Serial.println("% of factory value");
	    Serial.print("z-axis self test: gyration trim within : ");
	    Serial.print(myIMU.SelfTest[5],1); Serial.println("% of factory value");

	    // Calibrate gyro and accelerometers, load biases in bias registers
	    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

	myIMU.initMPU9250();
   // Initialize device for active mode read of acclerometer, gyroscope, and
   // temperature
   Serial.println("MPU9250 initialized for active data mode....");

   // Read the WHO_AM_I register of the magnetometer, this is a good test of
   // communication
   byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
   Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
   Serial.print(" I should be "); Serial.println(0x48, HEX);

   //myIMU.initAK8963(myIMU.magCalibration);
    // Initialize device for active mode read of magnetometer
    Serial.println("AK8963 initialized for active data mode....");
    if (SerialDebug)
    {
      //  Serial.println("Calibration values: ");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[2], 2);
    }

	delay(4000);

  	pinMode(BOARD_LED, OUTPUT);
	pinMode(BOARD_BUTTON, INPUT_PULLUP);
}

void loop(){
	digitalWrite(BOARD_LED, !digitalRead(BOARD_BUTTON));

	//Serial.println(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS));
//if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
//Serial.println(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS));
delay(1300);
if(myIMU.readByte(MPU9250_ADDRESS, INT_STATUS))
 {
   myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
   myIMU.getAres();

   // Now we'll calculate the accleration value into actual g's
   // This depends on scale being set
   myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes; // - accelBias[0];
   myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes; // - accelBias[1];
   myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes; // - accelBias[2];

   myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
   myIMU.getGres();

   // Calculate the gyro value into actual degrees per second
   // This depends on scale being set
   myIMU.gx = (float)myIMU.gyroCount[0]*myIMU.gRes;
   myIMU.gy = (float)myIMU.gyroCount[1]*myIMU.gRes;
   myIMU.gz = (float)myIMU.gyroCount[2]*myIMU.gRes;

   myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
   myIMU.getMres();
   // User environmental x-axis correction in milliGauss, should be
   // automatically calculated
   myIMU.magbias[0] = +470.;
   // User environmental x-axis correction in milliGauss TODO axis??
   myIMU.magbias[1] = +120.;
   // User environmental x-axis correction in milliGauss
   myIMU.magbias[2] = +125.;

   // Calculate the magnetometer values in milliGauss
   // Include factory calibration per data sheet and user environmental
   // corrections
   // Get actual magnetometer value, this depends on scale being set
   myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
			  myIMU.magbias[0];
   myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
			  myIMU.magbias[1];
   myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
			  myIMU.magbias[2];
 } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

 // Must be called before updating quaternions!
 myIMU.updateTime();

 // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
 // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
 // (+ up) of accelerometer and gyro! We have to make some allowance for this
 // orientationmismatch in feeding the output to the quaternion filter. For the
 // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
 // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
 // modified to allow any convenient orientation convention. This is ok by
 // aircraft orientation standards! Pass gyro rate as rad/s
 //MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
 MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx*DEG_TO_RAD,
 					myIMU.gy*DEG_TO_RAD, myIMU.gz*DEG_TO_RAD, myIMU.my,
 					myIMU.mx, myIMU.mz, myIMU.deltat);


myIMU.delt_t = millis() - myIMU.count;
if (myIMU.delt_t > 500)
{
	// Print acceleration values in milligs!
	// Print acceleration values in milligs!
   Serial.print("X-acceleration: "); Serial.print(1000*myIMU.ax);
   Serial.print(" mg ");
   Serial.print("Y-acceleration: "); Serial.print(1000*myIMU.ay);
   Serial.print(" mg ");
   Serial.print("Z-acceleration: "); Serial.print(1000*myIMU.az);
   Serial.println(" mg ");

   // Print gyro values in degree/sec
   Serial.print("X-gyro rate: "); Serial.print(myIMU.gx, 3);
   Serial.print(" degrees/sec ");
   Serial.print("Y-gyro rate: "); Serial.print(myIMU.gy, 3);
   Serial.print(" degrees/sec ");
   Serial.print("Z-gyro rate: "); Serial.print(myIMU.gz, 3);
   Serial.println(" degrees/sec");

   // Print mag values in degree/sec
   Serial.print("X-mag field: "); Serial.print(myIMU.mx);
   Serial.print(" mG ");
   Serial.print("Y-mag field: "); Serial.print(myIMU.my);
   Serial.print(" mG ");
   Serial.print("Z-mag field: "); Serial.print(myIMU.mz);
   Serial.println(" mG");

   myIMU.tempCount = myIMU.readTempData();  // Read the adc values
   // Temperature in degrees Centigrade
   myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
   // Print temperature in degrees Centigrade
  Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
   Serial.println(" degrees C");
   myIMU.count = millis();
}

	//delay(00);
}

// #include <Wire.h>
// void setup()
// {
//   Wire.begin(17, 16);
//   Serial.begin(115200);
//   Serial.println("14CORE | i2C SCANNER ");
//   Serial.println("=========================");
//   Serial.println("Starting ....");
//   Serial.println("");
// }
//
// void loop()
// {
//   byte error, address;
//   int devices;
//
//   Serial.println("Scanning...");
//
//   devices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
//
//     if (error == 0)
//     {
//       Serial.print("I2C device found at address | 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");
//
//       devices++;
//     }
//     else if (error==4)
//     {
//       Serial.print("Unknow error at address | 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }
//   }
//   if (devices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");
//
//   delay(5000);
// }
