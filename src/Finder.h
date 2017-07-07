
#pragma once

#include <SD.h>
#include <string>
#include <vector>

enum Type
{
	TypeFile = 0,
	TypeDirectory
};

class Finder
{
public:


	Finder( const fs::FS & fs, const String path = "/" ) : fs( fs ), _path( path ) { }


	Finder *from( String path )
	{
		_path = path;
		return this;
	}


	Finder *type( Type type )
	{
		_type = type;
		return this;
	}


	Finder *depth( int depth )
	{
		_depth = depth;
		return this;
	}


	String currentDirectory( )
	{
		return _path;
	}


	std::vector < File > find( String name )
	{
		File dir = fs.open( _path );

		if ( !dir.isDirectory( ) ) {
//			throw new
		}

		std::vector < File > result;
		File file = dir.openNextFile( );

		while ( file ) {
			if ( ( _type == Type::TypeDirectory && file.isDirectory( ) ) ||
			     ( _type == Type::TypeFile && !file.isDirectory( ) ) ) {
				if ( String(file.name( )) == name ) {
					result.push_back( file );
				}

				if ( _depth > 0 ) {
					String path = _path;
					std::vector < File > child = from( file.name( ) )->depth( _depth - 1 )->find( name );
					for ( auto childFile : child ) {
						result.push_back( childFile );
					}
					_depth += 1;
					_path = path;
				}
			}
			file = dir.openNextFile( );
		}

		return result;
	}


	std::vector < File > get( )
	{
		File dir = fs.open( _path );

		if ( !dir ) {
//			return std::vector < String >{ };
		}

		if ( !dir.isDirectory( ) ) {
//			return std::vector < String >{ };
		}

		std::vector < File > result;

		File file = dir.openNextFile( );
		while ( file ) {
			if ( file.isDirectory( ) ) {
				result.push_back( file);
				if ( _depth > 0 ) {
					String path = _path;
					std::vector < File > child = from( file.name( ) )->depth( _depth - 1 )->get( );
					for ( auto childFile : child ) {
						result.push_back( childFile );
					}
					_depth += 1;
					_path = path;
				}
			} else {
				result.push_back( file );
			}
			file = dir.openNextFile( );
		}

		return result;
	}


private:
	fs::FS fs;

	String _path;
	Type _type;
	int _depth;


};