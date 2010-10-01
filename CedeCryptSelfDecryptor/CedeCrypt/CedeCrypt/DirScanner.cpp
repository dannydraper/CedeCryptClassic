#include "DirScanner.h"

DirScanner::DirScanner ()
{
	m_bUseDiagnostics = true;
}

DirScanner::~DirScanner ()
{
	
}

void DirScanner::ShowInt (int iInttoShow) {
	char szMsg[255];
	ZeroMemory (szMsg, 255);
	sprintf_s (szMsg, 255, "Value of int: %d", iInttoShow);
	MessageBox (NULL, szMsg, "ShowInt", MB_OK);
}

void DirScanner::SetDiagnostics (Diagnostics *pdiag)
{
	m_pdiag = pdiag;
}

// Given a source and dest path, copy the files to the dest that do not exist, but are present
// on the source.
void DirScanner::ListFiles (char *szSourcePath) {
	struct _finddata_t c_file;
	long hFile;

	char szSource[255];
	char szFilter[255];
	char szFullPath[1024];
	
	BOOL bExists;
	DynStringList dlSourceFiles;
	
	ZeroMemory (szSource, 255);
	ZeroMemory (szFilter, 255);
	ZeroMemory (szFullPath, 1024);
	
	strcpy (szSource, szSourcePath);
	strcpy (szFilter, "\\*.*");
	
	strcat (szFullPath, szSource);
	strcat (szFullPath, szFilter);
	
	OutputText ("SourcePath: ", szSourcePath);

	// Build the list of files in the source path
	if( (hFile = _findfirst(szFullPath, &c_file )) == -1L )
		OutputText ("No Files.");
	else
	{
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			dlSourceFiles.AddItem (c_file.name);
			OutputText (c_file.name);
		}
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) != 16) {
				dlSourceFiles.AddItem (c_file.name);
				OutputText (c_file.name);
			}
		}
		_findclose( hFile );
	}
	
	if (dlSourceFiles.GetNumItems () > 0) {
		dlSourceFiles.Clear ();
	}
}

// Directory recursion function. Get the next unscanned dir given by szPath
char *DirScanner::GetNextDir (char *szPath, char *szFilter) {
	
	struct _finddata_t c_file;
	long hFile;
	
	char szFullPath[1024];
	ZeroMemory (szFullPath, 1024);
	
	char szDirPath[1024];
	ZeroMemory (szDirPath, 1024);
	
	strcat (szFullPath, szPath);
	strcat (szFullPath, szFilter);
	
	if( (hFile = _findfirst(szFullPath, &c_file )) == -1L )
		return "";
	else
	{
		if ((c_file.attrib & _A_SUBDIR) == 16) {
			strcat (szDirPath, szPath);
			strcat (szDirPath, "\\");
			strcat (szDirPath, c_file.name);
			
			if (m_dlDoneDirs.DoesExist (szDirPath) == FALSE) {
				m_dlDoneDirs.AddItem (szDirPath);
				return c_file.name;
			}
		}
		
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) == 16) {
				ZeroMemory (szDirPath, 1024);
				strcat (szDirPath, szPath);
				strcat (szDirPath, "\\");
				strcat (szDirPath, c_file.name);
				
				if (m_dlDoneDirs.DoesExist (szDirPath) == FALSE) {
					m_dlDoneDirs.AddItem (szDirPath);
					return c_file.name;
				} 
				
			}
		}

		_findclose( hFile );
	}
	return "";
}

char *DirScanner::ScanNextSubDir () {
	int p;
	char szRetstring[1024];
	ZeroMemory (szRetstring, 1024);
	
	ZeroMemory (m_szCompleteDir, 1024);

	for (p=0;p<m_dlCurPath.GetNumItems ();p++) {
		strcat (m_szCompleteDir, m_dlCurPath.GetItem (p));
		strcat (m_szCompleteDir, "\\");
	}
	
	strcpy (szRetstring, m_szCompleteDir);
	
	ZeroMemory (m_szPassDir, 1024);
	strcpy (m_szPassDir, m_szCompleteDir);
	
	ZeroMemory (m_szCurrentDir, 1024);
	strcpy (m_szCurrentDir,GetNextDir (m_szPassDir, "*.*"));
	while (strcmp (m_szCurrentDir, ".") == 0 || strcmp (m_szCurrentDir, "..") == 0) {
		ZeroMemory (m_szCurrentDir, 1024);
		strcpy (m_szCurrentDir,GetNextDir (m_szPassDir, "*.*"));
	}
	
	if (strcmp (m_szCurrentDir, "") != 0) {	
		m_dlCurPath.AddItem (m_szCurrentDir);
	} else {
		if (m_dlCurPath.GetNumItems () > 0) {
			m_dlCurPath.RemoveLastItem ();
			ZeroMemory (m_szCurrentDir, 1024);
			ZeroMemory (m_szCompleteDir, 1024);
		}
	}
	
	return szRetstring;
}

void DirScanner::GetFullFolderList (char *szSrc) {
	int count = 0;
	ZeroMemory (m_szInitialDir, 255);
	ZeroMemory (m_szCurrentDir, 1024);
	ZeroMemory (m_szCompleteDir, 1024);
	ZeroMemory (m_szPassDir, 1024);
		
	strcpy (m_szInitialDir, szSrc);
	m_dlCurPath.AddItem (m_szInitialDir);
	
	char szSubDir[1024];
	ZeroMemory (szSubDir, 1024);
	
	char szMsg[92];
	ZeroMemory (szMsg, 92);

	strcpy (szSubDir, ScanNextSubDir ());
	
	
	while (strcmp (szSubDir, "") != 0) {
		
		if (m_dlDirs.DoesExist (szSubDir) == FALSE) {
												
			// Synchronise from Source to Dest
			ListFiles (szSubDir);
			m_dlDirs.AddItem (szSubDir);
		}
		
		strcpy (szSubDir, ScanNextSubDir ());
		count ++;

		if (count > 1000) {
			return;
		}
	}
	
	//OutputText (szSubDir);

	m_dlDoneDirs.Clear ();
	m_dlCurPath.Clear ();
	m_dlDirs.Clear ();
}

void DirScanner::ListAllFiles (char *szSourcePath, DynList *pPathList) {
	struct _finddata_t c_file;
	long hFile;

	char szSource[SIZE_STRING];
	char szFilter[SIZE_STRING];
	char szFullPath[SIZE_STRING];
	char szDirectoryPath[SIZE_STRING];
	CHAR szFullFilePath[SIZE_STRING];

	//BOOL bExists;
	//DynStringList dlSourceFiles;
	//pPathList->Clear ();

	ZeroMemory (szSource, SIZE_STRING);
	ZeroMemory (szFilter, SIZE_STRING);
	ZeroMemory (szFullPath, SIZE_STRING);
	
	if (strncmp (szSourcePath+strlen(szSourcePath)-1, "\\", 1) == 0) {
		strncpy_s (szSource, SIZE_STRING, szSourcePath, strlen(szSourcePath)-1);
	} else {
		strcpy_s (szSource, SIZE_STRING, szSourcePath);
	}

	strcpy (szFilter, "\\*.*");
	
	strcat (szFullPath, szSource);
	strcat (szFullPath, szFilter);
	
	//OutputText ("SourcePath: ", szSourcePath);

	// Build the list of files in the source path
	if( (hFile = _findfirst(szFullPath, &c_file )) == -1L )
		OutputText ("No Files.");
	else
	{
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			//dlSourceFiles.AddItem (c_file.name);
			//OutputText (c_file.name, ": AFILE" );

			ZeroMemory (szFullFilePath, SIZE_STRING);
			strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
			strcat_s (szFullFilePath, SIZE_STRING, "\\");
			strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
			//OutputText (szFullFilePath);
			pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);			

		} else {
			if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
				//OutputText (c_file.name, ": ADIRECTORY");

				ZeroMemory (szDirectoryPath, SIZE_STRING);
				strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
				strcat_s (szDirectoryPath, SIZE_STRING, "\\");
				strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
				ListAllFiles (szDirectoryPath, pPathList);
			}
		}
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) != 16) {			
				//OutputText (c_file.name, ": AFILE");

				ZeroMemory (szFullFilePath, SIZE_STRING);
				strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
				strcat_s (szFullFilePath, SIZE_STRING, "\\");
				strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
				//OutputText (szFullFilePath);
				pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);
			} else {
				if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
					//OutputText (c_file.name, ": ADIRECTORY");

					ZeroMemory (szDirectoryPath, SIZE_STRING);
					strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
					strcat_s (szDirectoryPath, SIZE_STRING, "\\");
					strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
					ListAllFiles (szDirectoryPath, pPathList);
				}
			}
		}
		_findclose( hFile );
	}
	
	//if (dlSourceFiles.GetNumItems () > 0) {
	//	dlSourceFiles.Clear ();
	//}
}

bool DirScanner::IsFileCCR (char *pszFilePath)
{
	char szFullPath[SIZE_STRING];
	ZeroMemory (szFullPath, SIZE_STRING);
	strcpy_s (szFullPath, SIZE_STRING, pszFilePath);

	char szTmp[SIZE_NAME];
	char szExtension[SIZE_NAME];

	int a = 0;
	int exloc = 0;
	bool bexfound = false;

	for (a=strlen(szFullPath);a>0;a--) {
		ZeroMemory (szTmp, SIZE_NAME);
		strncpy_s (szTmp, SIZE_NAME, szFullPath+a, 1);
		
		if (strcmp (szTmp, ".") == 0) {
			exloc = a;
			bexfound = true;
			break;
		}
	}

	if (bexfound == true) {
		ZeroMemory (szExtension, SIZE_NAME);
		strcpy_s (szExtension, SIZE_NAME, szFullPath+exloc);

		if (strcmp (szExtension, ".CCR") == 0 || strcmp (szExtension, ".ccr") == 0) {
			//OutputText ("Extension: ", szExtension);
			return true;
		} else {
			//OutputText ("No Extension: ", szExtension);
			return false;
		}
	} else {
		return false;
	}

	return false;
}

void DirScanner::ListAllCCRFiles (char *szSourcePath, DynList *pPathList) {

	struct _finddata_t c_file;
	long hFile;

	char szSource[SIZE_STRING];
	char szFilter[SIZE_STRING];
	char szFullPath[SIZE_STRING];
	char szDirectoryPath[SIZE_STRING];
	CHAR szFullFilePath[SIZE_STRING];

	//BOOL bExists;
	//DynStringList dlSourceFiles;
	//pPathList->Clear ();

	ZeroMemory (szSource, SIZE_STRING);
	ZeroMemory (szFilter, SIZE_STRING);
	ZeroMemory (szFullPath, SIZE_STRING);
	
	if (strncmp (szSourcePath+strlen(szSourcePath)-1, "\\", 1) == 0) {
		strncpy_s (szSource, SIZE_STRING, szSourcePath, strlen(szSourcePath)-1);
	} else {
		strcpy_s (szSource, SIZE_STRING, szSourcePath);
	}

	strcpy (szFilter, "\\*.*");
	
	strcat (szFullPath, szSource);
	strcat (szFullPath, szFilter);
	
	//OutputText ("SourcePath: ", szSourcePath);

	// Build the list of files in the source path
	if( (hFile = _findfirst(szFullPath, &c_file )) == -1L )
		OutputText ("No Files.");
	else
	{
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			//dlSourceFiles.AddItem (c_file.name);
			//OutputText (c_file.name, ": AFILE" );

			ZeroMemory (szFullFilePath, SIZE_STRING);
			strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
			strcat_s (szFullFilePath, SIZE_STRING, "\\");
			strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
			
			if (IsFileCCR (szFullFilePath) == true) {
				//OutputText (szFullFilePath);
				pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);			
			} else {
				//OutputText ("NOT CCR: ", szFullFilePath);
			}
		} else {
			if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
				//OutputText (c_file.name, ": ADIRECTORY");

				ZeroMemory (szDirectoryPath, SIZE_STRING);
				strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
				strcat_s (szDirectoryPath, SIZE_STRING, "\\");
				strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
				ListAllCCRFiles (szDirectoryPath, pPathList);
			}
		}
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) != 16) {			
				//OutputText (c_file.name, ": AFILE");

				ZeroMemory (szFullFilePath, SIZE_STRING);
				strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
				strcat_s (szFullFilePath, SIZE_STRING, "\\");
				strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
				
				if (IsFileCCR (szFullFilePath) == true) {
					//OutputText (szFullFilePath);
					pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);
				} else {
					//OutputText ("NOT CCR: ", szFullFilePath);
				}
				
			} else {
				if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
					//OutputText (c_file.name, ": ADIRECTORY");

					ZeroMemory (szDirectoryPath, SIZE_STRING);
					strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
					strcat_s (szDirectoryPath, SIZE_STRING, "\\");
					strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
					ListAllCCRFiles (szDirectoryPath, pPathList);
				}
			}
		}
		_findclose( hFile );
	}
	
	//if (dlSourceFiles.GetNumItems () > 0) {
	//	dlSourceFiles.Clear ();
	//}
}

void DirScanner::ListAllNONCCRFiles (char *szSourcePath, DynList *pPathList) {

	struct _finddata_t c_file;
	long hFile;

	char szSource[SIZE_STRING];
	char szFilter[SIZE_STRING];
	char szFullPath[SIZE_STRING];
	char szDirectoryPath[SIZE_STRING];
	CHAR szFullFilePath[SIZE_STRING];

	//BOOL bExists;
	//DynStringList dlSourceFiles;
	//pPathList->Clear ();

	ZeroMemory (szSource, SIZE_STRING);
	ZeroMemory (szFilter, SIZE_STRING);
	ZeroMemory (szFullPath, SIZE_STRING);
	
	if (strncmp (szSourcePath+strlen(szSourcePath)-1, "\\", 1) == 0) {
		strncpy_s (szSource, SIZE_STRING, szSourcePath, strlen(szSourcePath)-1);
	} else {
		strcpy_s (szSource, SIZE_STRING, szSourcePath);
	}

	strcpy (szFilter, "\\*.*");
	
	strcat (szFullPath, szSource);
	strcat (szFullPath, szFilter);
	
	//OutputText ("SourcePath: ", szSourcePath);

	// Build the list of files in the source path
	if( (hFile = _findfirst(szFullPath, &c_file )) == -1L )
		OutputText ("No Files.");
	else
	{
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			//dlSourceFiles.AddItem (c_file.name);
			//OutputText (c_file.name, ": AFILE" );

			ZeroMemory (szFullFilePath, SIZE_STRING);
			strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
			strcat_s (szFullFilePath, SIZE_STRING, "\\");
			strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
			
			if (IsFileCCR (szFullFilePath) == false) {
				//OutputText (szFullFilePath);
				pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);			
			} else {
				//OutputText ("NOT CCR: ", szFullFilePath);
			}
		} else {
			if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
				//OutputText (c_file.name, ": ADIRECTORY");

				ZeroMemory (szDirectoryPath, SIZE_STRING);
				strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
				strcat_s (szDirectoryPath, SIZE_STRING, "\\");
				strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
				ListAllNONCCRFiles (szDirectoryPath, pPathList);
			}
		}
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) != 16) {			
				//OutputText (c_file.name, ": AFILE");

				ZeroMemory (szFullFilePath, SIZE_STRING);
				strcpy_s (szFullFilePath, SIZE_STRING, szSourcePath);
				strcat_s (szFullFilePath, SIZE_STRING, "\\");
				strcat_s (szFullFilePath, SIZE_STRING, c_file.name);
				
				if (IsFileCCR (szFullFilePath) == false) {
					//OutputText (szFullFilePath);
					pPathList->AddItem (szFullFilePath, strlen (szFullFilePath), false);
				} else {
					//OutputText ("NOT CCR: ", szFullFilePath);
				}
				
			} else {
				if (strcmp (c_file.name, ".") != 0 && strcmp (c_file.name, "..") != 0) {
					//OutputText (c_file.name, ": ADIRECTORY");

					ZeroMemory (szDirectoryPath, SIZE_STRING);
					strcpy_s (szDirectoryPath, SIZE_STRING, szSourcePath);
					strcat_s (szDirectoryPath, SIZE_STRING, "\\");
					strcat_s (szDirectoryPath, SIZE_STRING, c_file.name);
					ListAllNONCCRFiles (szDirectoryPath, pPathList);
				}
			}
		}
		_findclose( hFile );
	}
	
	//if (dlSourceFiles.GetNumItems () > 0) {
	//	dlSourceFiles.Clear ();
	//}
}

void DirScanner::OutputInt (LPCSTR lpszText, int iValue)
{
	if (m_bUseDiagnostics == true) {
		m_pdiag->OutputInt (lpszText, iValue);
	}
}

void DirScanner::OutputText (LPCSTR lpszText)
{
	if (m_bUseDiagnostics == true) {
		m_pdiag->OutputText (lpszText);
	}
}

void DirScanner::OutputText (LPCSTR lpszName, LPCSTR lpszValue)
{
	if (m_bUseDiagnostics == true) {
		m_pdiag->OutputText (lpszName, lpszValue);
	}
}