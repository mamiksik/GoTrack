#include <Arduino.h>
#include "Settings.h"
#include <string>
#include <Wire.h>
#include <MPU9250.h>
#include <../lib/atoms/include/atoms/communication/avakar.h>
#include <OLEDDisplayUi.h>
#include <SSD1306.h>
#include "Symbols.h"
#include "FS.h"
#include "SD.h"
#include "Finder.h"
#include <vector>
#include <Frames.h>
#include <WiFi.h>
#include <time.h>
#include <SmartLeds.h>
#include <Adafruit_BMP280.h>

using namespace atoms;


HardwareSerial SerialBLE( 2 );
Finder finder = Finder( SD );
File logs;
MPU9250 mpu;
Adafruit_BMP280 bmp;

SmartLed leds( LED_WS2812, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer );

SSD1306 display( 0x3c, WIRE_PINS[ 0 ], WIRE_PINS[ 1 ] );
OLEDDisplayUi ui( & display );
DisplayContext displayContext;
//DS1302 rtc( RT_CLOCK_PINS[ 0 ], RT_CLOCK_PINS[ 1 ], RT_CLOCK_PINS[ 2 ] );
//DS1302 rtc(RT_CLOCK_PINS[0], RT_CLOCK_PINS[2], RT_CLOCK_PINS[1]);

FrameCallback frames[] = { introFrame, gyroFrame, accFrame, magFrame, dataFrame };
const int frameCount = 5;

OverlayCallback overlays[] = { packetOverlay, savedOverlay };
const int overlaysCount = 2;


int yPosition = 0;
int packetCount = 0;

uint8_t hue;
void showGradient() {
	hue++;
	// Use HSV to create nice gradient
	for ( int i = 0; i != LED_COUNT; i++ ) {
		leds[ i ] = Hsv{ static_cast< uint8_t >( hue + 50 * i ), 255, 15 };
	}

	leds.show();
	// Show is asynchronous; if we need to wait for the end of transmission,
	// we can use leds.wait(); however we use double buffered mode, so we
	// can start drawing right after showing.
}

void showRgb() {
	leds[ 0 ] = Rgb{ 255, 0, 0 };
	leds[ 1 ] = Rgb{ 0, 255, 0 };
	leds[ 2 ] = Rgb{ 0, 0, 255 };
	leds[ 3 ] = Rgb{ 0, 0, 0 };
	leds[ 4 ] = Rgb{ 255, 255, 255 };
	leds.show();
}


void send( AvakarPacket packet )
{
	for ( int i = 0; i != packet.raw_size(); i++ ) {
		Serial.write( packet.raw()[ i ] );
		SerialBLE.write( packet.raw()[ i ] );
		logs.write( packet.raw()[ i ] );
	}

	packetCount++;
	if ( packetCount >= FLUSH_AT ) {
		packetCount = 0;
		String name = logs.name();
		logs.flush();
		logs.close();
		logs = SD.open( name, FILE_APPEND );
	}
}


void report( String str )
{
	display.drawString( 0, yPosition, str );
	display.drawString( 140, yPosition, String( yPosition ) );
	yPosition += 10;
	display.display();
	delay( 250 );

	if ( yPosition >= 60 ) {
		delay( 250 );
		yPosition = 0;
		display.clear();
	}
	Serial.println(str);
}


void setup( )
{
	display.init();
	display.flipScreenVertically();
	display.setTextAlignment( TEXT_ALIGN_LEFT );
	display.setFont( ArialMT_Plain_10 );

	initArduino();
	report( "Arduino initialized" );

	pinMode( SERIAL_LED, OUTPUT );
	pinMode( BTN, INPUT_PULLUP );

	Serial.begin( 115200 );
	report( "Serial initialized" );

	SerialBLE.begin( 115200, SERIAL_8N1, SERIAL_PINS[ 0 ], SERIAL_PINS[ 1 ] );
	report( "Serial bluetooth initialized" );

	Wire.begin( WIRE_PINS[ 0 ], WIRE_PINS[ 1 ], 400000 );
	report( "I2C initialized" );

//	32
	//static const uint8_t MOSI  = 23;
	//static const uint8_t MISO  = 19;
	//static const uint8_t SCK   = 18;
	//(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
	SPI.begin(14, 19, 23);

	bmp.begin();
	report(String(bmp.readAltitude(1010)));
	report(String(bmp.readPressure()));
	report(String(bmp.readTemperature()));

	report("------");
/*
	rtc.writeProtect(false);
	rtc.halt(false);
	Time t2(2017,7,7,11,7, 14, Time::Day::kFriday);
	rtc.time(t2);
	report(String(rtc.time().date));
	Time t = rtc.time();
	char buf[50];
	snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
	         "sad ",
	         t.yr, t.mon, t.date,
	         t.hr, t.min, t.sec);
	report(String(buf));
	rtc.begin( );
	rtc.adjust( DateTime( "Dec 26 2009", "12:34:56" ) );

	if ( !rtc.isrunning( ) ) {
		Serial.println( "RTC is NOT running!" );
		report( "RTC is NOT running!" );
		rtc.adjust( DateTime( "Dec 26 2009", "12:34:56" ) );
		exit( 1 );
	}

	DateTime now = rtc.now( );
	char buf[100];
	strncpy( buf, "DD.MM.YYYY  hh:mm:ss\0", 100 );
	report( String( now.format( buf ) ) );
	delay(1000);
	now = rtc.now( );
	strncpy( buf, "DD.MM.YYYY  hh:mm:ss\0", 100 );
	report( String( now.format( buf ) ) );
	exit( 1 );*/


	int counter = 0;
//	while ( !WiFi.isConnected() ) {
//		WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
//		report( "Trying to connect..." );
//		counter++;
//
//		if ( counter == WIFI_TRIES ) {
//			report( "Cant connect to wifi!" );
//			exit( 1 );
//		}
//		delay( 10 );
//	}
//
//	report( "WiFi connected" );

	time_t now = time( nullptr );
	time_t last = time( nullptr );
	counter = 0;
//	while ( !now || now == last) {
//		configTime( 3600, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com" );
//		time( & now ); // until time is set, it remains 0
//		counter++;
//
//		if ( counter == TIME_TRIES ) {
//			report( "Cant obtain time!" );
//			report( String( ctime( & now ) ) );
//			exit( 1 );
//		}
//
//		report( "Waiting for time..." );
//		report( String( ctime( & now ) ) );
//		delay( 10 );
//	}

	report( String( ctime( & now ) ) );

//	if ( !SD.begin(12) ) {
//		report( "Card Mount Failed" );
//		exit( 1 );
//	}
//	report( "Card mounted" );
//
//	uint8_t cardType = SD.cardType();
//	if ( cardType == CARD_NONE ) {
//		report( "No SD card attached" );
//		exit( 1 );
//	}
//	int cardSize = SD.cardSize() / ( 1024 * 1024 );
//	report( "SD Card Size: " + String( cardSize ) );
//	report( String( cardSize ) );
//
//	if ( !SD.open( "/GoTrack_logs" ).isDirectory() ) {
//		report( "mkdir GoTrack_logs" );
//		SD.mkdir( "/GoTrack_logs" );
//	}

//	std::vector < File > fs = finder.from( "/GoTrack_logs" )->type( Type::TypeFile )->depth( 0 )->get();
//	report( "ls logs:" );
//	for ( auto content : fs ) {
//		report( content.name() );
//	}


//	String path = "/GoTrack_logs/" + String( now ) + ".avakar";
//	report( "Path: " + path );
//	logs = SD.open( path, FILE_WRITE );

	byte c = mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
	if ( c == 0x71 ) {
		report( "MPU9250 connected" );
	} else {
		report(String(c));
		report( "MPU9250 failed to connected" );
		//exit( 1 );
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

	mpu.initMPU9250();

	byte d = mpu.readByte( AK8963_ADDRESS, WHO_AM_I_AK8963 );
	if ( d == 0x48 ) {
		report( "AK8963 connected" );
	} else {
		report( "AK8963 failed to connect" );
		//exit( 1 );
	}


	mpu.initAK8963( mpu.magCalibration );
	Serial.print( "X-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 0 ], 2 );
	Serial.print( "Y-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 1 ], 2 );
	Serial.print( "Z-Axis sensitivity adjustment value " );
	Serial.println( mpu.magCalibration[ 2 ], 2 );

	report(String(bmp.readPressure()));
	report(String(bmp.readTemperature()));

	delay( 500 );
	display.clear();



	ui.setTargetFPS( 23 );

	ui.setActiveSymbol( activeSymbol );
	ui.setInactiveSymbol( inactiveSymbol );

	ui.setIndicatorPosition( LEFT );
	ui.setIndicatorDirection( LEFT_RIGHT );
	ui.setFrameAnimation( SLIDE_UP );


	ui.setFrames( frames, frameCount );
	ui.setOverlays( overlays, overlaysCount );

	ui.init();
	display.flipScreenVertically();
	ui.disableAutoTransition();

	ui.getUiState()->userData = & displayContext;
	displayContext.newDataMPU = false;

}


void loop( )
{

	showGradient();

	ui.update();

	if ( mpu.readByte( MPU9250_ADDRESS, INT_STATUS ) & 0x01 ) {
		displayContext.newDataMPU = true;
		Serial.println( "New values" );
		mpu.readAccelData( mpu.accelCount );  // Read the x/y/z adc values
		mpu.getAres();

		// Now we'll calculate the accleration value into actual g's
		// This depends on scale being set
		mpu.ax = ( float ) mpu.accelCount[ 0 ] * mpu.aRes; // - accelBias[0];
		mpu.ay = ( float ) mpu.accelCount[ 1 ] * mpu.aRes; // - accelBias[1];
		mpu.az = ( float ) mpu.accelCount[ 2 ] * mpu.aRes; // - accelBias[2];

		mpu.readGyroData( mpu.gyroCount );  // Read the x/y/z adc values
		mpu.getGres();

		// Calculate the gyro value into actual degrees per second
		// This depends on scale being set
		mpu.gx = ( float ) mpu.gyroCount[ 0 ] * mpu.gRes;
		mpu.gy = ( float ) mpu.gyroCount[ 1 ] * mpu.gRes;
		mpu.gz = ( float ) mpu.gyroCount[ 2 ] * mpu.gRes;

		mpu.readMagData( mpu.magCount );  // Read the x/y/z adc values
		mpu.getMres();

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
	mpu.updateTime();

	mpu.delt_t = millis() - mpu.count;
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


		mpu.tempCount = mpu.readTempData();
		mpu.temperature = ( ( float ) mpu.tempCount ) / 333.87 + 21.0;


		AvakarPacket data;
		data.set_command( 6 );
		data.push < float >( ( float ) mpu.sumCount / mpu.sum ); //Hz
		data.push < float >( mpu.temperature );

		send( data );

		mpu.count = millis();
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
		displayContext.packetCount = packetCount;
	}

	if ( digitalRead( BTN ) == 1 ) {
		ui.nextFrame();
	}
}
