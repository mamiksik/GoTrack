#pragma once


#include <Tasks.h>

class ServerCallbacks : public BLEServerCallbacks
{
public:
	ServerCallbacks( )
	{
		pinMode(LED, OUTPUT);
		digitalWrite(LED, LOW);
	};

	static bool deviceConnected;
private:
	void onConnect( BLEServer *pServer )
	{
		deviceConnected = true;
		Tasks::BLEHandler(true);
		digitalWrite(LED, HIGH);
	};


	void onDisconnect( BLEServer *pServer )
	{
		deviceConnected = false;
		Tasks::BLEHandler(false);
		digitalWrite(LED, LOW);
	}

	const uint8_t LED = 23;
};

bool ServerCallbacks::deviceConnected = false;
