#pragma once


#include <Tasks.h>

#define LED_PIN 23

class ServerCallbacks final : public BLEServerCallbacks
{
public:
	ServerCallbacks( )
	{
		pinMode(LED_PIN, OUTPUT);
		digitalWrite(LED_PIN, LOW);
	};

	static bool deviceConnected;
private:
	void onConnect( BLEServer *pServer ) override
	{
		deviceConnected = true;
		Tasks::BLEHandler(true);
		digitalWrite(LED_PIN, HIGH);
	};


	void onDisconnect( BLEServer *pServer ) override
	{
		deviceConnected = false;
		Tasks::BLEHandler(false);
		digitalWrite(LED_PIN, LOW);
	}

};

bool ServerCallbacks::deviceConnected = false;
