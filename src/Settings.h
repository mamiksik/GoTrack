#pragma once

const uint8_t SERIAL_PINS[2] = { 16, 4 };
const uint8_t WIRE_PINS[2] = { 25, 33 };
const uint8_t SPI_PINS[3] = { 5, 4, 18};
const uint8_t SD_CS = 19;
//const uint8_t RT_CLOCK_PINS[3] = { 33, 35, 32 }; // RTS(CE), CLK, I/O

const int SERIAL_LED = 15;
const int BTN = 34;
const int GND_PIN = 35;
const int FLUSH_AT = 500;

const int LED_COUNT = 3;
const int DATA_PIN = 22;
const int CHANNEL = 0;

const char* WIFI_SSID     = "dd-wrt";
const char* WIFI_PASSWORD = "Etna991?deem";
const int WIFI_TRIES = 40;

const int TIME_TRIES = 40;




/**
 *	NOTES
 */


/**
 * BMP280 or BME280 address is 0x77 if SDO pin is high, and is 0x76 if
 * SDO pin is low.
 */