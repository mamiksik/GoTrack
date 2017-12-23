#pragma once

#include <format.hpp>
#include <string>

using namespace std::string_literals;

template < class String >
class SerialLogSinkT : public BaseLogSink < String >
{
public:
	SerialLogSinkT( HardwareSerial & serial, int width ) : serial( serial )
	{
		_width = width;

		if ( _width < 0 ) {
			_width = 0;
		}


		std::string data = format( "| {} | {} | {} | {} |\n" )
				<< string( "Time" ).width( TIME_WIDTH ).alignLeft()
				<< string( "Level" ).width( LEVEL_WIDTH ).center()
				<< string( "Tag" ).width( TAG_WIDTH ).center()
				<< string( "Message " ).width( _width ).center();

		for ( auto ch : header ) {
			serial.write( ch );
		}
	}


	virtual void log( Verbosity verb, const String & tag, const String & message,
	                  uint64_t timestamp ) override
	{
		std::string data;

		static std::vector < String > levels( { "panic", "error", "warning", "info", "debug" } );
		if ( verb >= PANIC && verb <= DEBUG ) {
			data = format( "| {} | {} | {} | {} |\n" )
					<< number( timestamp ).alignRight().width( TIME_WIDTH )
					<< string( levels[ verb + 3 ] ).alignRight().width( LEVEL_WIDTH )
					<< string( tag ).alignRight().width( TAG_WIDTH )
					<< string( message ).alignLeft().width( _width ).clip();
		} else {
			data = format( "| {} | {} | {} | {} |\n" )
					<< number( timestamp ).alignRight().width( TIME_WIDTH )
					<< number( static_cast< int >( verb ) ).width( LEVEL_WIDTH )
					<< string( tag ).alignRight().width( TAG_WIDTH )
					<< string( message ).alignLeft().width( _width ).clip();
		}

		for ( auto ch : data ) {
			serial.write( ch );
		}

	}


private:
	std::string header;
	HardwareSerial & serial;

	int _width;

	static constexpr const int TIME_WIDTH = 10;
	static constexpr const int LEVEL_WIDTH = 7;
	static constexpr const int TAG_WIDTH = 10;
};

using SerialLogSink = SerialLogSinkT < std::string >;
