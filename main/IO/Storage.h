#pragma once


#include <string>
#include <queue>
#include "Common.h"

class Storage
{
public:
	enum Tag {
		ACC_X,
		ACC_Y,
		ACC_Z,
	};

	explicit Storage(Common& common): _common(common){};

	void addLoad(Tag tag, std::string data);
	void flush();

private:
	Common _common;
	std::queue<std::string> _load;
};


