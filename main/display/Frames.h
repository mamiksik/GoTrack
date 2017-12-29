#pragma once


#include <time.h>
#include "OLEDDisplay.h"
#include "OLEDDisplayUi.h"
#include "WString.h"

const int FLUSH_AT = 500;

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
	float pressure;
	int packetCount;

	bool BLEConnected;
};

extern void savedOverlay( OLEDDisplay *display, OLEDDisplayUiState *state );

extern void packetOverlay( OLEDDisplay *display, OLEDDisplayUiState *state );


extern void introFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void gyroFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void accFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void magFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void dataFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );