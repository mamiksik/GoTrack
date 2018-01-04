#pragma once

#ifndef GOTRACK_FRAMES
#define GOTRACK_FRAMES

#include "Frames.h"


void sessionOverlay( OLEDDisplay *display, OLEDDisplayUiState *state )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	display->setTextAlignment( TEXT_ALIGN_RIGHT );
	display->setFont( ArialMT_Plain_10 );
	display->drawString( 20, 0, "SE:");
	display->drawCircle(24, 6, 3);

	if ( context->session ) {
		display->fillCircle(24, 6, 3);
	}
}


//const char BLE_Logo_bits[] PROGMEM = {
//		0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
//		0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
//		0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
//		0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00,
//		0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
//		0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
//};


void bleOverlay( OLEDDisplay *display, OLEDDisplayUiState *state )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);

	display->setTextAlignment( TEXT_ALIGN_RIGHT );
	display->setFont( ArialMT_Plain_10 );
	display->drawString( 112, 0, "BLE:");
	display->drawCircle(120, 6, 3);

	if ( context->BLEConnected ) {
		display->fillCircle(120, 6, 3);
	}
}


void introFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_24 );
	display->drawString( x + 20, y + 16, "GoTrack" );
	display->setFont( ArialMT_Plain_10 );
	display->drawString( x + 20, y + 40, "Martin Miksik" );

}


void gyroFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String gx = "Gyro x: " + String( context->gyroX );
	display->drawString( x + 32, 16 + y, gx );

	String gy = "Gyro y: " + String( context->gyroY );
	display->drawString( x + 32, 26 + y, gy );

	String gz = "Gyro z: " + String( context->gyroZ );
	display->drawString( x + 32, 36 + y, gz );
}


void accFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String ax = "Acc x: " + String( context->accX );
	display->drawString( x + 32, 16 + y, ax );

	String ay = "Acc y: " + String( context->accY );
	display->drawString( x + 32, 26 + y, ay );

	String az = "Acc z: " + String( context->accZ );
	display->drawString( x + 32, 36 + y, az );
}


void magFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String mx = "Mag x: " + String( context->magX );
	display->drawString( x + 32, 16 + y, mx );

	String my = "Mag y: " + String( context->magY );
	display->drawString( x + 32, 26 + y, my );

	String mz = "Mag z: " + String( context->magZ );
	display->drawString( x + 32, 36 + y, mz );
}


void dataFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	String temp = "Temp: " + String( context->temp );
	display->drawString( x + 32, 16 + y, temp );

	struct tm *timeinfo;
	char buffer[80];

	time_t now = 0;
	time( & now );
	timeinfo = localtime( & now );

//	strftime( buffer, 80, "%D ", timeinfo );
	temp = "Press: " + String( context->pressure );
	display->drawString( x + 32, 26 + y, temp );

	strftime( buffer, 80, "%T", timeinfo );
	display->drawString( x + 32, 36 + y, buffer );
}


#endif