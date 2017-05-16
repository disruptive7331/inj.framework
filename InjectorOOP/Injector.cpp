#include "Injector.hpp"

namespace InjectionFramework
{
	Injector::Injector( const uint16_t pid )
	{
		this->m_pid = pid;

		while ( !this->isProcessActive( ) )
			Sleep( 20 );

		this->m_processHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, this->m_pid );
	}

	Injector::Injector( const std::string& processName )
	{
		this->m_processName = processName;
		this->m_pid = 0;

		while ( !this->isProcessActive( ) )
			Sleep( 20 );

		this->m_processHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, this->m_pid );
	}

	Injector::Injector( const uint16_t pid, const std::string& dllName )
	{
		this->m_pid = pid;
		this->m_dllName = dllName;

		while ( !this->isProcessActive( ) )
			Sleep( 20 );

		this->m_processHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, this->m_pid );
		setup( );
	}

	Injector::Injector( const std::string & processName, const std::string& dllName )
	{
		this->m_processName = processName;
		this->m_dllName = dllName;

		while ( !this->isProcessActive( ) )
			Sleep( 20 );

		this->m_processHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, this->m_pid );
		setup( );
	}

	void Injector::setPid( const uint16_t pid )
	{
		this->m_pid = pid;
	}

	void Injector::setDllName( const std::string& dllName )
	{
		this->m_dllName = dllName;
	}

	bool Injector::inject( )
	{
		if ( this->m_pszDllPath == nullptr )
			setup( );

		if ( this->m_dllName.empty( ) ||
			this->m_processHandle == NULL ||
			this->m_processHandle == INVALID_HANDLE_VALUE ||
			( this->m_pid == 0 && this->m_processName.empty( ) ) )
			return false;

		if ( !this->isProcessActive( ) )
			return false;


		this->m_injectionThread = CreateRemoteThread( this->m_processHandle, nullptr, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( &LoadLibraryA ), this->m_pszDllPath, 0, nullptr );

		if ( !this->m_injectionThread )
			return false;

		WaitForSingleObject( this->m_injectionThread, INFINITE );

		return !!VirtualFreeEx( this->m_processHandle, this->m_pszDllPath, 0, MEM_RELEASE );
	}

	void Injector::setup( )
	{
		if ( !this->m_processHandle )
			return;

		if ( this->m_dllName.empty( ) )
			return;

		char buf[ MAX_PATH ] = { 0 };
		if ( GetFullPathNameA( this->m_dllName.c_str( ), MAX_PATH, buf, nullptr ) )
		{
			this->m_pszDllPath = static_cast< char* >( VirtualAllocEx( this->m_processHandle, nullptr, strlen( buf + 1 ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ) );

			if ( !this->m_pszDllPath )
				return;

			if ( !WriteProcessMemory( this->m_processHandle, this->m_pszDllPath, buf, strlen( buf + 1 ), nullptr ) )
				return;
		}
	}

	bool Injector::isProcessActive( )
	{
		HANDLE snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if ( snap == INVALID_HANDLE_VALUE )
			return false;

		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof( PROCESSENTRY32 );

		if ( Process32First( snap, &processEntry ) )
		{
			do
			{
				if ( _stricmp( this->m_processName.c_str( ), unicodeToString( processEntry.szExeFile ).c_str( ) ) == 0
					|| this->m_pid == processEntry.th32ProcessID )
				{
					this->m_pid = static_cast< uint16_t >( processEntry.th32ProcessID );
					CloseHandle( snap );
					return true;
				}
			} while ( Process32Next( snap, &processEntry ) );
		}

		CloseHandle( snap );
		return false;
	}
}
