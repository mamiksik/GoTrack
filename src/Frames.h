#pragma once

#include <time.h>
#include "Settings.h"

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

	float temp;
	int packetCount;
};

void savedOverlay( OLEDDisplay *display, OLEDDisplayUiState *state )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_LEFT );
	display->setFont( ArialMT_Plain_10 );

	if(context->packetCount > FLUSH_AT - 100) {
		display->drawString( 20, 0, "Saving...");
	}

	if(context->packetCount < 100) {
		display->drawString( 20, 0, "Saved!");
	}

}

void packetOverlay( OLEDDisplay *display, OLEDDisplayUiState *state )
{
	DisplayContext *context = reinterpret_cast<DisplayContext *>(state->userData);
	display->setTextAlignment( TEXT_ALIGN_RIGHT );
	display->setFont( ArialMT_Plain_10 );

	display->drawString( 128, 0, "Packets: " + String(context->packetCount));
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

	struct tm * timeinfo;
	char buffer [80];

	time_t now = 0;
	time(&now);
	timeinfo = localtime(&now);

	strftime(buffer, 80, "%D " ,timeinfo);
	display->drawString( x + 32, 26 + y, buffer);

	strftime(buffer, 80, "%T" ,timeinfo);
	display->drawString( x + 32, 36 + y, buffer);
}