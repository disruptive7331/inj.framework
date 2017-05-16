#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>
#include <codecvt>
#include <algorithm>

namespace InjectionFramework
{
	inline std::string unicodeToString( const std::wstring& unicodeString )
	{
		std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> converter;
		return converter.to_bytes( reinterpret_cast< const int16_t* >( unicodeString.data( ) ) );
	}

	class Injector
	{
	public:
		Injector( ) = default;
		Injector( const uint16_t pid );
		Injector( const std::string& processName );
		Injector( const uint16_t pid, const std::string& dllName );
		Injector( const std::string& processName, const std::string& dllName );

	public:
		void setPid( const uint16_t pid );
		void setDllName( const std::string& dllName );
		bool inject( );
		void setup( );

	public:
		bool isProcessActive( );

	private:
		uint16_t m_pid;
		std::string m_dllName, m_processName;
		char* m_pszDllPath;
		HANDLE m_injectionThread, m_processHandle;
	};
}