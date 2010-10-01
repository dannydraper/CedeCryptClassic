#pragma once
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include "Diagnostics.h"
#include "DynList.h"
#include "DynStringList.h"

class DirScanner {
	
	public:
		DirScanner ();
		~DirScanner ();		
		void OutputInt (LPCSTR lpszText, int iValue);
		void OutputText (LPCSTR lpszText);
		void OutputText (LPCSTR lpszName, LPCSTR lpszValue);

		void SetDiagnostics (Diagnostics *pdiag);
		void GetFullFolderList (char *szSrc);

		//**************************************
		void ListAllFiles (char *szSourcePath, DynList *pPathList);
		void ListAllCCRFiles (char *szSourcePath, DynList *pPathList);
		void ListAllNONCCRFiles (char *szSourcePath, DynList *pPathList);
		bool IsFileCCR (char *pszFilePath);

	private:
		void ShowInt (int iInttoShow);
		void ListFiles (char *szSourcePath);
		char *GetNextDir (char *szPath, char *szFilter);
		char *ScanNextSubDir ();

		// Flag indicating if we're using diagnostics
		bool m_bUseDiagnostics;
		Diagnostics *m_pdiag;

		DynStringList m_dlDoneDirs;
		DynStringList m_dlCurPath;
		DynStringList m_dlDirs;

		char m_szCompleteDir[1024];
		char m_szPassDir[1024];
		char m_szCurrentDir[1024];
		char m_szInitialDir[255];



		//************************************


};
