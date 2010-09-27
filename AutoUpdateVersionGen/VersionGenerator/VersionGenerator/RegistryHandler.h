#pragma once
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include "resource.h"

class RegistryHandler {
	
	public:
		RegistryHandler ();
		~RegistryHandler ();		
		void WriteString (LPCSTR szHive, LPCSTR szKey, LPCSTR szValue);
		LPCSTR ReadString (LPCSTR szHive, LPCSTR szKey);

	private:

		// Storage for last read string
		char m_szReadstring[SIZE_STRING];

};