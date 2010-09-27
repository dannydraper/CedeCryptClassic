#ifndef HDR_OSVERSION_H
#define HDR_OSVERSION_H

// Class for identifying the Operating System Version

// Version 3.10
// License - Free for any purpose, just do not remove the copyright
// Copyright (C) 1996-2007 - Jonathan Wilkes (www.xanya.net)

/* History
	v3.10	Added support for Windows Vista
	v3.01	Bug fix in IsWinME() discovered by frankd (http://www.codeguru.com/Cpp/W-P/win32/tutorials/comments.php/c10849/?thread=8491)
	v3.00	Initial Public Release
*/

#if !defined(STRICT)
#define STRICT 1
#endif

// Includes
#if !defined(_WINDOWS_)
#include <windows.h>
#endif

#include <string>
using std::string;

namespace darka
{
	class OSVersion
	{
	private:
		OSVERSIONINFO verInfo_;

	public:
		// Inline Constructor
		OSVersion()
		{ 
			verInfo_.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&verInfo_); 
		};

		inline DWORD GetMajorVersion() const { return verInfo_.dwMajorVersion; };
		inline DWORD GetMinorVersion() const { return verInfo_.dwMinorVersion; };
		inline DWORD GetBuildNumber() const { return verInfo_.dwBuildNumber; };

		// Returns a pointer to a null-terminated string that provides arbitrary additional information about the operating system, such as the Service Pack installed
		inline string GetVersionString() const { return string(verInfo_.szCSDVersion); };

		// Various OS methods
		inline bool IsWin32s() const { return verInfo_.dwPlatformId == VER_PLATFORM_WIN32s ? true : false; } // Is this Win32s on Windows 3.1 ?

		inline bool IsWinNT() const { return (IsWinNT351() || IsWinNT4()) ? true : false; } // Is this Windows NT ?
		inline bool IsWinNT351() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 3) && (verInfo_.dwMinorVersion == 51)) ? true : false; }; // Is this Windows NT 3.51 ?
		inline bool IsWinNT4() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 4) && (verInfo_.dwMinorVersion == 0)) ? true : false; }; // Is this Windows NT4 ?

		inline bool IsWin95() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (verInfo_.dwMajorVersion == 4) && (verInfo_.dwMinorVersion == 0)) ? true : false; } // Is this Windows 95 ?
		inline bool IsWin98() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (verInfo_.dwMajorVersion == 4) && (verInfo_.dwMinorVersion == 10)) ? true : false; } // Is this Windows 98 ?
		inline bool IsWinME() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (verInfo_.dwMajorVersion == 4) && (verInfo_.dwMinorVersion == 90)) ? true : false; }; // Is this Windows ME ?

		inline bool IsWin2000() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 5) && (verInfo_.dwMinorVersion == 0)) ? true : false; } // Is this Windows 2000 ?
		inline bool IsWinXP() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 5) && (verInfo_.dwMinorVersion == 1)) ? true : false; }; // Is this Windows XP ?
		inline bool IsWinServer2003() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 5) && (verInfo_.dwMinorVersion == 2)) ? true : false; }; // Is this Windows Server 2003 ?

		inline bool IsWinVista() const { return ((verInfo_.dwPlatformId == VER_PLATFORM_WIN32_NT) && (verInfo_.dwMajorVersion == 6) && (verInfo_.dwMinorVersion == 0)) ? true : false; }; // Is this Windows Vista ?
	};
} // namespace

#endif // HDR_OSVERSION_H
