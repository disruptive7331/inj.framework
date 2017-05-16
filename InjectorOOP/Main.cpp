#include "Injector.hpp"

int main( )
{
	// 1
	InjectionFramework::Injector injector( "csgo.exe", "cheat.dll" );
	injector.inject( );

	// 2
	InjectionFramework::Injector injector2( "csgo.exe" );
	injector2.setDllName( "cheat.dll" );
	injector2.inject( );

	// 3
	InjectionFramework::Injector injector3( 6113, "cheat.dll" );
	injector3.inject( );

	// 4
	InjectionFramework::Injector injector4( 6113 );
	injector4.setDllName( "cheat.dll" );
	injector4.inject( );
}