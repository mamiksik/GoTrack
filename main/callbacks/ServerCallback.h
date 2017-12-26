#pragma once


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
		digitalWrite(LED, HIGH);
	};


	void onDisconnect( BLEServer *pServer )
	{
		deviceConnected = false;
		digitalWrite(LED, LOW);
	}

	const uint8_t LED = 23;
};

bool ServerCallbacks::deviceConnected = false;
