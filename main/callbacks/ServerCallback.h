#pragma once

class ServerCallbacks : public BLEServerCallbacks
{
public:
	ServerCallbacks() = default;

	static bool deviceConnected;
private:
	void onConnect( BLEServer *pServer )
	{
		deviceConnected = true;
	};


	void onDisconnect( BLEServer *pServer )
	{
		deviceConnected = false;
	}


};

bool ServerCallbacks::deviceConnected = false;
