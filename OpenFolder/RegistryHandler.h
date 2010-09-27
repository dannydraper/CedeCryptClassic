#pragma once
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include "MemoryBuffer.h"
#include "resource.h"

class RegistryHandler {
	
	public:
		RegistryHandler ();
		~RegistryHandler ();		
		void WriteString (LPCSTR szHive, LPCSTR szKey, LPCSTR szValue);
		void WriteStringCR (LPCSTR szHive, LPCSTR szKey, LPCSTR szValue);

		LPCSTR ReadStringCU (LPCSTR szHive, LPCSTR szKey);
		LPCSTR ReadString (LPCSTR szHive, LPCSTR szKey);
		void WriteDword (LPCSTR szHive, LPCSTR szKey, DWORD dwValue);
		DWORD ReadDword (LPCSTR szHive, LPCSTR szKey);
		bool DoesValueExist (LPCSTR szHive, LPCSTR szValue, DWORD dwRegType);
		bool DoesValueExistCU (LPCSTR szHive, LPCSTR szValue, DWORD dwRegType);

		void WriteDwordEx (LPCSTR szKey, DWORD dwValue);
		DWORD ReadDwordEx (LPCSTR szKey);
		bool DoesKeyExist (LPCSTR szHive);		
		void WriteBinary (LPCSTR szHive, LPCSTR szKey, MemoryBuffer *memBuffer);
		void ReadBinary (LPCSTR szHive, LPCSTR szKey, MemoryBuffer *memBuffer, unsigned int iSize);


	private:

		
		// Storage for last read string
		char m_szReadstring[SIZE_STRING];

};