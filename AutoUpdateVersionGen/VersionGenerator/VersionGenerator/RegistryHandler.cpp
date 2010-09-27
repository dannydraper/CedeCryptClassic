#include "RegistryHandler.h"

RegistryHandler::RegistryHandler ()
{
	ZeroMemory (m_szReadstring, SIZE_STRING);
}

RegistryHandler::~RegistryHandler ()
{
	
}

void RegistryHandler::WriteString (LPCSTR szHive, LPCSTR szKey, LPCSTR szValue) {
	// Write a REG_SZ value to the registry.
	HKEY hParentKey;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, 1};
	DWORD state = 0;
	long lRes = 0;
	
	RegCreateKeyEx (HKEY_LOCAL_MACHINE, szHive, 0, (LPTSTR) NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, &sa, &hParentKey, &state);
	
	if( state == 2) /* opened existing key */
	{
		RegCloseKey (hParentKey);
		RegOpenKeyEx (HKEY_LOCAL_MACHINE, szHive, 0, KEY_WRITE, &hParentKey);
		RegSetValueEx(hParentKey, szKey, 0, REG_SZ, (BYTE *) szValue, strlen (szValue));
	} else {
		/* save them to the registry */
		RegSetValueEx(hParentKey, szKey, 0, REG_SZ, (BYTE *) szValue, strlen (szValue));
	}
}

LPCSTR RegistryHandler::ReadString (LPCSTR szHive, LPCSTR szKey) {
	// Function to read a REG_SZ value from the registry.
	HKEY hParentKey;
	
	ZeroMemory (m_szReadstring, SIZE_STRING);
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, 1};
	DWORD size =  0, state = 0, type = REG_SZ;
	long lRes = 0;
	//DWORD dwErr = 0;
	RegCreateKeyEx (HKEY_LOCAL_MACHINE, szHive, 0, (LPTSTR) NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, &sa, &hParentKey, &state);
	
	if( state == 2) /* opened existing key */
	{
		RegCloseKey (hParentKey);
		RegOpenKeyEx (HKEY_LOCAL_MACHINE, szHive, 0, KEY_READ, &hParentKey);
		size = SIZE_STRING;
		lRes = RegQueryValueEx(hParentKey, szKey, NULL, &type, (unsigned char*) m_szReadstring, &size);
		if (lRes == ERROR_SUCCESS) {
			//MessageBox (NULL, "Reg function succeeded.", "Info", MB_OK);
		} else {
			if (lRes == 2L) {
				//MessageBox (NULL, "Error is 2L", "Info", MB_OK);
			} else {
				//MessageBox (NULL, "Reg function failed. Not 2L", "Info", MB_OK);
			}
		}
		if (lRes != 0) {
			RegCloseKey (hParentKey);
		} else {
			RegCloseKey (hParentKey);
		}
	} 
	return m_szReadstring;
}