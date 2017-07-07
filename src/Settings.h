#pragma once

const uint8_t SERIAL_PINS[2] = { 16, 4 };
const uint8_t WIRE_PINS[2] = { 21, 22 };
const uint8_t SPI_PINS[2] = { 21, 22 };
const uint8_t RT_CLOCK_PINS[3] = { 33, 35, 32 }; // RTS(CE), CLK, I/O

const int SERIAL_LED = 15;
const int BTN = 25;
const int FLUSH_AT = 500;

const char* WIFI_SSID     = "Technika";
const char* WIFI_PASSWORD = "materidouska";
const int WIFI_TRIES = 40;

const int TIME_TRIES = 40;