#pragma once


class Client
{

	void indicate();

	void getSession();

	BLECharacteristic *notification;
	BLECharacteristic *lastSession;

	BLECharacteristic *timestamp;
};

