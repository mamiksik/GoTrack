#include <Arduino.h>
#include <Wire.h>
#include <string>
#include <vector>
#include <time.h>
#include <cstdlib>
//#include <csignal>

#include <MPU9250.h>
#include <OLEDDisplayUi.h>
#include <SSD1306.h>
#include <Frames.h>
#include <WiFi.h>
#include <SmartLeds.h>
#include <Adafruit_BMP280.h>
#include <logging.hpp>

#include "Settings.h"
#include "Symbols.h"
#include "FS.h"
#include "SD.h"
#include "Finder.h"
#include <../lib/atoms/include/atoms/communication/avakar.h>
#include "LogSinks/DisplayLogSink.h"
#include "LogSinks/SerialLogSink.h"

#include "SimpleBLE.h"


SimpleBLE ble;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

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

FrameCallback frames[] = { introFrame, gyroFrame, accFrame, magFrame, dataFrame };
const int frameCount = 5;

OverlayCallback overlays[] = { packetOverlay, savedOverlay };
const int overlaysCount = 2;

int yPosition = 0;
int packetCount = 0;

Logger l;

uint8_t hue;


void showGradient( )
{
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


void showRgb( )
{
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

void exit_handler(int signal)
{
	delay(1400);
}

void setup( )
{
	//signal(SIGINT, exit_handler);

	initArduino();

	display.init();
	display.flipScreenVertically();
	display.setTextAlignment( TEXT_ALIGN_LEFT );
	display.setFont( ArialMT_Plain_10 );

	l.addSink( ALL, std::unique_ptr < LogSink >( new DisplayLogSink( display, 140, 64, 10 ) ) );
	//l.addSink( ALL, std::unique_ptr < LogSink >( new SerialLogSink( SerialBLE, 255 ) ) );
	l.addSink( ALL, std::unique_ptr < LogSink >( new SerialLogSink( Serial, 255 ) ) );

	l.logInfo( "", "Logger initialized" );

	//ble.begin("ESP32 SimpleBLE");
	//l.logInfo( "", "Bluetooth on!" );

	pinMode( SERIAL_LED, OUTPUT );

	pinMode( GND_PIN, OUTPUT );
	digitalWrite( GND_PIN, LOW );

	pinMode( BTN, INPUT_PULLUP );

	Serial.begin( 115200 );
	l.logInfo( "", "Serial initialized" );

	SerialBLE.begin( 115200, SERIAL_8N1, SERIAL_PINS[ 0 ], SERIAL_PINS[ 1 ] );
	l.logInfo( "", "Serial bluetooth initialized" );

	Wire.begin( WIRE_PINS[ 0 ], WIRE_PINS[ 1 ], 400000 );
	l.logInfo( "", "I2C initialized" );

	SPI.begin( SPI_PINS[0], SPI_PINS[1], SPI_PINS[2] );
	l.logInfo( "", "SPI initialized" );

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

	l.logInfo( "", "Wifi disabled" );
	l.logInfo( "", "UTC time disabled" );

/*	int counter = 0;
	while ( !WiFi.isConnected() ) {
		WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
		report( "Trying to connect..." );
		counter++;

		if ( counter == WIFI_TRIES ) {
			report( "Cant connect to wifi!" );
			exit( 1 );
		}
		delay( 10 );
	}

	report( "WiFi connected" );

	time_t now = time( nullptr );
	time_t last = time( nullptr );
	counter = 0;
	while ( !now || now == last) {
		configTime( 3600, 0, "pool.ntp.org", "time.nist.gov", "time.windows.com" );
		time( & now ); // until time is set, it remains 0
		counter++;

		if ( counter == TIME_TRIES ) {
			report( "Cant obtain time!" );
			report( String( ctime( & now ) ) );
			exit( 1 );
		}

		report( "Waiting for time..." );
		report( String( ctime( & now ) ) );
		delay( 10 );
	}

	l.logDebug( "", ctime( & now ) );
    */

	if ( !SD.begin( SD_CS, SPI ) ) {
		l.logError( "", "SD Slot mount Failed" );
		//raise(SIGINT);
	}
	l.logInfo( "", "SD Slot initailized" );

	uint8_t cardType = SD.cardType();
	if ( cardType == CARD_NONE ) {
		l.logError( "", "No SD card attached" );
		//raise(SIGINT);
	}
	int cardSize = SD.cardSize() / ( 1024 * 1024 );
	l.logDebug( "", "SD Size: " + cardSize );

	if ( !SD.open( "/GoTrack_logs" ).isDirectory() ) {
		l.logInfo( "", "mkdir GoTrack_logs" );
		SD.mkdir( "/GoTrack_logs" );
	}

	std::vector < File > fs = finder.from( "/GoTrack_logs" )->type( Type::TypeFile )->depth( 0 )->get();
	l.logDebug( "", "ls logs:" );
	for ( auto content : fs ) {
		l.logDebug( "", content.name() );
	}


	//String path = "/GoTrack_logs/" + now + ".avakar";
	//std::string path = "/GoTrack_logs/" + rand() + ".avakar";
	std::string path = "/GoTrack_logs/a.avakar";
	l.logDebug( "", "Path: " + path );
	logs = SD.open( path.c_str(), FILE_WRITE );

	byte c = mpu.readByte( MPU9250_ADDRESS, WHO_AM_I_MPU9250 );
	if ( c == 0x71 ) {
		l.logInfo( "", "MPU9250 connected" );
	} else {
		l.logError( "", "MPU9250 failed to connected" );
		l.logDebug( "", "Wrong adress!" );
//		exit( 1 );
	}

	// Start by performing self test and reporting values
	mpu.MPU9250SelfTest( mpu.SelfTest );
	l.logDebug( "", "x-axis self test: acceleration trim within : %  of factory value", mpu.SelfTest[ 0 ] );
	l.logDebug( "", "y-axis self test: acceleration trim within : % of factory value", mpu.SelfTest[ 1 ] );
	l.logDebug( "", "z-axis self test: acceleration trim within : % of factory value", mpu.SelfTest[ 2 ] );
	l.logDebug( "", "x-axis self test: gyration trim within : % of factory value", mpu.SelfTest[ 3 ] );
	l.logDebug( "", "y-axis self test: gyration trim within : % of factory value", mpu.SelfTest[ 4 ] );
	l.logDebug( "", "z-axis self test: gyration trim within : % of factory value", mpu.SelfTest[ 5 ] );

	// Calibrate gyro and accelerometers, load biases in bias registers
	mpu.calibrateMPU9250( mpu.gyroBias, mpu.accelBias );

	mpu.initMPU9250();

	byte d = mpu.readByte( AK8963_ADDRESS, WHO_AM_I_AK8963 );
	if ( d == 0x48 ) {
		l.logInfo( "", "AK8963 connected" );
	} else {
		l.logError( "", "AK8963 failed to connect" );
		//raise(SIGINT);
	}

	mpu.initAK8963( mpu.magCalibration );
	l.logDebug( "", "X-Axis sensitivity adjustment value %", mpu.magCalibration[ 0 ] );
	l.logDebug( "", "Y-Axis sensitivity adjustment value %", mpu.magCalibration[ 1 ] );
	l.logDebug( "", "Z-Axis sensitivity adjustment value %", mpu.magCalibration[ 2 ] );


	bmp.begin();
	l.logDebug( "", "Current pressure: %", bmp.readPressure() );
	l.logDebug( "", "Current temperature: %", bmp.readTemperature() );

	delay( 1400 );
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

	ble.begin("ESP32 SimpleBLE");

	String out = "BLE32 name: ";
	ble.begin(out);

}


void loop( )
{

	showGradient();

	ui.update();
	return;
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
