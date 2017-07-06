#include <Arduino.h>
#include <string>
#include <Wire.h>
#include <MPU9250.h>
#include <../lib/atoms/include/atoms/communication/avakar.h>
#include <OLEDDisplayUi.h>
#include <SSD1306.h>
#include "Symbols.cpp"

using namespace atoms;

const uint8_t SERIAL_PINS[2] = { 16, 4 };
const uint8_t WIRE_PINS[2] = { 21, 22 };

const int SERIAL_LED = 15;
const int BTN = 25;

HardwareSerial SerialBLE( 2 );
MPU9250 mpu;

SSD1306 display( 0x3c, WIRE_PINS[ 0 ], WIRE_PINS[ 1 ] );
OLEDDisplayUi ui( & display );

struct DisplayContext
{
	bool newDataMPU;

	float gyroX;
	float gyroY;
	float gyroZ;

	float accX;
	float accY;
	float accZ;

	float magX;
	float magY;
	float magZ;

	float  temp;
};
DisplayContext displayContext;


void msOverlay( OLEDDisplay *display, OLEDDisplayUiState *state )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_RIGHT );
	display->setFont( ArialMT_Plain_10 );

	String str = "";
	if (context->newDataMPU) {
		str = "yes";
	} else {
		str = " no";
	}

	display->drawString( 128, 0, "Serial out: " + str );
}



void introFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_24 );
	display->drawString( x + 20, y + 16, "GoTrack" );
	display->setFont( ArialMT_Plain_10 );
	display->drawString(x + 20, y + 40, "Martin Miksik" );

}
void gyroFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String gx = "Gyro x: " + String( context->gyroX );
	display->drawString(x + 32, 16 + y, gx );

	String gy = "Gyro y: " + String( context->gyroY );
	display->drawString(x + 32, 26 + y, gy );

	String gz = "Gyro z: " + String( context->gyroZ );
	display->drawString(x + 32, 36 + y, gz );
}


void accFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String ax = "Acc x: " + String( context->accX );
	display->drawString(x + 32, 16 + y, ax );

	String ay = "Acc y: " + String( context->accY );
	display->drawString(x + 32, 26 + y, ay );

	String az = "Acc z: " + String( context->accZ );
	display->drawString(x + 32, 36 + y, az );
}

void magFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String mx = "Mag x: " + String( context->magX );
	display->drawString(x + 32, 16 + y, mx );

	String my = "Mag y: " + String( context->magY );
	display->drawString(x + 32, 26 + y, my );

	String mz = "Mag z: " + String( context->magZ );
	display->drawString(x + 32, 36 + y, mz );
}

void dataFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String temp = "Temp: " + String( context->temp );
	display->drawString(x + 32, 26 + y, temp );
}

FrameCallback frames[] = { introFrame, gyroFrame, accFrame, magFrame, dataFrame };

OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

// how many frames are there?
int frameCount = 5;


void send( AvakarPacket packet )
{
	for ( int i = 0; i != packet.raw_size( ); i++ ) {
		Serial.write( packet.raw( )[ i ] );
		SerialBLE.write( packet.raw( )[ i ] );
	}
}


void setup( )
{
	initArduino( );

	pinMode( SERIAL_LED, OUTPUT );
	pinMode( BTN, INPUT_PULLDOWN );

	Serial.begin( 115200 );
	SerialBLE.begin( 115200, SERIAL_8N1, SERIAL_PINS[ 0 ], SERIAL_PINS[ 1 ] );

	Wire.begin( WIRE_PINS[ 0 ], WIRE_PINS[ 1 ], 400000 );

	ui.setTargetFPS( 23 );

	// Customize the active and inactive symbol
	ui.setActiveSymbol( activeSymbol );
	ui.setInactiveSymbol( inactiveSymbol );

	// You can change this to
	// TOP, LEFT, BOTTOM, RIGHT
	ui.setIndicatorPosition( LEFT );
//	ui.getUiState( )->userData =;

	// Defines where the first frame is located in the bar.
	ui.setIndicatorDirection( LEFT_RIGHT );

	// You can change the transition that is used
	// SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
	ui.setFrameAnimation( SLIDE_UP );

	// Add frames
	ui.setFrames( frames, frameCount );

	// Add overlays
	ui.setOverlays( overlays, overlaysCount );

//	ui.setTimePerFrame(1000);

	// Initialising the UI will init the display too.
	ui.init( );

	display.flipScreenVertically( );

	ui.getUiState( )->userData = & displayContext;

	ui.disableAutoTransition( );
//	display.setFont( ArialMT_Plain_10 );
//	display.drawString( 0, 0, "Hello world" );
//	display.setFont( ArialMT_Plain_16 );
//	display.drawString( 0, 10, "Hello world" );
//	display.setFont( ArialMT_Plain_24 );
//	display.display( );


//	display.firstPage ();
//	display.setFont(u8g_font_unifont);
//	display.print ("Hello world!");
//	delay( 3000 );
//	exit(1);
	byte c = mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
	if ( c == 0x71 ) {
		Serial.println( "Successful connection to MPU9250" );
	} else {
		Serial.println( "Failed connection to MPU9250" );
		exit( 1 );
	}

	// Start by performing self test and reporting values
	mpu.MPU9250SelfTest( mpu.SelfTest );
	Serial.print( "x-axis self test: acceleration trim within : " );
	Serial.print( mpu.SelfTest[ 0 ], 1 );
	Serial.println( "% of factory value" );
	Serial.print( "y-axis self test: acceleration trim within : " );
	Serial.print( mpu.SelfTest[ 1 ], 1 );
	Serial.println( "% of factory value" );
	Serial.print( "z-axis self test: acceleration trim within : " );
	Serial.print( mpu.SelfTest[ 2 ], 1 );
	Serial.println( "% of factory value" );
	Serial.print( "x-axis self test: gyration trim within : " );
	Serial.print( mpu.SelfTest[ 3 ], 1 );
	Serial.println( "% of factory value" );
	Serial.print( "y-axis self test: gyration trim within : " );
	Serial.print( mpu.SelfTest[ 4 ], 1 );
	Serial.println( "% of factory value" );
	Serial.print( "z-axis self test: gyration trim within : " );
	Serial.print( mpu.SelfTest[ 5 ], 1 );
	Serial.println( "% of factory value" );

	// Calibrate gyro and accelerometers, load biases in bias registers
	mpu.calibrateMPU9250( mpu.gyroBias, mpu.accelBias );

	mpu.initMPU9250( );

	byte d = mpu.readByte( AK8963_ADDRESS, WHO_AM_I_AK8963 );
	if ( d == 0x48 ) {
		Serial.println( "Successful connection to AK8963" );
	} else {
		Serial.println( "Failed connection to AK8963" );
		exit( 1 );
	}


	mpu.initAK8963( mpu.magCalibration );
	Serial.print( "X-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 0 ], 2 );
	Serial.print( "Y-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 1 ], 2 );
	Serial.print( "Z-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 2 ], 2 );

	displayContext.newDataMPU = false;

}


void loop( )
{
	int remainingTimeBudget = ui.update( );

	if ( mpu.readByte( MPU9250_ADDRESS, INT_STATUS ) & 0x01 ) {
		displayContext.newDataMPU = true;
		Serial.println( "New values" );
		mpu.readAccelData( mpu.accelCount );  // Read the x/y/z adc values
		mpu.getAres( );

		// Now we'll calculate the accleration value into actual g's
		// This depends on scale being set
		mpu.ax = ( float ) mpu.accelCount[ 0 ] * mpu.aRes; // - accelBias[0];
		mpu.ay = ( float ) mpu.accelCount[ 1 ] * mpu.aRes; // - accelBias[1];
		mpu.az = ( float ) mpu.accelCount[ 2 ] * mpu.aRes; // - accelBias[2];

		mpu.readGyroData( mpu.gyroCount );  // Read the x/y/z adc values
		mpu.getGres( );

		// Calculate the gyro value into actual degrees per second
		// This depends on scale being set
		mpu.gx = ( float ) mpu.gyroCount[ 0 ] * mpu.gRes;
		mpu.gy = ( float ) mpu.gyroCount[ 1 ] * mpu.gRes;
		mpu.gz = ( float ) mpu.gyroCount[ 2 ] * mpu.gRes;

		mpu.readMagData( mpu.magCount );  // Read the x/y/z adc values
		mpu.getMres( );

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

		Serial.println( "Finish read new values" );

	}
	// Must be called before updating quaternions!
	mpu.updateTime( );

	mpu.delt_t = millis( ) - mpu.count;
	if ( mpu.delt_t > 100 ) {
		Serial.println( "Send values" );

		AvakarPacket acc;
		acc.set_command( 1 );
		acc.push < float >( 1000 * mpu.ax ); // mg
		acc.push < float >( 1000 * mpu.ay );
		acc.push < float >( 1000 * mpu.az );

		send( acc );

		AvakarPacket gyro;
		gyro.set_command( 2 );
		gyro.push < float >( mpu.gx ); // deg/s
		gyro.push < float >( mpu.gy );
		gyro.push < float >( mpu.gz );

		send( gyro );

		AvakarPacket mag;
		mag.set_command( 3 );
		mag.push < float >( mpu.mx ); // mG
		mag.push < float >( mpu.my );
		mag.push < float >( mpu.mz );

		send( mag );


		mpu.tempCount = mpu.readTempData( );
		mpu.temperature = ( ( float ) mpu.tempCount ) / 333.87 + 21.0;


		AvakarPacket data;
		data.set_command( 6 );
		data.push < float >( ( float ) mpu.sumCount / mpu.sum ); //Hz
		data.push < float >( mpu.temperature );

		send( data );

		mpu.count = millis( );
		digitalWrite( SERIAL_LED, !digitalRead( SERIAL_LED ) );  // toggle led
		displayContext.newDataMPU = !displayContext.newDataMPU;

		Serial.println( "Finish send values" );

		displayContext.gyroX = mpu.gx;
		displayContext.gyroY = mpu.gy;
		displayContext.gyroZ = mpu.gz;

		displayContext.accX = mpu.ax;
		displayContext.accY = mpu.ay;
		displayContext.accZ = mpu.az;

		displayContext.magX = mpu.mx;
		displayContext.magY = mpu.my;
		displayContext.magZ = mpu.mz;

		displayContext.temp = mpu.temperature;
	}

	if ( digitalRead( BTN ) == 1 ) {
//		ui.setTargetFPS( 23 );
		ui.nextFrame( );
//		ui.setTargetFPS( 11 );
	}
}
