#pragma once


#include <time.h>
#include "OLEDDisplay.h"
#include "OLEDDisplayUi.h"
#include "WString.h"

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

	bool session;
	bool BLEConnected;
};

extern void sessionOverlay( OLEDDisplay *display, OLEDDisplayUiState *state );

extern void bleOverlay( OLEDDisplay *display, OLEDDisplayUiState *state );


extern void introFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void gyroFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void accFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void magFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );


extern void dataFrame( OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y );