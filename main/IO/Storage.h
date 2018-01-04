#pragma once


#include <string>
#include <queue>
#include <SD.h>

#define SD_CS 18

class Storage
{
public:
	enum Tag {
		ACC_X,
		ACC_Y,
		ACC_Z,

		GYRO_X,
		GYRO_Y,
		GYRO_Z,

		MAG_X,
		MAG_Y,
		MAG_Z,

		BMP_PRESSURE,
		BMP_TEMPERATURE,
	};

	Storage();

	void addLoad(Tag tag, std::string data);
	void flush();

private:
	std::queue<std::string> _load;
};


