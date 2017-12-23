#pragma once

#ifndef TEST_DDD
#define TEST_DDD

//#include <logging.hpp>
#include <string>
#include <OLEDDisplay.h>
#include <deque>

using namespace std::string_literals;

template < class StringT >
class DisplayLogSinkT : public BaseLogSink < StringT >
{
public:
	DisplayLogSinkT( OLEDDisplay & display, uint8_t width, uint8_t height, uint8_t fontSize ) : _display( display ),
	                                                                                            _width( width ),
	                                                                                            _height( height ),
	                                                                                            _fontSize( fontSize )
	{
		if ( _width < 0 ) {
			_width = 0;
		}

		rows = height / fontSize;

	}


	virtual void log( Verbosity verb, const StringT & tag, const StringT & message,
	                  uint64_t timestamp ) override
	{

		StringT msgPart = "";
		int counter = 0;
		for ( char c : message) {
			msgPart += c;
			counter++;
			if (counter > _width){
				lines.push_back(msgPart);
				msgPart = "";
				counter = 0;
			}
		} // ?????

		if (msgPart != "") {
			lines.push_back( msgPart );
		}

		if ( lines.size() > rows ) {
			lines.pop_front();
		}

		_display.clear();

		for ( uint8_t i = 0; i < rows && ( i + 1 ) <= lines.size(); i++ ) {
			String msg = "";
			for ( char c : lines[ i ] ) {
				msg += String( c );
			}

			_display.drawString( 0, uint8_t( i * _fontSize ), msg );

		}

		_display.display();
	}


private:
	//std::string header;
	OLEDDisplay & _display;
	std::deque < StringT > lines;

	uint8_t _width;
	uint8_t _height;
	uint8_t _fontSize;

	uint8_t rows;

	static constexpr const int TIME_WIDTH = 0;
	static constexpr const int LEVEL_WIDTH = 5;
	static constexpr const int TAG_WIDTH = 0;
};

using DisplayLogSink = DisplayLogSinkT < std::string >;

#endif