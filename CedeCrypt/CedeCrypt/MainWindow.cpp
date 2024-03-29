// This work is dedicated to the Lord our God. King of Heaven, Lord of Heaven's Armies.

#include "MainWindow.h"

// Update history:
//23-March-2009:
// Added the ability to choose the different encryption algorithms. File format was modified to include
// an algorithm ID, and the SaveEncryptedFile () and DoEncryption () functions modified to handle the additiona
// field in the file. Also had to account for older file formats for previous versions of CedeCrypt.

//24-March-2009
// Moved the Initialisepackage for the Main UI to the ValidateInput function because this caused a problem
// when created the same encrypted package more than once. Nothing was getting added to the encrypted package
// because initialisepackage was only called once.
// Clear the progress bars on the main window when a single file or folder has been encrypted or decrypted
// When using the main window to create an Encrypted Package, and the user changes the destination, we need to check for
// and if necessary add the .exe extension.
// Check how the new file format with added AlgorithmID field will effect the encrypted package. Presumably the package
// viewer will need to be modified to handle the different algorithms too. - DONE


// Tasks Todo:


MainWindow::MainWindow ()
{
	m_checksum = 0;
	m_ccryptid = 477771549; // Latest version of the cedecrypt file format - included the algorithm type
	m_ccryptid2 = 477771545; // Older version of the cedecrypt file format - no canvas
	m_ccryptid3 = 477771547; // Version 2.40 of the CedeCrypt file format - included canvas backdoor
	m_ccryptidprev1 = 477771527;
	m_inputchecksum = 0;
	m_inputid = 0;
	m_iFilepointer = 0;
	m_bDeleteorig = false;
	m_bShellmode = false;
	m_bAwaitingcancel = false;
	m_bRecoverymode = false;
	m_bCommandmode = false;
	m_bCreatewindow = true;
	m_bSessionmode = false;
	m_bProtectedfilesexist = false;
	m_bUsesessionpassword = false;
	m_bSessionpasswordpresent = false;
	m_bDecryptionsessioncheck = false;
	m_bSessionshuttingdown = false;
	m_bShutdownencryptdone = false;
	m_sbcancelwaitdlg = false;
	m_btextmode = false;
	m_bCanvasPublicKeypresent = false;
	m_bcreatepackage = false;
	m_bCancelquestionshown = false;
	m_busingownpackagepath = false;
	m_busingtimings = true;
}

MainWindow::~MainWindow ()
{
}

bool MainWindow::ValidateOS ()
{
	OSVERSIONINFO osinfo;

	osinfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (GetVersionEx (&osinfo) != 0) {
		
		if (osinfo.dwMajorVersion >= 5 && osinfo.dwMinorVersion >= 1) {
			return true;
		} else {
			if (osinfo.dwMajorVersion >=6) {
				return true;
			} else {
				MessageBox (NULL, "You are running CedeCrypt on an unsupported Operating System. Supported operating systems include Windows XP, Windows Server 2003 & Windows Vista", "Unsupport OS", MB_OK | MB_ICONINFORMATION);
				return false;
			}
		}

	} else {
		return true;
	}
	
}

void MainWindow::ParseCommandLine (char *pszCmdline)
{
	//MessageBox (NULL, "Hello", "Info", MB_OK);
	//MessageBox (NULL, pszCmdline, "Command Line", MB_OK);

	char szAction[SIZE_STRING];
	ZeroMemory (szAction, SIZE_STRING);

	char szActionEx[SIZE_STRING];
	ZeroMemory (szActionEx, SIZE_STRING);

	char szExtendedAction[SIZE_STRING];
	ZeroMemory (szExtendedAction, SIZE_STRING);

	char szPath[SIZE_STRING];
	ZeroMemory (szPath, SIZE_STRING);

	strncpy_s (szAction, SIZE_STRING, pszCmdline, 2);
	strncpy_s (szActionEx, SIZE_STRING, pszCmdline, 3);
	strncpy_s (szExtendedAction, SIZE_STRING, pszCmdline, 5);

	if (strcmp (szAction, "/e") == 0) {
		//MessageBox (NULL, "Encrypt mode", "Info", MB_OK);

		if ((strlen (pszCmdline)-2) > 2) {
			m_pdiag->OutputInt ("CmdLine length: ", strlen (pszCmdline));
			//MessageBox (NULL, "Path not empty", "Info", MB_OK);
			
			strcpy_s (szPath, SIZE_STRING, pszCmdline+3);
			
			//MessageBox (NULL, szPath, "Info", MB_OK);

			strcpy_s (m_szInputfile, SIZE_STRING, szPath);
			strcpy_s (m_szOutputfile, SIZE_STRING, szPath);
			strcat_s (m_szOutputfile, SIZE_STRING, ".ccr");
			
			m_cmdmode = APP_ENCRYPT;
		} else {
			//MessageBox (NULL, "Path is NOTHING", "Info", MB_OK);
		}
	}

	if (strcmp (szAction, "/r") == 0) {
		// Recovery mode has been requested
		m_bRecoverymode = true;
	}

	if (strcmp (szActionEx, "/cr") == 0) {
		// Canvas recovery has been requested
		m_bRecoverymode = true;
		m_bCanvasRecoverymode = true;
	}

	if (strcmp (szAction, "/s") == 0) {
		// We've been asked to startup in session mode
		// in session mode we must first check the protected folders list file
		// the check each folder for ccr files, then if any ccr files need to be decrypted then
		// we show the reduced application window
		m_bSessionmode = true;

		if (CheckProtectedModeFiles (false) == true) {
			m_bDecryptionsessioncheck = true;
			m_bProtectedfilesexist = true;
		}
	}

	if (strcmp (szExtendedAction, "/diag") != 0) {
		if (strcmp (szAction, "/d") == 0) {
			//MessageBox (NULL, "Command Mode Activated", "Test", MB_OK);
			m_bCommandmode = true;
			ZeroMemory (m_szCommandPath, SIZE_STRING);
			strcpy_s (m_szCommandPath, SIZE_STRING, pszCmdline+3);
			//if (strlen (m_szCommandPath) > 0) {
				//MessageBox (NULL, m_szCommandPath, "Command Path", MB_OK);
			//}
		}
	}

	if (strcmp (szAction, "/i") == 0) {
		CreateShellExtensionPath ();
		CreateFileAssociation ();
		m_bCreatewindow = false;
		PostQuitMessage (0);
	}

	if (strcmp (szAction, "/u") == 0) {
		RemoveFileAssociation ();
		m_bCreatewindow = false;
		PostQuitMessage (0);
	}
}

void MainWindow::Initialise (HWND hWnd, LPSTR lpCmdLine)
{	
	SetParentHWND (hWnd);
	//m_hwnd = hWnd;
	SetBgColor (GetSysColor (COLOR_BTNFACE));
	SetCaption (TEXT ("CedeCrypt v2.43"));

	ZeroMemory (m_szWindowcomment, SIZE_STRING);
	strcpy_s (m_szWindowcomment, SIZE_STRING, "");
	
	UpdateWindowCaption ();
	SetWindowStyle (FS_STYLESTANDARD);

	ParseCommandLine (lpCmdLine);

	if (m_bCreatewindow == true) {
		if (DoesShellCommandExist () == true) {
			m_bShellmode = true;
			CreateAppWindow ("CRYPTWindowClass", 70, 0, 448, 220, false);
		} else {
			if (m_bCommandmode == true) {
				m_bShellmode = false;
				CreateAppWindow ("CRYPTWindowClass", 70, 0, 448, 220, false);
			} else {
				if (m_bSessionmode == true) {
					m_bShellmode = false;
					m_bCommandmode = false;
					CreateAppWindow ("CRYPTWindowClass", 70, 0, 448, 220, false);
				} else {
					m_bShellmode = false;
					CreateAppWindow ("CRYPTWindowClass", 70, 0, 610, 515, false);
				}
			}		
		}

		m_uihandler.SetWindowProperties (0, 0, 0, 443, GetSysColor (COLOR_BTNFACE));
		SetWindowPosition (FS_CENTER);
		
		if (m_bSessionmode == true) {
			if (m_bProtectedfilesexist == true) {
				Show ();
			}
		} else {
			Show ();
		}
		
	}	
}

BOOL MainWindow::MySystemShutdown()
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp;
	
	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	return( FALSE ); 
	
	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,  &tkp.Privileges[0].Luid); 
	
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	
	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
	
	if (GetLastError() != ERROR_SUCCESS) 
	return FALSE; 
	
	// Shut down and poweroff the system
	if (!ExitWindowsEx(EWX_POWEROFF, 0))
	return FALSE; 
	
	return TRUE;
}


void MainWindow::UpdateWindowCaption ()
{
	ZeroMemory (m_szWindowcaption, SIZE_STRING);
	strcpy_s (m_szWindowcaption, SIZE_STRING, "CedeCrypt v2.43 ");
	strcat_s (m_szWindowcaption, SIZE_STRING, m_szWindowcomment);	
	SetTimer (m_hwnd, IDT_UPDATECAPTION, 2000, NULL);
}

void MainWindow::SetDiagnostics (Diagnostics *pdiag)
{
	m_pdiag = pdiag;
	m_crypt.SetDiagnostics (pdiag);
	m_Dirscanner.SetDiagnostics (pdiag);
	m_fileshred.SetDiagnostics (pdiag);
}
	
void MainWindow::OnCreate (HWND hWnd)
{
	m_hwnd = hWnd;

	int ioffset1  = 0;
	int xoffset = 160;

	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		ioffset1 = 65;
	} else {
		ioffset1 = 11;
	}

	m_crypt.SetAlgorithm (ALG_CYBERCEDE); // The default algorithm

	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		m_header.SetBitmapResources (IDB_HEADER);
		m_header.SetBitmapProperties (0, 0, 443, 44);
		m_header.SetProperties (hWnd, CID_HEADER, 0, 0, 443, 44);
		m_uihandler.AddDirectControl (&m_header);
	} else {
		m_header.SetBitmapResources (IDB_SIDEBANNER);
		m_header.SetBitmapProperties (0, 0, 161, 475);
		m_header.SetProperties (hWnd, CID_HEADER, 0, 0, 161, 475);
		m_uihandler.AddDirectControl (&m_header);
	}
	
	
	m_mainmenu.CreateMainMenu (hWnd, m_bRecoverymode);

	m_pfolders.SetDiagnostics (m_pdiag);
	
	//
	//

	// The group by box	
	HFONT hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

	m_hwndgroup1 = CreateWindow ("button", "Encryption / Decryption", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12+xoffset, 5, 419, 57, hWnd, (HMENU) ID_GROUPBOX1, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndgroup1, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));


	// The encrypt to package option
	m_hwndoptpackage = CreateWindow ("button", "Encrypt to Self-Decrypting package", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 30+xoffset, 30, 200, 16, hWnd, (HMENU) ID_OPTENCRYPTPACKAGE, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndoptpackage, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The encrypt option
	m_hwndoptencrypt = CreateWindow ("button", "Encrypt", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 250+xoffset, 30, 55, 16, hWnd, (HMENU) ID_OPTENCRYPT, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndoptencrypt, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The decrypt option
	m_hwndoptdecrypt = CreateWindow ("button", "Decrypt", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 340+xoffset, 30, 60, 16, hWnd, (HMENU) ID_OPTDECRYPT, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndoptdecrypt, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	m_hwndgroup = CreateWindow ("button", "Encrypt file or folder?", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12+xoffset, 57+ioffset1, 419, 50, hWnd, (HMENU) ID_GROUPBOX, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndgroup, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The single file option
	m_hwndoptfile = CreateWindow ("button", "Single File", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 100+xoffset, 77+ioffset1, 100, 16, hWnd, (HMENU) ID_OPTFILE, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndoptfile, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The entire folder option
	m_hwndoptfolder = CreateWindow ("button", "Entire Folder", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 270+xoffset, 77+ioffset1, 100, 16, hWnd, (HMENU) ID_OPTFOLDER, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndoptfolder, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The second group box
	m_hwndgroup2 = CreateWindow ("button", "Source and Destination Files", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12+xoffset, 114+ioffset1, 419, 130, hWnd, (HMENU) ID_GROUPBOX2, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndgroup2, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Source edit box
	m_hwndeditsource = CreateWindow ("edit", "<No Source Selected>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT | ES_AUTOHSCROLL, 25+xoffset, 135+ioffset1, 310, 19, hWnd, (HMENU) ID_EDITSOURCE, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndeditsource, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Destination edit box
	m_hwndeditdest = CreateWindow ("edit", "<No Destination Selected>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT | ES_AUTOHSCROLL, 25+xoffset, 165+ioffset1, 310, 19, hWnd, (HMENU) ID_EDITDEST, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndeditdest, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Delete original files check box
	m_chkkeepopen = CreateWindow ("button", "Delete original files after encryption/decryption", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 28+xoffset, 210+ioffset1, 240, 19, hWnd, (HMENU) ID_CHKDELETEORIG, GetModuleHandle (NULL), NULL);
	SendMessage (m_chkkeepopen, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Begin button
	m_hwndbtnbegin = CreateWindow ("button", "Begin", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 345+xoffset, 210+ioffset1, 70, 23, hWnd, (HMENU) ID_BTNBEGIN, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndbtnbegin, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));	

	// Source browse button
	m_hwndbtnsrcbrowse = CreateWindow ("button", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 345+xoffset, 135+ioffset1, 70, 19, hWnd, (HMENU) ID_BTNSRCBROWSE, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndbtnsrcbrowse, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Destination browse button
	m_hwndbtndstbrowse = CreateWindow ("button", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 345+xoffset, 165+ioffset1, 70, 19, hWnd, (HMENU) ID_BTNDSTBROWSE, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndbtndstbrowse, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The third group box
	m_hwndgroup3 = CreateWindow ("button", "Status", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12+xoffset, 251+ioffset1, 419, 105, hWnd, (HMENU) ID_GROUPBOX3, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndgroup3, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	
	// The exit button
	m_btncancel = CreateWindow ("button", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 357, 75+ioffset1, 70, 23, hWnd, (HMENU) ID_BTNCANCEL, GetModuleHandle (NULL), NULL);
	SendMessage (m_btncancel, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The cancel button
	m_hwndbtnexit = CreateWindow ("button", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 361+xoffset, 420+ioffset1, 70, 23, hWnd, (HMENU) ID_BTNEXIT, GetModuleHandle (NULL), NULL);
	SendMessage (m_hwndbtnexit, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The status list box
	m_hwndlststatus = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 25+xoffset, 270+ioffset1, 392, 90, hWnd, (HMENU) ID_LSTSTATUS, GetModuleHandle (NULL), NULL) ;
	SendMessage (m_hwndlststatus, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// Set some initial options
	SendMessage (m_hwndoptfile, BM_SETCHECK, 1, 0);
	SendMessage (m_hwndoptfolder, BM_SETCHECK, 0, 0);
	m_filemode = MODE_FILE;

	SendMessage (m_hwndoptencrypt, BM_SETCHECK, 1, 0);
	SendMessage (m_hwndoptdecrypt, BM_SETCHECK, 0, 0);
	m_appmode = MODE_ENCRYPT;

	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		m_hwndphase = CreateWindow ("static", "", WS_CHILD | WS_VISIBLE, 12, 24+ioffset1, 270, 15, hWnd, (HMENU) ID_LBLPHASE, GetModuleHandle (NULL), NULL);
		m_hwndlbltotal = CreateWindow ("static", "", WS_CHILD | WS_VISIBLE, 12, 49, 415, 15, hWnd, (HMENU) ID_LBLTOTAL, GetModuleHandle (NULL), NULL);
	} else {
		m_hwndphase = CreateWindow ("static", "", WS_CHILD | WS_VISIBLE, 12+xoffset, 405+ioffset1, 270, 15, hWnd, (HMENU) ID_LBLPHASE, GetModuleHandle (NULL), NULL);
		m_hwndlbltotal = CreateWindow ("static", "", WS_CHILD | WS_VISIBLE, 12+xoffset, 363+ioffset1, 415, 15, hWnd, (HMENU) ID_LBLTOTAL, GetModuleHandle (NULL), NULL);
	}
	
	SendMessage (m_hwndphase, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	SendMessage (m_hwndlbltotal, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	// The progress bar
	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		m_hwndprogress  = CreateWindowEx(0L, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 12, 40+ioffset1, 415, 22, hWnd, (HMENU) ID_PROGRESS, GetModuleHandle (NULL), NULL);
	} else {
		m_hwndprogress  = CreateWindowEx(0L, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 12+xoffset, 420+ioffset1, 340, 22, hWnd, (HMENU) ID_PROGRESS, GetModuleHandle (NULL), NULL);
	}

	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		m_hwndtotalprogress  = CreateWindowEx(0L, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 12, ioffset1, 415, 22, hWnd, (HMENU) ID_TOTALPROGRESS, GetModuleHandle (NULL), NULL);
	} else {
		m_hwndtotalprogress  = CreateWindowEx(0L, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 12+xoffset, 380+ioffset1, 340, 22, hWnd, (HMENU) ID_TOTALPROGRESS, GetModuleHandle (NULL), NULL);
	}

	SendMessage(m_hwndprogress, PBM_SETRANGE, 0L, MAKELONG (0, 80));
	SendMessage (m_hwndprogress, PBM_SETSTEP, 1, 0L);
	SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);

	// Initialise the random buffer in the file shredder
	m_fileshred.InitRandom ();

	// Here we need to check if a public key exists in the canvas, and read it into memory
	if (GetKeyfromCanvas () == true) {
		m_pdiag->OutputText ("Canvas public key ok.");
	} else {
		m_pdiag->OutputText ("Canvas public key not present.");
	}

	if (GetDistributionfromCanvas () == true) {
		m_pdiag->OutputText ("Distribution name read ok.");
	} else {
		m_pdiag->OutputText ("Distribution name not found. Default has been used.");
	}

	// Add some pretty application text
	OutputStatusText ("CedeCrypt Initialised and Ready.");

	if (m_cmdmode == APP_ENCRYPT) {
		SetDlgItemText (hWnd, ID_EDITSOURCE, m_szInputfile);
		SetDlgItemText (hWnd, ID_EDITDEST, m_szOutputfile);
		//ValidateInput ();
		PostMessage (hWnd, WM_COMMAND, ID_BTNBEGIN, 0);
	}

	// Now check if we're in shell mode, because then we need to hide all of the standard controls
	if (m_bShellmode == true || m_bCommandmode == true || m_bSessionmode == true) {
		ShowWindow (m_hwndgroup1, SW_HIDE);
		ShowWindow (m_hwndoptencrypt, SW_HIDE);
		ShowWindow (m_hwndoptdecrypt, SW_HIDE);
		ShowWindow (m_hwndoptpackage, SW_HIDE);
		ShowWindow (m_hwndgroup, SW_HIDE);
		ShowWindow (m_hwndoptfile, SW_HIDE);
		ShowWindow (m_hwndoptfolder, SW_HIDE);
		ShowWindow (m_hwndgroup2, SW_HIDE);
		ShowWindow (m_hwndeditsource, SW_HIDE);
		ShowWindow (m_hwndeditdest, SW_HIDE);
		ShowWindow (m_chkkeepopen, SW_HIDE);
		ShowWindow (m_hwndbtnbegin, SW_HIDE);
		ShowWindow (m_hwndbtnsrcbrowse, SW_HIDE);
		ShowWindow (m_hwndbtndstbrowse, SW_HIDE);
		ShowWindow (m_hwndgroup3, SW_HIDE);
		ShowWindow (m_hwndbtnexit, SW_HIDE);
		ShowWindow (m_hwndlststatus, SW_HIDE);
		ShowWindow (m_btncancel, SW_SHOW);
		
		//CheckShellCommand ();
	} else {
		ShowWindow (m_btncancel, SW_HIDE);
	}

	if (m_license.CheckDLLPath () == false) {
		MessageBox (NULL, "Unable to load critical file CCrypt.DLL. Application will exit.", "CedeCrypt Load Error", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage (0);
	}

	ZeroMemory (m_szUserlicense, SIZE_STRING);
	m_bvalidlicenseinitialised = false;
	bool bOkinit = true;
	EnableWindow (hWnd, false);

	if (m_license.UserKeyPresent () == false) { // Open up the CCrypt.DLL and check that the License key is physically there, byte by byte
		m_pdiag->OutputText ("No license present, requesting license...");
		ShowLicensePrompt ();

		// If no license is present then default to a trial key


	} else {
		
		m_license.LoadUserKey (m_szUserlicense); // Load the user key into the supplied string, but first decoding the bytes. The bytes are stored encoded.

		if (m_license.ValidateUserKey (m_szUserlicense) == true) { // This essentially deserializes the user key and retrieves each field from the key
			
			if (m_license.MachineMatch () == true) { // This keys that the unique machine id matches the one stored in the dll
				m_pdiag->OutputText ("License was valid...");
				if (strcmp (m_license.GetUniqueProductID (), "11977557777777") == 0) {
															
					if (m_license.GetTrialMode () == true) {
						
						m_pdiag->OutputText ("License Type: Trial/Evaluation");
						m_pdiag->OutputInt ("Evaluation Length (days): ", m_license.GetTrialDays ());
						
						m_pdiag->OutputText ("This CedeCrypt software is a Trial Version Only,");
						m_pdiag->OutputText ("To purchase the full product with ");
						m_pdiag->OutputText ("no limitations and enhanced features please visit ");
						m_pdiag->OutputText ("http://www.cedesoft.com ");

						m_license.CheckInstEx (); // This checks if this is the first time the app has been licensed

						if (m_license.GetFirstInst () == false) { // Check if this is not the first installation
							if (m_license.TrialValid () != 0) { // Check if the trial is still ok, and the user hasn't put the clock back.
								bOkinit = false;
							}
						}

						m_pdiag->OutputInt ("Number of Days left: ",  m_license.GetTrialDays () - m_license.GetDaysInTrial ());

						if (bOkinit == true) {
							EnableWindow (m_hwnd, true);
							m_bvalidlicenseinitialised = true;
							m_pdiag->OutputText ("Trial license is still valid...");
							sprintf_s (m_szWindowcomment, SIZE_STRING, "(Trial Version - %d days remaining)", (m_license.GetTrialDays () - m_license.GetDaysInTrial ()));

							if (m_bShellmode == true) {
								PostMessage (hWnd, WM_UICOMMAND, CID_CHECKSHELL, 0);
							}

							if (m_bCommandmode == true) {
								PostMessage (hWnd, WM_UICOMMAND, CID_CHECKCOMMAND, 0);
							}

							if (m_bSessionmode == true) {
								if (m_bProtectedfilesexist == true) {
									PostMessage (hWnd, WM_UICOMMAND, CID_SESSIONDECRYPT, 0);
								}
							}

						} else {
							m_pdiag->OutputText ("Trial license has expired...");
							ShowLicensePrompt ();
							sprintf_s (m_szWindowcomment, SIZE_STRING, "(Trial Expired)");
						}

					} else {
						m_pdiag->OutputText ("License Type: Full/Unlimited");
						sprintf_s (m_szWindowcomment, SIZE_STRING, "(Registered to %s)", m_license.GetOwner ());
						EnableWindow (m_hwnd, true);
						m_bvalidlicenseinitialised = true;

						if (m_bShellmode == true) {
							PostMessage (hWnd, WM_UICOMMAND, CID_CHECKSHELL, 0);
						}

						if (m_bCommandmode == true) {
							PostMessage (hWnd, WM_UICOMMAND, CID_CHECKCOMMAND, 0);
						}

						if (m_bSessionmode == true) {
							if (m_bProtectedfilesexist == true) {
								PostMessage (hWnd, WM_UICOMMAND, CID_SESSIONDECRYPT, 0);
							}
						}
					}															

				} else {
					m_pdiag->OutputText ("License was for a different product, requesting license...");
					ShowLicensePrompt ();
				}
			} else {

				// Machine key was not valid, show license prompt.
				// Most likely the program dir was copied to another machine without
				// being properly licensed.
				m_pdiag->OutputText ("Machine key not valid, requesting license...");
				ShowLicensePrompt ();
			}

		} else {

			// License was not valid, show license prompt.
			// an attempt at license tampering was made, show license.
			m_pdiag->OutputText ("Possible license tampering, requesting license...");
			ShowLicensePrompt ();
		}
	}

	UpdateWindowCaption ();
	CreateShellExtensionPath (); // Reset the shell extension path in the registry


	// Diagnostics only - quickly check a file on the d drive
	// for dev actions like encryption and decryption but bypassing windows triggers
	// for shutdown/standby and hibernation
	//if (m_bSessionmode == true) {
	//	SetTimer (hWnd, IDT_ACTIONCHECK, 2000, NULL);
	//}
	
	// Read the hotkey registry settings, and register the users preferred hotkey
	// Use CTRL tickbox
	bool bencusectrl = false;
	bool bencusealt = false;
	bool bdecusectrl = false;
	bool bdecusealt = false;

	char szEnchotkey[SIZE_NAME];
	ZeroMemory (szEnchotkey, SIZE_NAME);

	char szDechotkey[SIZE_NAME];
	ZeroMemory (szDechotkey, SIZE_NAME);

	if (m_registry.DoesSettingExist (UTEENCRYPTUSECTRL) == true) {
		if (strcmp (m_registry.ReadStringSetting (UTEENCRYPTUSECTRL), "yes") == 0) {
			bencusectrl = true;
		} else {
			bencusectrl = false;
		}
	} else {
		bencusectrl = true; // Default Value
	}

	// Use ALT tickbox
	if (m_registry.DoesSettingExist (UTEENCRYPTUSEALT) == true) {
		if (strcmp (m_registry.ReadStringSetting (UTEENCRYPTUSEALT), "yes") == 0) {
			bencusealt = true;
		} else {
			bencusealt = false;
		}
	} else {
		bencusealt = false; // Default Value
	}

	// Encrypt HotKey
	if (m_registry.DoesSettingExist (UTEENCRYPTHOTKEY) == true) {
		strcpy_s (szEnchotkey, SIZE_NAME, m_registry.ReadStringSetting (UTEENCRYPTHOTKEY));
	} else {
		strcpy_s (szEnchotkey, SIZE_NAME, "E"); // Default value
	}

	// Decryption Hotkey registry settings
	// Use CTRL tickbox
	if (m_registry.DoesSettingExist (UTEDECRYPTUSECTRL) == true) {
		if (strcmp (m_registry.ReadStringSetting (UTEDECRYPTUSECTRL), "yes") == 0) {
			bdecusectrl = true;
		} else {
			bdecusectrl = false;
		}
	} else {
		 bdecusectrl = true; // Default Value
	}

	// Use ALT tickbox
	if (m_registry.DoesSettingExist (UTEDECRYPTUSEALT) == true) {
		if (strcmp (m_registry.ReadStringSetting (UTEDECRYPTUSEALT), "yes") == 0) {
			bdecusealt = true;
		} else {
			bdecusealt = false;
		}
	} else {
		bdecusealt = false; // Default Value
	}

	// Decrypt HotKey
	if (m_registry.DoesSettingExist (UTEDECRYPTHOTKEY) == true) {
		strcpy_s (szDechotkey, SIZE_NAME, m_registry.ReadStringSetting (UTEDECRYPTHOTKEY));
	} else {
		strcpy_s (szDechotkey, SIZE_NAME, "D"); // Default value
	}

	
	// Register a system hot key - ENCRYPT
	RegisterEncryptionHotkey (bencusectrl, bencusealt, szEnchotkey);
	RegisterDecryptionHotkey (bdecusectrl, bdecusealt, szDechotkey);

	/*
	if (RegisterHotKey (hWnd, 1234, MOD_CONTROL, 69) != 0) {
		m_pdiag->OutputText ("Encryption Hot key registered ok.");
	} else {
		m_pdiag->OutputText ("Encryption Hot key registration failed.");
	}

	// Register a system hot key - DECRYPT
	if (RegisterHotKey (hWnd, 1235, MOD_CONTROL, 68) != 0) {
		m_pdiag->OutputText ("Decryption Hot key registered ok.");
	} else {
		m_pdiag->OutputText ("Decryption Hot key registration failed.");
	}
	*/
}

void MainWindow::RegisterEncryptionHotkey (bool bUseCtrl, bool bUseAlt, LPCSTR szHotkey)
{
	unsigned int vkcode = GetVirtualKeyCode ((char *) szHotkey);

	if (vkcode > 0) {
		if (bUseCtrl == true && bUseAlt == true) {
			if (RegisterHotKey (m_hwnd, 1234, MOD_CONTROL | MOD_ALT, vkcode) != 0) {
				m_pdiag->OutputText ("Encryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Encryption Hot key registration failed.");
			}
		}

		if (bUseCtrl == true && bUseAlt == false) {
			if (RegisterHotKey (m_hwnd, 1234, MOD_CONTROL, vkcode) != 0) {
				m_pdiag->OutputText ("Encryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Encryption Hot key registration failed.");
			}
		}

		if (bUseCtrl == false && bUseAlt == true) {
			if (RegisterHotKey (m_hwnd, 1234, MOD_ALT, vkcode) != 0) {
				m_pdiag->OutputText ("Encryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Encryption Hot key registration failed.");
			}
		}	
	} else {
		m_pdiag->OutputText ("Encryption Hot key registration failed - unable to find virtual key code for specified string");
	}
}

void MainWindow::RegisterDecryptionHotkey (bool bUseCtrl, bool bUseAlt, LPCSTR szHotkey)
{
	unsigned int vkcode = GetVirtualKeyCode ((char *) szHotkey);

	if (vkcode > 0) {
		if (bUseCtrl == true && bUseAlt == true) {
			if (RegisterHotKey (m_hwnd, 1235, MOD_CONTROL | MOD_ALT, vkcode) != 0) {
				m_pdiag->OutputText ("Decryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Decryption Hot key registration failed.");
			}
		}

		if (bUseCtrl == true && bUseAlt == false) {
			if (RegisterHotKey (m_hwnd, 1235, MOD_CONTROL, vkcode) != 0) {
				m_pdiag->OutputText ("Decryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Decryption Hot key registration failed.");
			}
		}

		if (bUseCtrl == false && bUseAlt == true) {
			if (RegisterHotKey (m_hwnd, 1235, MOD_ALT, vkcode) != 0) {
				m_pdiag->OutputText ("Decryption Hot key registered ok.");
			} else {
				m_pdiag->OutputText ("Decryption Hot key registration failed.");
			}
		}	
	} else {
		m_pdiag->OutputText ("Decryption Hot key registration failed - unable to find virtual key code for specified string");
	}
}

void MainWindow::OutputStatusInt (LPCSTR lpszText, int iValue)
{
	char szInteger[SIZE_INTEGER];
	ZeroMemory (szInteger, SIZE_INTEGER);
	sprintf_s (szInteger, SIZE_INTEGER, "%d", iValue);

	char szText[SIZE_STRING];
	ZeroMemory (szText, SIZE_STRING);
	strcpy_s (szText, SIZE_STRING, lpszText);

	strcat_s (szText, SIZE_STRING, szInteger);

	SendMessage (m_hwndlststatus, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwndlststatus, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwndlststatus, LB_SETCURSEL, lCount-1, 0);
}

void MainWindow::OutputStatusText (LPCSTR lpszText)
{
	char szText[SIZE_STRING];
	ZeroMemory (szText, SIZE_STRING);
	strcpy_s (szText, SIZE_STRING, lpszText);
	SendMessage (m_hwndlststatus, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwndlststatus, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwndlststatus, LB_SETCURSEL, lCount-1, 0);
}

void MainWindow::OutputStatusText (LPCSTR lpszName, LPCSTR lpszValue)
{
	char szText[SIZE_STRING*2];
	ZeroMemory (szText, SIZE_STRING*2);	
	strcat_s (szText, SIZE_STRING*2, lpszName);
	strcat_s (szText, SIZE_STRING*2, lpszValue);

	SendMessage (m_hwndlststatus, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwndlststatus, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwndlststatus, LB_SETCURSEL, lCount-1, 0);
}

void MainWindow::ShowLicensePrompt ()
{
	m_licensewindow.SetDiagnostics (m_pdiag);
	m_licensewindow.Initialise (m_hwnd, 0);
	m_licensewindow.SetAlwaysOnTop (true);
}

bool MainWindow::OpenPrivateKey ()
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory (m_szPrivateKey, SIZE_STRING);
	ZeroMemory (m_szPrivateKeyTitle, SIZE_STRING);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szPrivateKey;
	ofn.lpstrFileTitle = m_szPrivateKeyTitle;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;
	ofn.nMaxFileTitle = SIZE_STRING;
	ofn.lpstrFilter = "Private Key File (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";

	//ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;	
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetOpenFileName (&ofn) != 0) {
		//m_pdiag->OutputText (m_szInputfiletitle);
		return true;

	} else {
		return false;
	}
}

bool MainWindow::OpenSingleFile ()
{
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory (m_szInputfile, SIZE_STRING);
	ZeroMemory (m_szInputfiletitle, SIZE_STRING);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szInputfile;
	ofn.lpstrFileTitle = m_szInputfiletitle;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;
	ofn.nMaxFileTitle = SIZE_STRING;

	if (m_appmode == MODE_DECRYPT) {
		ofn.lpstrFilter = "CedeCrypt Encoded File (*.ccr)\0*.ccr\0All Files (*.*)\0*.*\0";
	}
	
	if (m_appmode == MODE_ENCRYPT) {
		ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	}

	//ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;	
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetOpenFileName (&ofn) != 0) {
		//m_pdiag->OutputText (m_szInputfiletitle);
		return true;

	} else {
		return false;
	}
}

void MainWindow::BrowseSource ()
{
	if (m_filemode == MODE_FILE) {
		if (OpenSingleFile () == true) {
			char szHomedrive[SIZE_STRING];
			char szOutputPath[SIZE_STRING];
			ZeroMemory (szOutputPath, SIZE_STRING);
			
			// Set the source edit text
			SetDlgItemText (m_hwnd, ID_EDITSOURCE, m_szInputfile);

			// Set the destination edit text
			if (m_appmode == MODE_ENCRYPT) {
				if (m_bcreatepackage == false) {
					strcpy_s (szOutputPath, SIZE_STRING, m_szInputfile);
					strcat_s (szOutputPath, SIZE_STRING, ".ccr");
					SetDlgItemText (m_hwnd, ID_EDITDEST, szOutputPath);
				} else {
					ZeroMemory (szHomedrive, SIZE_STRING);
					GetEnvironmentVariable ("USERPROFILE", szHomedrive, SIZE_STRING);						
					strcat_s (szHomedrive, SIZE_STRING, "\\Desktop\\EncryptedPackages");						
					strcat_s (szHomedrive, SIZE_STRING, "\\EncryptedPackage.exe");
					SetDlgItemText (m_hwnd, ID_EDITDEST, szHomedrive);
				}
			}
			
			if (m_appmode == MODE_DECRYPT) {
				if (strncmp (m_szInputfile+strlen(m_szInputfile)-4, ".ccr", 4) == 0) {
					strncpy_s (szOutputPath, SIZE_STRING, m_szInputfile, strlen(m_szInputfile)-4);
					SetDlgItemText (m_hwnd, ID_EDITDEST, szOutputPath);
				}
			}
		}
	}

	if (m_filemode == MODE_FOLDER) {
		BrowseForFolder ("Select a Source Folder...", m_szInputfolder);
		
		if (strlen (m_szInputfolder) > 0) {
			SetDlgItemText (m_hwnd, ID_EDITSOURCE, m_szInputfolder);
		}
	}
}

void MainWindow::BrowseDestination ()
{
	if (m_filemode == MODE_FILE) {
		if (SaveSingleFile () == true) {
			if (m_bcreatepackage == true) {
				if (strncmp (m_szOutputfile+strlen(m_szOutputfile)-4, ".exe", 4) != 0) {
					if (strncmp (m_szOutputfile+strlen(m_szOutputfile)-4, ".EXE", 4) != 0) {
						strcat_s (m_szOutputfile, SIZE_STRING, ".exe");
					}					
				}
			}

			SetDlgItemText (m_hwnd, ID_EDITDEST, m_szOutputfile);
		}
	}
	
	if (m_filemode == MODE_FOLDER) {
		BrowseForFolder ("Select a Destination Folder...", m_szOutputfile);
		
		if (strlen (m_szOutputfile) > 0) {
			SetDlgItemText (m_hwnd, ID_EDITDEST, m_szOutputfile);
		}
	}
}

bool MainWindow::SaveSingleFile ()
{
	ZeroMemory (m_szOutputfile, SIZE_STRING);

	char szFileonly[SIZE_STRING];
	ZeroMemory (szFileonly, SIZE_STRING);

	char szPathonly[SIZE_STRING];
	ZeroMemory (szPathonly, SIZE_STRING);

	strcpy_s (szFileonly, SIZE_STRING, m_szInputfiletitle);

	strncpy_s (szPathonly, SIZE_STRING, m_szInputfile, strlen(m_szInputfile)-strlen(szFileonly));

	if (m_appmode == MODE_ENCRYPT) {
		if (m_bcreatepackage == false) {
			strcat_s (szFileonly, SIZE_STRING, ".ccr");
			strcpy_s (m_szOutputfile, SIZE_STRING, szFileonly);
		} else {
			ZeroMemory (szFileonly, SIZE_STRING);
			strcpy_s (szFileonly, SIZE_STRING, "EncryptedPackage.exe");
			strcpy_s (m_szOutputfile, SIZE_STRING, szFileonly);
		}
	}

	if (m_appmode == MODE_DECRYPT) {
		if (strncmp (szFileonly+strlen(szFileonly)-4, ".ccr", 4) == 0) {
			strncpy_s (m_szOutputfile, SIZE_STRING, szFileonly, strlen(szFileonly)-4);			
		}
	}

	m_pdiag->OutputText (szPathonly);

	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szOutputfile;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;

	if (m_appmode == MODE_ENCRYPT) {
		if (m_bcreatepackage == false) {
			ofn.lpstrFilter = "CedeCrypt Encoded File (*.ccr)\0*.ccr\0All Files (*.*)\0*.*\0";
		} else {
			ofn.lpstrFilter = "Self-Decrypting Package (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
		}
	}
	
	if (m_appmode == MODE_DECRYPT) {
		ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	}

	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileonly;
	ofn.nMaxFileTitle = SIZE_STRING;
	ofn.lpstrInitialDir = szPathonly;	
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	m_pdiag->OutputText ("SSF: szFileonly: ", szFileonly);
	m_pdiag->OutputText ("SSF: szPathonly: ", szPathonly);

	if (GetSaveFileName (&ofn) != 0) {
		m_busingownpackagepath = true;
		return true;
	} else {
		return false;
	}
}

bool MainWindow::BrowseForFolder (LPCTSTR szTitle, char *szOutPath) {
	CoInitialize (NULL);
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_USENEWUI;
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 ) {
		// get the name of the folder
		SHGetPathFromIDList (pidl, szOutPath);
		// free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) ) {
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}
		return true;
	}
	return false;
}

bool MainWindow::DoesFileExist (char *szFilepath, char *szFilename) 
{
	char szFoundfile[SIZE_STRING];
	ZeroMemory (szFoundfile, SIZE_STRING);

	DWORD dwRes = 0;

	dwRes = SearchPath (szFilepath, szFilename, NULL, SIZE_STRING, szFoundfile, NULL);

	if (dwRes == 0) {
		return false;
	} else {
		return true;
	}
}

void MainWindow::ValidateInput ()
{


	// Grab the input control values
	if (m_filemode == MODE_FILE) {
		GetDlgItemText (m_hwnd, ID_EDITSOURCE, m_szInputfile, SIZE_STRING);
		GetDlgItemText (m_hwnd, ID_EDITDEST, m_szOutputfile, SIZE_STRING);
	}

	if (m_filemode == MODE_FOLDER) {
		ZeroMemory (m_szInputfolder, SIZE_STRING);
		GetDlgItemText (m_hwnd, ID_EDITSOURCE, m_szInputfolder, SIZE_STRING);
		GetDlgItemText (m_hwnd, ID_EDITDEST, m_szOutputfile, SIZE_STRING);
	}

	// Some basic input validation here.
	if (m_filemode == MODE_FILE) {	
		if (strlen (m_szInputfile) > 0) {
			if (strcmp (m_szInputfile, "<No Source Selected>") == 0) {
				if (m_filemode == MODE_FILE) {
					MessageBox (m_hwnd, "Please select a source file to encrypt.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
					return;
				}
				if (m_filemode == MODE_FOLDER) {
					MessageBox (m_hwnd, "Please select a source folder to encrypt.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
					return;
				}				
			}
		} else {
			MessageBox (m_hwnd, "Please select a source.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	if (m_filemode == MODE_FOLDER) {
		if (strlen (m_szInputfolder) > 0) {
			if (strcmp (m_szInputfolder, "<No Source Selected>") == 0) {

				if (m_filemode == MODE_FOLDER) {
					MessageBox (m_hwnd, "Please select a source folder to encrypt.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
					return;
				}
				
			}
		} else {
			MessageBox (m_hwnd, "Please select a source.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}

	if (strlen (m_szOutputfile) > 0) {
		if (strcmp (m_szOutputfile, "<No Destination Selected>") == 0) {
			if (m_filemode == MODE_FILE) {
				MessageBox (m_hwnd, "Please select a destination file.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
				return;
			}
			if (m_filemode == MODE_FOLDER) {
				//MessageBox (m_hwnd, "Please select a destination folder.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
				//return;
			}
		}
	} else {
		MessageBox (m_hwnd, "Please select a destination.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_passwindow.Initialise (m_hwnd, 0);
	
	if (m_appmode == MODE_ENCRYPT) {
		m_passwindow.SetEncryptMode (true);
	} else {
		m_passwindow.SetEncryptMode (false);
	}
	
	if (m_bcreatepackage == true) {
		// If we are in package creation mode, we need to work out the total size of all of the files				
		InitialisePackage ();
	}

	m_passwindow.SetSessionMode (false, false);
	m_passwindow.Show ();
}

unsigned int MainWindow::CalcInputChecksum ()
{
	unsigned int b = 0;
	unsigned int sum = 0;
	BYTE bcurrent;

	if (m_filemode == MODE_TEXT) {
		for (b=0;b<m_memTextbuffer.GetSize ();b+=10) {
			bcurrent = m_memTextbuffer.GetByte (b);
			//m_pdiag->OutputInt ("Byte: ", bcurrent);
			sum+=bcurrent;
		}
	} else {
		for (b=0;b<m_fileinput.GetSize ();b+=10) {
			bcurrent = m_fileinput.GetByte (b);
			//m_pdiag->OutputInt ("Byte: ", bcurrent);
			sum+=bcurrent;
		}
	}
	
	return sum;
}

unsigned int MainWindow::CalcOutputChecksum ()
{
	unsigned int b = 0;
	unsigned int sum = 0;
	BYTE bcurrent;
	for (b=0;b<m_fileoutput.GetSize ();b+=10) {
		bcurrent = m_fileoutput.GetByte (b);
		//m_pdiag->OutputInt ("Byte: ", bcurrent);
		sum+=bcurrent;
	}
	return sum;
}

void MainWindow::DoTextEncryption ()
{
	SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);

	ZeroMemory (m_szPassword, SIZE_NAME);
	strcpy_s (m_szPassword, SIZE_NAME, m_passwindow.GetLastPassword ());

	char szStatusMessage[SIZE_STRING];

	//m_pdiag->OutputText ("Password Entered: ", m_passwindow.GetLastPassword ());
	if (m_crypt.CreateKeyHive (m_szPassword) == true) {
		
		OutputStatusText ("Encryption Hive successfully generated.");
	
		if (m_appmode == MODE_ENCRYPT) {
			// Now secure the users password in the file
			SecurePassword ();

			m_checksum = CalcInputChecksum ();
			m_pdiag->OutputInt ("Input file checksum: ", m_checksum);
						
			if (m_filemode == MODE_TEXT) {
				OutputStatusText ("Encrypting Clipboard Text...");
			}
						
			m_crypt.DoSingleCipher (m_hwnd, &m_memTextbuffer, &m_memTextoutput, true);
		}

		if (m_appmode == MODE_DECRYPT) {
				int ipointer = 0;
				int ibdsize = 0;
				memcpy (&m_inputid, (BYTE *) m_memTextprocessed.GetBuffer ()+ipointer, sizeof (unsigned int));
				ipointer+=sizeof (unsigned int);

				if (m_inputid == m_ccryptidprev1) {
					MessageBox (m_hwnd, "This text was encrypted using CedeCrypt v2.1, and is incompatible with this version. Please use CedeCrypt v2.1 to decrypt this text. A backward compatibility feature will be implemented in future versions.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
				} else {			
					if (m_inputid != m_ccryptid) {
						MessageBox (m_hwnd, "This is not a valid CedeCrypt v2.3 encoded text stream. Please check your highlighted text and try again.", "CedeCrypt Invalid Text", MB_OK | MB_ICONEXCLAMATION);

					} else {
						memcpy (&m_inputchecksum, (BYTE *) m_memTextprocessed.GetBuffer ()+ipointer, sizeof (unsigned int));
						ipointer+=sizeof (unsigned int);

						// Skip over the backdoor for decryption, but first we need it's size
						memcpy (&ibdsize, (BYTE *) m_memTextprocessed.GetBuffer ()+ipointer, sizeof (int));																	
						ipointer+=sizeof (int);
						ipointer+=ibdsize; // skip over the backdoor data

						m_fileinputex.SetSize (m_memTextprocessed.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize);
						m_fileinputex.Write ((BYTE *) m_memTextprocessed.GetBuffer ()+ipointer, 0, m_memTextprocessed.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize);

						m_pdiag->OutputInt ("File ID is: ", m_inputid);
						m_pdiag->OutputInt ("Text Checksum is: ", m_inputchecksum);						
						m_pdiag->OutputInt ("ibdsize is: ", ibdsize);


						if (m_filemode == MODE_TEXT) {
							OutputStatusText ("Decrypting text from clipboard...");
							
							ZeroMemory (szStatusMessage, SIZE_STRING);
							sprintf_s (szStatusMessage, SIZE_STRING, "Decrypting Clipboard Text...");
							SetDlgItemText (m_hwnd, ID_LBLTOTAL, szStatusMessage);
						}

						m_crypt.DoSingleCipher (m_hwnd, &m_fileinputex, &m_fileoutput, false);
					}
				}
		}

	} else {
		OutputStatusText ("Encryption Hive generation failed.");
	}
}

void MainWindow::DoEncryption ()
{
	SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);

	ZeroMemory (m_szPassword, SIZE_NAME);
	strcpy_s (m_szPassword, SIZE_NAME, m_passwindow.GetLastPassword ());

	if (m_bSessionmode == true) {
		if (m_bUsesessionpassword == false) {
			ZeroMemory (m_szSessionpassword, SIZE_NAME);
			strcpy_s (m_szSessionpassword, SIZE_NAME, m_passwindow.GetLastPassword ());
			m_bSessionpasswordpresent = true;
		}

		if (m_bUsesessionpassword == true) {
			ZeroMemory (m_szPassword, SIZE_NAME);
			strcpy_s (m_szPassword, SIZE_NAME, m_szSessionpassword);
		}
	}

	char szStatusMessage[SIZE_STRING];

	if (m_appmode == MODE_ENCRYPT) {
		// Set the encryption algorithm to the one chosen by the user (if any)
		m_crypt.SetAlgorithm (m_optionswindow.GetSelectedAlgorithm ());

		if (m_crypt.GetAlgorithm () == ALG_CYBERCEDE) {
			OutputStatusText ("Encrypting using: CyberCede 80,128 Bit");
		}

		if (m_crypt.GetAlgorithm () == ALG_AES256) {
			OutputStatusText ("Encrypting using: AES 256 Bit");
		}

		if (m_crypt.GetAlgorithm () == ALG_3DES) {
			OutputStatusText ("Encrypting using: 3-DES 168 Bit");
		}

		if (m_crypt.GetAlgorithm () == ALG_DES) {
			OutputStatusText ("Encrypting using: DES 56 Bit");
		}

		
	} else {
		// If we are in decrypt mode, we need to look ahead in the input file
		// and check the file format, and then check the algorithm used in the file
		// and set the algorithm here and now. This prevents us from having to create a CyberCede
		// keyhive if we don't need to (for algorithms other than CyberCede) as generating a key hive
		// will add additional time taken to do a decrypt.

		// For now, default to Cybercede
		m_crypt.SetAlgorithm (ALG_CYBERCEDE);
	}

	//m_pdiag->OutputText ("Password Entered: ", m_passwindow.GetLastPassword ());
	if (m_crypt.CreateKeyHive (m_szPassword) == true) {
		OutputStatusText ("Encryption Hive successfully generated.");

		if (m_fileinput.ReadFromFile (m_szInputfile) == true) {

			//if (m_fileinput.GetSize () > 2000000) {
				//MessageBox (m_hwnd, "Demonstration version limitation. Only files below 2MB can be processed. Please purchase the full product.", "CedeCrypt Demo Limitation", MB_OK | MB_ICONEXCLAMATION);
				//return;
			//}

			if (m_appmode == MODE_ENCRYPT) {
				// Now secure the users password in the file
				SecurePassword ();

				// Now secure the users password again, but using the canvas key - the key generated by the customer
				// using our distribution setup program
				SecurePasswordUsingCanvas ();

				m_checksum = CalcInputChecksum ();
				m_pdiag->OutputInt ("Input file checksum: ", m_checksum);
				if (m_filemode == MODE_FILE) {
					OutputStatusText ("Encrypting file: ", m_szInputfiletitle);

					ZeroMemory (szStatusMessage, SIZE_STRING);
					sprintf_s (szStatusMessage, SIZE_STRING, "Encrypting: %s", m_szInputfiletitle);
					SetDlgItemText (m_hwnd, ID_LBLTOTAL, szStatusMessage);
				}

				if (m_filemode == MODE_FOLDER) {
					OutputStatusText ("Encrypting file: ", m_szInputfile);

					ZeroMemory (szStatusMessage, SIZE_STRING);
					sprintf_s (szStatusMessage, SIZE_STRING, "Encrypting: %s", m_szInputfile);
					SetDlgItemText (m_hwnd, ID_LBLTOTAL, szStatusMessage);
				}
				
				
				//m_crypt.DoResequence (m_hwnd, &m_fileinput, true);
				//m_crypt.DoResequence (m_hwnd, &m_fileinput, false);
				m_crypt.DoSingleCipher (m_hwnd, &m_fileinput, &m_fileoutput, true);
			}

			if (m_appmode == MODE_DECRYPT) {

				// m_cryptid2 format:
				// u-int	   u-int		   int			  byte			 byte
				// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | ENCRYPTED FILEDATA

				// m_cryptid3 format:
				// u-int	   u-int		   int			  byte			 int		  byte			int			   char			  byte
				// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | CANVASSIZE | CANVAS DATA | DISTNAMESIZE | DISTNAMETEXT | ENCRYPTED FILEDATA

				// m_cryptid format: (Latest version)
				// u-int	   u-int		   int			  byte			 int		  byte			int			   char			  int			byte
				// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | CANVASSIZE | CANVAS DATA | DISTNAMESIZE | DISTNAMETEXT | ALGORITHMID | ENCRYPTED FILEDATA

				int ipointer = 0;
				int ibdsize = 0;
				int ibdcanvassize = 0;
				int idistnamesize = 0;
				int iAlgorithm = 0;
				memcpy (&m_inputid, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
				ipointer+=sizeof (unsigned int);

				if (m_inputid == m_ccryptidprev1) {
					MessageBox (m_hwnd, "This file was encrypted using CedeCrypt v2.1, and is incompatible with this version. Please use CedeCrypt v2.1 to decrypt this file. A backward compatibility feature will be implemented in future versions.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
				} else {			
					if (m_inputid != m_ccryptid && m_inputid != m_ccryptid2 && m_inputid != m_ccryptid3) {
						if (m_inputid > 477771549 && m_inputid < 477771700) {
							MessageBox (m_hwnd, "This file was encrypted using a newer version of CedeCrypt. Please visit http://www.cedesoft.com for a software update.", "CedeCrypt Newer File Format", MB_OK | MB_ICONEXCLAMATION);
						} else {
							MessageBox (m_hwnd, "This is not a valid CedeCrypt encoded file. Please check the input filename and try again.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
						}
						
						
						if (m_bShellmode == true || m_bCommandmode == true) {
							PostQuitMessage (0);
						}
					} else {
						memcpy (&m_inputchecksum, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
						ipointer+=sizeof (unsigned int);


						// Skip over the backdoor for decryption, but first we need it's size
						memcpy (&ibdsize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
						ipointer+=sizeof (int);
						ipointer+=ibdsize; // skip over the backdoor data

						if (m_inputid == m_ccryptid || m_inputid == m_ccryptid3) { // If this is newer saved file than skip over the canvas 
							// backdoor for decryption, but we also need it's size
							memcpy (&ibdcanvassize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
							ipointer+=sizeof (int);
							ipointer+=ibdcanvassize; // skip over the canvas backdoor data

							// We also need to skip over the distribution name, the distribution name is only
							// used for retrieving recovery data
							memcpy (&idistnamesize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
							ipointer+=sizeof (int);
							ipointer+=idistnamesize; // skip over the distribution name							
						}

						if (m_inputid == m_ccryptid) {
							// Now we need to read the algorithm id used to encrypt this file
							memcpy (&iAlgorithm, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
							ipointer+=sizeof (int);

							// Now set the algorithm
							m_crypt.SetAlgorithm (iAlgorithm);

							// Print the algorithm used

							if (iAlgorithm == ALG_CYBERCEDE) {
								OutputStatusText ("Algorithm: CyberCede 80,128 Bit");
							}

							if (iAlgorithm == ALG_AES256) {
								OutputStatusText ("Algorithm: AES 256 Bit");
							}

							if (iAlgorithm == ALG_3DES) {
								OutputStatusText ("Algorithm: 3-DES 168 Bit");
							}

							if (iAlgorithm == ALG_DES) {
								OutputStatusText ("Algorithm: DES 56 Bit");
							}							
						}


						if (m_inputid == m_ccryptid) { // m_ccryptid always identifies the latest version
							m_fileinputex.SetSize (m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize-sizeof (int)-ibdcanvassize-sizeof (int) - idistnamesize - sizeof (int));
							m_fileinputex.Write ((BYTE *) m_fileinput.GetBuffer ()+ipointer, 0, m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize-sizeof (int)-ibdcanvassize - sizeof (int) - idistnamesize - sizeof (int));
						}

						if (m_inputid == m_ccryptid3) { // m_ccryptid3 is the last version prior to latest
							m_fileinputex.SetSize (m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize-sizeof (int)-ibdcanvassize-sizeof (int) - idistnamesize);
							m_fileinputex.Write ((BYTE *) m_fileinput.GetBuffer ()+ipointer, 0, m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize-sizeof (int)-ibdcanvassize - sizeof (int) - idistnamesize);
						}

						if (m_inputid == m_ccryptid2) { // the higher the cryptid version, than the more older the version
							m_fileinputex.SetSize (m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize);
							m_fileinputex.Write ((BYTE *) m_fileinput.GetBuffer ()+ipointer, 0, m_fileinput.GetSize ()-(sizeof (unsigned int)*2)-sizeof (int)-ibdsize);
						}
						
						m_pdiag->OutputInt ("File ID is: ", m_inputid);
						m_pdiag->OutputInt ("File Checksum is: ", m_inputchecksum);						

						if (m_filemode == MODE_FILE) {
							OutputStatusText ("Decrypting file: ", m_szInputfiletitle);
							
							ZeroMemory (szStatusMessage, SIZE_STRING);
							sprintf_s (szStatusMessage, SIZE_STRING, "Decrypting: %s", m_szInputfiletitle);
							SetDlgItemText (m_hwnd, ID_LBLTOTAL, szStatusMessage);
						}

						if (m_filemode == MODE_FOLDER) {
							OutputStatusText ("Decrypting file: ", m_szInputfile);
							
							ZeroMemory (szStatusMessage, SIZE_STRING);
							sprintf_s (szStatusMessage, SIZE_STRING, "Decrypting: %s", m_szInputfile);
							SetDlgItemText (m_hwnd, ID_LBLTOTAL, szStatusMessage);
						}						

						m_crypt.DoSingleCipher (m_hwnd, &m_fileinputex, &m_fileoutput, false);
						//m_crypt.DoResequence (m_hwnd, &m_fileinput, &m_fileoutput, false);
					}
				}
			}

		} else {
			OutputStatusText ("Unable to read input file: ", m_szInputfile);
			
			char szError [SIZE_STRING];
			ZeroMemory (szError, SIZE_STRING);

			strcpy_s (szError, SIZE_STRING, "Unable to open: ");
			strcat_s (szError, SIZE_STRING, m_szInputfile);

			MessageBox (NULL, szError, "Read Error", MB_OK | MB_ICONEXCLAMATION);
		}

	} else {
		OutputStatusText ("Encryption Hive generation failed.");
	}
}

bool MainWindow::CheckEnclosingTags (MemoryBuffer *memReadable)
{
	BYTE bCur = 0;
	unsigned long c = 0;

	bool bfoundstart = false;
	bool bfoundend = false;

	for (c=0;c<memReadable->GetSize ();c++) {
		bCur = memReadable->GetByte (c);

		if (bCur == 60) {
			bfoundstart = true;
		}

		if (bCur == 62) {
			bfoundend = true;
		}
	}

	if (bfoundstart == true && bfoundend == true) {
		return true;
	} else {
		return false;
	}
}

bool MainWindow::CheckStartTag (MemoryBuffer *memReadable)
{
	// Function to look for CCRYPT191188
	char szCurtag[SIZE_NAME];
	unsigned long c = 0;


	for (c=0;c<memReadable->GetSize ();c++) {
		
		ZeroMemory (szCurtag, SIZE_NAME);
		strncpy_s (szCurtag, SIZE_NAME, (char *) memReadable->GetBuffer ()+c, 12);

		if (strcmp (szCurtag, "CCRYPT191188") == 0) {
			return true;
		}
	}

	return false;
}

int MainWindow::GetStartTagLocation (MemoryBuffer *memReadable)
{
	// Function to look for CCRYPT191188 and then return the location of the first
	// readable character after the found tag

	// So if the text is CCRYPT191188BA8F, the location of BA is returned.

	char szCurtag[SIZE_NAME];
	unsigned long c = 0;

	for (c=0;c<memReadable->GetSize ();c++) {
		ZeroMemory (szCurtag, SIZE_NAME);

		strncpy_s (szCurtag, SIZE_NAME, (char *) memReadable->GetBuffer ()+c, 12);

		if (strcmp (szCurtag, "CCRYPT191188") == 0) {
			return c + 12;
		}
	}

	return -1;
}

int MainWindow::GetEndTagLocation (MemoryBuffer *memReadable)
{
	// Function to look for the > character in the readable text and return the location

	BYTE bCur = 0;
	unsigned long c = 0;

	for (c=0;c<memReadable->GetSize ();c++) {
		bCur = memReadable->GetByte (c);

		if (bCur == 62) {
			return c;
		}
	}

	return -1;
}

void MainWindow::MakeReadable (MemoryBuffer *memInput, MemoryBuffer *memOutput)
{
	memOutput->Clear ();
	memOutput->SetSize (memInput->GetSize () * 2);

	unsigned long l = 0;
	BYTE bCur = 0;
	char szByte[SIZE_NAME];

	for (l=0;l<memInput->GetSize ();l++) {
		bCur = memInput->GetByte (l);
				
		ZeroMemory (szByte, SIZE_NAME);
		
		sprintf_s (szByte, SIZE_NAME, "%.2X", bCur);
		//itoa (bCur, szByte, 16);

		memOutput->Append ((char *) szByte, strlen (szByte));
		
	}
}

void MainWindow::MakeBinary (MemoryBuffer *memInput, MemoryBuffer *memOutput)
{
	char szHex[SIZE_NAME];
	BYTE bCur = 0;
	int c = 0;
	int idestpointer = 0;
	char *endchar;

	memOutput->Clear ();
	memOutput->SetSize (memInput->GetSize () / 2);

	for (c=0;c<memInput->GetSize ();c+=2) {
		ZeroMemory (szHex, SIZE_NAME);
		strncpy (szHex, (char *) memInput->GetBuffer ()+c, 2);

		bCur = strtol (szHex, &endchar, 16);
		
		memOutput->SetByte (idestpointer, bCur);
		idestpointer++;
		
	}
}


void MainWindow::SaveEncryptedText ()
{
	m_pdiag->OutputInt ("m_textoutput size is: ", m_memTextoutput.GetSize ());

	int ibackdoorsize = 0;

	if (m_filemode == MODE_TEXT) {
		
		if (m_appmode == MODE_ENCRYPT) {
			
			// u-int	   u-int		   int			  byte			 byte
			// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | ENCRYPTED FILEDATA

			m_memTextprocessed.SetSize (m_memTextoutput.GetSize ()+(sizeof (unsigned int)*2)+m_memEncrypted.GetSize ()+sizeof (int));
			m_memTextprocessed.Append (&m_ccryptid, sizeof (unsigned int));
			m_memTextprocessed.Append (&m_checksum, sizeof (unsigned int));
			ibackdoorsize = m_memEncrypted.GetSize ();
			m_memTextprocessed.Append (&ibackdoorsize, sizeof (int));
			m_memTextprocessed.Append (m_memEncrypted.GetBuffer (), m_memEncrypted.GetSize ());
			m_memTextprocessed.Append (m_memTextoutput.GetBuffer (), m_memTextoutput.GetSize ());


			m_pdiag->OutputInt ("m_ccryptid size is: ", m_ccryptid);
			m_pdiag->OutputInt ("m_checksum size is: ", m_checksum);
			m_pdiag->OutputInt ("ibdsize size is: ", ibackdoorsize);

			m_pdiag->OutputInt ("m_memTextprocessed size is: ", m_memTextprocessed.GetSize ());

			//m_fileoutputex.SaveToFile (m_szOutputfile);
			// Now we have to convert the text processed buffer to plain text readable
			// and then put back into the clipboard
			m_memTextprocessed.SaveToFile ("C:\\Temp\\TextProcessed.dat");

			// Now convert the processed buffer so that the encrypted bytes are text readable
			// suitable for pasting into any text window. Basically they are just converted
			// to their hex equivalents.
			MakeReadable (&m_memTextprocessed, &m_memTextreadable);

			//m_memTextreadable.SaveToFile ("C:\\Temp\\TextReadable.txt");

			// now put the contents back in the clipboard
			//m_memTextreadable.SaveToClipboard (m_hwnd);


			m_memTextpasteable.SetSize (m_memTextreadable.GetSize () + 15);
			// 14 is the number of characters for the preceding <CCRYPT191188 and ending >
			
			// Prepare the prefix and suffix text string which will be inserted
			// before and after the encrypted payload.
			char szPrefix[SIZE_NAME];
			ZeroMemory (szPrefix, SIZE_NAME);
			strcpy_s (szPrefix, SIZE_NAME, "<CCRYPT191188");

			char szSuffix[SIZE_NAME];
			ZeroMemory (szSuffix, SIZE_NAME);
			strcpy_s (szSuffix, SIZE_NAME, "> ");

			// Now prepare the pasteable buffer with the text strings and the main encrypted payload.
			m_memTextpasteable.Append ((char *) szPrefix, strlen (szPrefix));
			m_memTextpasteable.Append (m_memTextreadable.GetBuffer (), m_memTextreadable.GetAppendPointer ());
			m_memTextpasteable.Append ((char *) szSuffix, strlen (szSuffix));

			// Save to a file
			//m_memTextpasteable.SaveToFile ("C:\\Temp\\TextPasteable.dat");

			// now put the contents back in the clipboard
			m_memTextpasteable.SaveToClipboard (m_hwnd);



			// Now make the last users window active
			SetForegroundWindow (m_lastactivewindow);

			// Now do a CTRL-V to paste the text back into the users window
			Sleep (500);
			SetShiftState (1);
			//SendKeyState (67);
			SendKeyState (86);
			SetShiftState (0);
			CloseClipboard ();			

			OutputStatusText ("Encryption Completed - Clipboard Text");
		}

		if (m_appmode == MODE_DECRYPT) {
		
			m_checksum = CalcOutputChecksum ();
			m_pdiag->OutputInt ("Output checksum: ", m_checksum);

			if (m_inputchecksum != m_checksum) {
				
				OutputStatusText ("Decryption of Clipboard Text failed.");
				OutputStatusText ("Check that you have entered the correct decryption password.");

				MessageBox (NULL, "Decryption failed. Check that you have entered the correct password.", "CedeCrypt Decryption", MB_OK | MB_ICONEXCLAMATION);
				
				m_fileoutput.SaveToFile ("C:\\Temp\\TextDecrypted.txt");

			} else {
				// Text decryption was successful

				m_fileoutput.SaveToFile ("C:\\Temp\\TextDecrypted.txt");

				m_memTextfinished.Clear ();
				m_memTextfinished.SetSize (m_fileoutput.GetSize ()+2);
				m_memTextfinished.Write (m_fileoutput.GetBuffer (), 0, m_fileoutput.GetSize ());
				m_memTextfinished.SetByte (m_fileoutput.GetSize (), 32);

				m_textwindow.Initialise (m_hwnd, 0);
				m_textwindow.SetText ((char *) m_memTextfinished.GetBuffer ());
				m_textwindow.Show ();

				OutputStatusText ("Decryption of Clipboard completed.");
			}		
		}
	}
}

void MainWindow::SaveEncryptedFile ()
{
	m_pdiag->OutputInt ("m_fileoutput size is: ", m_fileoutput.GetSize ());
	bool bDonotdelete = false;
	int ibackdoorsize = 0;
	int icanvasbackdoorsize = 0;
	int idistnamesize = 0;
	int iAlgorithm = 0;
	char szHomedrive[SIZE_STRING];
	char szProposed[SIZE_STRING];

	if (m_filemode == MODE_FOLDER) {

		if (m_appmode == MODE_ENCRYPT) {
			// u-int	   u-int		   int			  byte			 int				  byte				   int            char       int		   byte
			// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | CANVASBACKDOORSIZE | CANVASBACKDOORDATA | DISTNAMESIZE | DISTNAME | ALGORITHMID | ENCRYPTED FILEDATA

			m_fileoutputex.SetSize (m_fileoutput.GetSize ()+(sizeof (unsigned int)*2)+m_memEncrypted.GetSize ()+m_memCanvasEncrypted.GetSize ()+sizeof (int) + strlen (m_szDistributionname) + sizeof (int) + sizeof (int)+sizeof (int));
			m_fileoutputex.Append (&m_ccryptid, sizeof (unsigned int));
			m_fileoutputex.Append (&m_checksum, sizeof (unsigned int));
			ibackdoorsize = m_memEncrypted.GetSize ();
			m_fileoutputex.Append (&ibackdoorsize, sizeof (int));
			m_fileoutputex.Append (m_memEncrypted.GetBuffer (), m_memEncrypted.GetSize ());
			icanvasbackdoorsize = m_memCanvasEncrypted.GetSize ();
			m_fileoutputex.Append (&icanvasbackdoorsize, sizeof (int));
			m_fileoutputex.Append (m_memCanvasEncrypted.GetBuffer (), m_memCanvasEncrypted.GetSize ());
			int idistnamesize = strlen (m_szDistributionname);
			m_fileoutputex.Append (&idistnamesize, sizeof (int));
			m_fileoutputex.Append (m_szDistributionname, idistnamesize);
			iAlgorithm = m_crypt.GetAlgorithm ();
			m_fileoutputex.Append (&iAlgorithm, sizeof (int));
			
			m_fileoutputex.Append (m_fileoutput.GetBuffer (), m_fileoutput.GetSize ());

			m_pdiag->OutputInt ("m_fileoutputex size is: ", m_fileoutputex.GetSize ());

			if (m_bcreatepackage == false) {
				m_fileoutputex.SaveToFile (m_szOutputfile);
			} else {
				
				// the bits marked done are sorted by CheckShellCommand () the bits marked todo
				// are the bits we take care of here.
				// m_memPackage

				// done | done | done | done | done     | todo    | todo | todo    | todo
				// SIG1 | SIG2 | SIG3 | SIG4 | NUMFILES | NAMELEN | NAME | FILELEN | FILE
				// ulong| ulong| ulong| ulong| int      | int     | char | ulong   | byte

				int nlen = 0;
				unsigned long flen = 0;
				char szfpath[SIZE_STRING];
				
				ZeroMemory (szfpath, SIZE_STRING);
				strcpy_s (szfpath, SIZE_STRING, m_szInputfile);

				nlen = strlen (szfpath);
				flen = m_fileoutputex.GetSize ();

				m_memPackage.Append (&nlen, sizeof (int));
				m_memPackage.Append (szfpath, nlen);
				m_memPackage.Append (&flen, sizeof (unsigned long));
				m_memPackage.Append (m_fileoutputex.GetBuffer (), flen);


				//m_pdiag->OutputText ("INPUT FILE IS: ", m_szInputfile);

				//for (f=0;f<m_dlFilelist.GetNumItems ();f++) {
				//	ZeroMemory (szfpath, SIZE_STRING);
				//	
				//	m_dlFilelist.GetItem (szfpath, f);

				//	m_pdiag->OutputText ("fpath: ", szfpath);
				//}
			}
			


			OutputStatusText ("Encryption Completed: ", m_szInputfile);
			

			SendMessage (m_hwndtotalprogress, PBM_SETPOS, m_iFilepointer, 0L);

			if (m_cmdmode == APP_ENCRYPT) {
				PostQuitMessage (0);
			} else {
				if (m_iFilepointer == m_dlFilelist.GetNumItems ()-1) {
					
					if (m_bcreatepackage == true) {
						// If we are in package creation mode then now we need to save all of the encrypted files
						// along with the self-decrypting viewer
						//IDR_BINDECRYPTOR
						MemoryBuffer memDecryptor;
						Resourcetomem (MAKEINTRESOURCE (IDR_BINDECRYPTOR), &memDecryptor);

						MemoryBuffer memCombined;
						memCombined.SetSize (memDecryptor.GetSize () + m_memPackage.GetSize ());
						memCombined.Append (memDecryptor.GetBuffer (), memDecryptor.GetSize ());
						memCombined.Append (m_memPackage.GetBuffer (), m_memPackage.GetSize ());
					
						ZeroMemory (szHomedrive, SIZE_STRING);

						GetEnvironmentVariable ("USERPROFILE", szHomedrive, SIZE_STRING);
						
						strcat_s (szHomedrive, SIZE_STRING, "\\Desktop\\EncryptedPackages");
						_mkdir (szHomedrive);
						
						strcat_s (szHomedrive, SIZE_STRING, "\\EncryptedPackage.exe");

						
						ZeroMemory (szProposed, SIZE_STRING);
						m_filehandler.CheckDuplicatedFile (szHomedrive, szProposed);
						
						memCombined.SaveToFile (szProposed);

						memDecryptor.Clear ();
						memCombined.Clear ();
						m_memPackage.Clear ();

					}
					
					if (m_bSessionmode == false)
					{
						if (m_bcreatepackage == true) {
							
							char szmessage[SIZE_STRING];
							ZeroMemory (szmessage, SIZE_STRING);

							sprintf_s (szmessage, SIZE_STRING, "Encrypted Package has been created successfully, and has been saved to %s.", szProposed);
							
							MessageBox (m_hwnd, szmessage, "CedeCrypt Encryption", MB_OK | MB_ICONINFORMATION);								

						} else {
							MessageBox (m_hwnd, "File(s) encrypted successfully.", "CedeCrypt Encryption", MB_OK | MB_ICONINFORMATION);
						}						

						SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);
						SendMessage (m_hwndtotalprogress, PBM_SETPOS, 0, 0L);

						SetDlgItemText (m_hwnd, ID_LBLPHASE, "");
						SetDlgItemText (m_hwnd, ID_LBLTOTAL, "");
					}
					
					if (m_bShellmode == true) {
						PostQuitMessage (0);
					}

					if (m_bSessionmode == true) {
						//if (m_bSessionshuttingdown == false) {
							m_sbcancelwaitdlg = true;

							// Clear all passwords just for safety
							ZeroMemory (m_szSessionpassword, SIZE_NAME);
							ZeroMemory (m_szPassword, SIZE_NAME);
							m_passwindow.ClearPassword ();

							Hide ();
						//} else {
							//m_bShutdownencryptdone = true;
							//PostQuitMessage (0);
						//}						
					}
				}
			}
		}

		if (m_appmode == MODE_DECRYPT) {
			m_checksum = CalcOutputChecksum ();
			m_pdiag->OutputInt ("Output checksum: ", m_checksum);

			if (m_inputchecksum != m_checksum) {
				bDonotdelete = true;
				OutputStatusText ("Decryption Failed: ", m_szInputfile);
				OutputStatusText ("Check that you have entered the correct decryption password.");

				if (m_iFilepointer == m_dlFilelist.GetNumItems ()-1) {
					MessageBox (m_hwnd, "Decryption failed. Check that you have entered the correct password.", "CedeCrypt Decryption", MB_OK | MB_ICONEXCLAMATION);

					if (m_bShellmode == true || m_bCommandmode == true) {
						PostQuitMessage (0);
					}

					if (m_bSessionmode == true) {
						//DoSessionDecryption ();
						Hide ();
					}

					SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);
					SendMessage (m_hwndtotalprogress, PBM_SETPOS, 0, 0L);

					SetDlgItemText (m_hwnd, ID_LBLPHASE, "");
					SetDlgItemText (m_hwnd, ID_LBLTOTAL, "");

				}

			} else {
				if (m_bCommandmode == true) {
					PrepareTempCommandFile (m_szOutputfile);
					m_fileoutput.SaveToFile (m_szCommandTempFile);
					SetFileAttributes (m_szCommandTempFile, FILE_ATTRIBUTE_READONLY);
				} else {
					m_fileoutput.SaveToFile (m_szOutputfile);
				}
				

				OutputStatusText ("Decryption Completed: ", m_szInputfile);
				
				SendMessage (m_hwndtotalprogress, PBM_SETPOS, m_iFilepointer, 0L);

				if (m_iFilepointer == m_dlFilelist.GetNumItems ()-1) {
					if (m_bCommandmode == false) {
						if (m_bSessionmode == false) {
							MessageBox (m_hwnd, "File(s) decrypted successfully.", "CedeCrypt Decryption", MB_OK | MB_ICONINFORMATION);

							SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);
							SendMessage (m_hwndtotalprogress, PBM_SETPOS, 0, 0L);

							SetDlgItemText (m_hwnd, ID_LBLPHASE, "");
							SetDlgItemText (m_hwnd, ID_LBLTOTAL, "");

						}
					}					

					if (m_bCommandmode == true) {
						ShellExecute (m_hwnd, "open", m_szCommandTempFile, NULL, NULL, SW_SHOW);
					}

					if (m_bShellmode == true || m_bCommandmode == true) {
						PostQuitMessage (0);
					}			

					if (m_bSessionmode == true) {
						Hide ();
					}
				}
			}
		}
	}

	if (m_filemode == MODE_FILE) {
		
		if (m_appmode == MODE_ENCRYPT) {
			// u-int	   u-int		   int			  byte			 int				  byte				   int            char		 int		   byte
			// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | CANVASBACKDOORSIZE | CANVASBACKDOORDATA | DISTNAMESIZE | DISTNAME | ALGORITHMID | ENCRYPTED FILEDATA

			m_fileoutputex.SetSize (m_fileoutput.GetSize ()+(sizeof (unsigned int)*2)+m_memEncrypted.GetSize ()+m_memCanvasEncrypted.GetSize () + sizeof(int)+ strlen (m_szDistributionname) + sizeof (int) + sizeof (int) + sizeof (int));
			m_fileoutputex.Append (&m_ccryptid, sizeof (unsigned int));
			m_fileoutputex.Append (&m_checksum, sizeof (unsigned int));
			ibackdoorsize = m_memEncrypted.GetSize ();
			m_fileoutputex.Append (&ibackdoorsize, sizeof (int));
			m_fileoutputex.Append (m_memEncrypted.GetBuffer (), m_memEncrypted.GetSize ());
			icanvasbackdoorsize = m_memCanvasEncrypted.GetSize ();
			m_fileoutputex.Append (&icanvasbackdoorsize, sizeof (int));
			m_fileoutputex.Append (m_memCanvasEncrypted.GetBuffer (), m_memCanvasEncrypted.GetSize ());
			int idistnamesize = strlen (m_szDistributionname);
			m_fileoutputex.Append (&idistnamesize, sizeof (int));
			m_fileoutputex.Append (m_szDistributionname, idistnamesize);
			iAlgorithm = m_crypt.GetAlgorithm ();
			m_fileoutputex.Append (&iAlgorithm, sizeof (int));

			m_fileoutputex.Append (m_fileoutput.GetBuffer (), m_fileoutput.GetSize ());

			m_pdiag->OutputInt ("m_fileoutputex size is: ", m_fileoutputex.GetSize ());
			
			if (m_bcreatepackage == false) {
				m_fileoutputex.SaveToFile (m_szOutputfile);
				m_finishtime = GetTickCount ();
			} else {
				
				// the bits marked done are sorted by CheckShellCommand () the bits marked todo
				// are the bits we take care of here.
				// m_memPackage

				// done | done | done | done | done     | todo    | todo | todo    | todo
				// SIG1 | SIG2 | SIG3 | SIG4 | NUMFILES | NAMELEN | NAME | FILELEN | FILE
				// ulong| ulong| ulong| ulong| int      | int     | char | ulong   | byte

				// Initialise the Encrypted Package
				InitialisePackage ();

				int nlen = 0;
				unsigned long flen = 0;
				char szfpath[SIZE_STRING];
				
				ZeroMemory (szfpath, SIZE_STRING);
				strcpy_s (szfpath, SIZE_STRING, m_szInputfile);

				nlen = strlen (szfpath);
				flen = m_fileoutputex.GetSize ();

				m_memPackage.Append (&nlen, sizeof (int));
				m_memPackage.Append (szfpath, nlen);
				m_memPackage.Append (&flen, sizeof (unsigned long));
				m_memPackage.Append (m_fileoutputex.GetBuffer (), flen);

				if (m_bcreatepackage == true) {
					// If we are in package creation mode then now we need to save all of the encrypted files
					// along with the self-decrypting viewer
					//IDR_BINDECRYPTOR
					MemoryBuffer memDecryptor;
					Resourcetomem (MAKEINTRESOURCE (IDR_BINDECRYPTOR), &memDecryptor);

					MemoryBuffer memCombined;
					memCombined.SetSize (memDecryptor.GetSize () + m_memPackage.GetSize ());
					memCombined.Append (memDecryptor.GetBuffer (), memDecryptor.GetSize ());
					memCombined.Append (m_memPackage.GetBuffer (), m_memPackage.GetSize ());
				
					ZeroMemory (szHomedrive, SIZE_STRING);

					GetEnvironmentVariable ("USERPROFILE", szHomedrive, SIZE_STRING);
					
					strcat_s (szHomedrive, SIZE_STRING, "\\Desktop\\EncryptedPackages");
					_mkdir (szHomedrive);
					
					strcat_s (szHomedrive, SIZE_STRING, "\\EncryptedPackage.exe");

					
					ZeroMemory (szProposed, SIZE_STRING);
					m_filehandler.CheckDuplicatedFile (szHomedrive, szProposed);
					
					if (m_busingownpackagepath == false) {
						memCombined.SaveToFile (szProposed);
					} else {
						memCombined.SaveToFile (m_szOutputfile);
					}
					
					memDecryptor.Clear ();
					memCombined.Clear ();
					m_memPackage.Clear ();

				}
			}

			OutputStatusText ("Encryption Completed: ", m_szInputfiletitle);

			if (m_cmdmode == APP_ENCRYPT) {
				PostQuitMessage (0);
			} else {
				if (m_bcreatepackage == true) {
					
					char szmessage[SIZE_STRING];
					ZeroMemory (szmessage, SIZE_STRING);

					if (m_busingownpackagepath == false) {
						sprintf_s (szmessage, SIZE_STRING, "Encrypted Package has been created successfully, and has been saved to %s.", szProposed);
					} else {
						sprintf_s (szmessage, SIZE_STRING, "Encrypted Package has been created successfully, and has been saved to %s.", m_szOutputfile);
					}
										
					MessageBox (m_hwnd, szmessage, "CedeCrypt Encryption", MB_OK | MB_ICONINFORMATION);

				} else {
					MessageBox (m_hwnd, "File encrypted successfully.", "CedeCrypt Encryption", MB_OK | MB_ICONINFORMATION);

					
					if (m_busingtimings == true) {
						m_pdiag->OutputInt ("Cipher Time taken: ", m_finishtime - m_starttime);
					}

				}

				SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);
				SendMessage (m_hwndtotalprogress, PBM_SETPOS, 0, 0L);

				SetDlgItemText (m_hwnd, ID_LBLPHASE, "");
				SetDlgItemText (m_hwnd, ID_LBLTOTAL, "");
			}
			
		}

		if (m_appmode == MODE_DECRYPT) {
			m_checksum = CalcOutputChecksum ();
			m_pdiag->OutputInt ("Output checksum: ", m_checksum);

			if (m_inputchecksum != m_checksum) {
				bDonotdelete = true;
				OutputStatusText ("Decryption Failed: ", m_szInputfiletitle);
				OutputStatusText ("Check that you have entered the correct decryption password.");

				MessageBox (m_hwnd, "Decryption failed. Check that you have entered the correct password.", "CedeCrypt Decryption", MB_OK | MB_ICONEXCLAMATION);

			} else {
				m_fileoutput.SaveToFile (m_szOutputfile);

				OutputStatusText ("Decryption Completed: ", m_szInputfiletitle);
				MessageBox (m_hwnd, "File decrypted successfully.", "CedeCrypt Decryption", MB_OK | MB_ICONINFORMATION);
			}
			
			SendMessage (m_hwndprogress, PBM_SETPOS, 0, 0L);
			SendMessage (m_hwndtotalprogress, PBM_SETPOS, 0, 0L);

			SetDlgItemText (m_hwnd, ID_LBLPHASE, "");
			SetDlgItemText (m_hwnd, ID_LBLTOTAL, "");
			
		}
	}

	if (m_bDeleteorig == true) {
		if (bDonotdelete == false) {
			if (m_bcreatepackage == false) {
				// Securely delete the file using DOD standards.
				m_fileshred.SecureDelete (m_szInputfile);
			}
		}
	}

	if (m_bShellmode == true || m_bCommandmode == true) {
		if (m_bAwaitingcancel == true) {
			if (m_bCancelquestionshown == false) {
				m_bCancelquestionshown = true;
				
				if (MessageBox (NULL, "If you cancel now, some of your files may be encrypted and others may not. Are you sure you wish to cancel?", "Cancel now?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
					PostQuitMessage (0);
					m_bCancelquestionshown = false;
				} else {
					m_bAwaitingcancel = false;
					m_bCancelquestionshown = false;
				}
			}
		}
	}
}

void MainWindow::GetFileList (char *szFolderPath)
{
	m_dlFilelist.Clear ();
	m_iFilepointer = 0;

	char szPath[SIZE_STRING];
	ZeroMemory (szPath, SIZE_STRING);

	char szFilepath[SIZE_STRING];

	strcpy_s (szPath, SIZE_STRING, m_szInputfolder);
	strcat_s (szPath, SIZE_STRING, "\\*.*");

	m_pdiag->OutputText ("Scanning Path: ", szPath);

	struct _finddata_t c_file;
	long hFile;

	if( (hFile = _findfirst(szPath, &c_file )) == -1L )
		m_pdiag->OutputText ("MainWindow: No Files found.");
	else
	{
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			
			ZeroMemory (szFilepath, SIZE_STRING);			
			strcpy_s (szFilepath, SIZE_STRING, m_szInputfolder);
			strcat_s (szFilepath, SIZE_STRING, "\\");
			strcat_s (szFilepath, SIZE_STRING, c_file.name);

			m_dlFilelist.AddItem (szFilepath, SIZE_STRING, false);
			m_pdiag->OutputText ("MainWindow: File: ", szFilepath);
		}
		while( _findnext(hFile, &c_file ) == 0 )
		{
			if ((c_file.attrib & _A_SUBDIR) != 16) {

				ZeroMemory (szFilepath, SIZE_STRING);			
				strcpy_s (szFilepath, SIZE_STRING, m_szInputfolder);
				strcat_s (szFilepath, SIZE_STRING, "\\");
				strcat_s (szFilepath, SIZE_STRING, c_file.name);

				m_dlFilelist.AddItem (szFilepath, SIZE_STRING, false);
				m_pdiag->OutputText ("MainWindow: File: ", szFilepath);
			}
		}
		_findclose( hFile );
	}

	m_pdiag->OutputText ("MainWindow: All Files listed.");
}

void MainWindow::OnCryptEvent (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Notify the communications object of an encryption event message.
	m_crypt.NotifyEvent (hWnd, wParam, lParam);
	
	if (wParam == CRYPT_COMPLETE) {		
		m_pdiag->OutputText ("CRYPT_COMPLETE received.");
		
		SendMessage (m_hwndprogress, PBM_SETPOS, 100, 0L);
		
		if (m_filemode == MODE_TEXT) {
			// Put the encrypted contents back into the clipboard
			// in text form.
			//MessageBox (NULL, "Ready to save encrypted text", "Encryption", MB_OK);
			SaveEncryptedText ();
		} else {
			SaveEncryptedFile ();
		}

		if (m_filemode == MODE_FOLDER) {
			if (m_iFilepointer < m_dlFilelist.GetNumItems ()-1) {
				m_iFilepointer ++;


				ZeroMemory (m_szInputfile, SIZE_STRING);
				ZeroMemory (m_szOutputfile, SIZE_STRING);

				

				if (m_appmode == MODE_DECRYPT) {

					m_dlFilelist.GetItem (m_szInputfile, m_iFilepointer);

					if (strncmp (m_szInputfile+strlen(m_szInputfile)-4, ".ccr", 4) == 0) {
						strncpy_s (m_szOutputfile, SIZE_STRING, m_szInputfile, strlen(m_szInputfile)-4);						
					} else {
						strcpy_s (m_szOutputfile, SIZE_STRING, m_szInputfile);
						strcat_s (m_szOutputfile, SIZE_STRING, ".dec");
					}
				}

				if (m_appmode == MODE_ENCRYPT) {
					m_dlFilelist.GetItem (m_szInputfile, m_iFilepointer);
					m_dlFilelist.GetItem (m_szOutputfile, m_iFilepointer);
					strcat_s (m_szOutputfile, SIZE_STRING, ".ccr");
				}				

				m_pdiag->OutputText ("Infile: ", m_szInputfile);

				DoEncryption ();
			}
		}

		/*
		if (m_state == COMMS_CRYPT_WAITING) {

			m_state = COMMS_CRYPT_READY;
			if (m_cryptmode == CRYPT_MODE_DECRYPT) {
				// We've just decrypted a message, it needs to be passed to the
				// messaging class for action
				PostMessage (hWnd, COMMS_MESSAGEREADY, 0, 0);
			}
			if (m_cryptmode == CRYPT_MODE_ENCRYPT) {
				// We've just encrypted a message, so it needs to be sent
				// to the relevant client.
				SendCryptData ();
			}
		} else {
			OutputText ("Comms: Unexpected CRYPT_COMPLETE received!!");
			m_state = COMMS_CRYPT_READY;
		}
		*/
	}

	if (wParam == CRYPT_USERPROGRESS) {
		//OutputStatusInt ("Progress: ", lParam);
		//SendMessage(m_hwndprogress, PBM_DELTAPOS, lParam, 0L);
		//ID_LBLPHASE

		SetDlgItemText (m_hwnd, ID_LBLPHASE, "Applying Cipher phase 2...");
		SendMessage (m_hwndprogress, PBM_SETPOS, lParam, 0L);
		if (m_bShellmode == true || m_bCommandmode == true) {
			if (m_bAwaitingcancel == true) {
				
				if (m_bCancelquestionshown == false) {
					m_bCancelquestionshown = true;
					
					if (MessageBox (NULL, "If you cancel now, some of your files may be encrypted and others may not. Are you sure you wish to cancel?", "Cancel now?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
						PostQuitMessage (0);
						m_bCancelquestionshown = false;
					} else {
						m_bAwaitingcancel = false;
						m_bCancelquestionshown = false;
					}
				}
				
			}
		}
	}

	if (wParam == CRYPT_RUSERPROGRESS) {
		//OutputStatusInt ("Progress: ", lParam);
		//SendMessage(m_hwndprogress, PBM_DELTAPOS, lParam, 0L);
		SetDlgItemText (m_hwnd, ID_LBLPHASE, "Applying Cipher phase 1...");
		SendMessage (m_hwndprogress, PBM_SETPOS, lParam, 0L);
		if (m_bShellmode == true || m_bCommandmode == true) {
			if (m_bAwaitingcancel == true) {
				if (m_bCancelquestionshown == false) {
					m_bCancelquestionshown = true;
					
					if (MessageBox (NULL, "If you cancel now, some of your files may be encrypted and others may not. Are you sure you wish to cancel?", "Cancel now?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
						PostQuitMessage (0);
						m_bCancelquestionshown = false;
					} else {
						m_bAwaitingcancel = false;
						m_bCancelquestionshown = false;
					}
				}
				
			}
		}
	}
}

int MainWindow::Resourcetomem (LPCTSTR ResourceName, MemoryBuffer *memResource) {
	HRSRC hResource;
	hResource = FindResource (GetModuleHandle (NULL), ResourceName, "BINARY");
	if (hResource == NULL) {
		//DWORD dwError;
		//dwError = GetLastError ();
		
		//ShowInt (dwError);
		//MessageBox (NULL, "Could not locate internal resource.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -2;
	} else {
		HGLOBAL hLoad = LoadResource (GetModuleHandle (NULL), hResource);
		void *data = LockResource (hLoad);
		size_t sz = SizeofResource (GetModuleHandle (NULL), hResource);
		
		memResource->SetSize (sz);

		memResource->Write (data, 0, sz);

		return 0;
	}
}

void MainWindow::CombineKeys (MemoryBuffer *memKeyPublic, MemoryBuffer *memKeyPrivate, MemoryBuffer *memKeyOut)
{
	char szPublic[SIZE_SMALLINT];
	char szPrivateOne[SIZE_SMALLINT];
	char szPrivateTwo[SIZE_SMALLINT];
	char szComb[SIZE_NAME];
	char szCurrentResult[SIZE_NAME];

	char szCompleteCombination[SIZE_VERYLARGESTRING];

	int b = 0;
	int p = 0;
	int iPublic = 0;
	int iPrivateOne = 0;
	int iPrivateTwo = 0;
	int iPaddingAmount = 0;
	int iTotallength = 0;

	unsigned long long ullMultOne = 0;
	unsigned long long ullMultTwo = 0;

	int iSmallest = 0;

	if (memKeyPublic->GetSize () >= memKeyPrivate->GetSize ()) {
		iSmallest = memKeyPrivate->GetSize ();
	} else {
		iSmallest = memKeyPublic->GetSize ();
	}

	ZeroMemory (szCompleteCombination, SIZE_VERYLARGESTRING);

	for (b=0;b<iSmallest;b+=3) {
		
		ZeroMemory (szPublic, SIZE_SMALLINT);
		strncpy (szPublic, (char *) memKeyPublic->GetBuffer ()+b, 3);

		ZeroMemory (szPrivateOne, SIZE_SMALLINT);
		ZeroMemory (szPrivateTwo, SIZE_SMALLINT);
		
		if (b >= 3) {
			strncpy (szPrivateOne, (char *) memKeyPrivate->GetBuffer ()+b, 3);
			strncpy (szPrivateTwo, (char *) memKeyPrivate->GetBuffer ()+(b-3), 3);
		} else {
			strncpy (szPrivateOne, (char *) memKeyPrivate->GetBuffer ()+b, 3);
			strncpy (szPrivateTwo, (char *) memKeyPrivate->GetBuffer ()+(b+3), 3);
		}

		iPublic = atoi (szPublic);
		iPrivateOne = atoi (szPrivateOne);
		iPrivateTwo = atoi (szPrivateTwo);

		ullMultOne = iPublic*iPrivateOne;
		ullMultTwo = ullMultOne*iPrivateTwo;

		ZeroMemory (szComb, SIZE_NAME);
		_ui64toa (ullMultTwo, szComb, 10);

		iPaddingAmount = 11-strlen(szComb);

		// Now pad the result containing our multiplied number with zero's at the beginning
		// the objective is that the end result string is always the same length;
		ZeroMemory (szCurrentResult, SIZE_NAME);
		for (p=0;p<iPaddingAmount;p++) {
			strcat_s (szCurrentResult, SIZE_NAME, "0");
		}
		// Add the combination result to the padded string
		strcat_s (szCurrentResult, SIZE_NAME, szComb);
		
		// Calculate the total length of all the current result strings
		iTotallength+=strlen (szCurrentResult);

		strcat_s (szCompleteCombination, SIZE_VERYLARGESTRING, szCurrentResult);

		//m_pdiag->OutputText ("szResult: ", szCurrentResult);
	}

	m_pdiag->OutputInt ("iTotallength: ", iTotallength);
	memKeyOut->SetSize (iTotallength);
	memKeyOut->Write (szCompleteCombination, 0, iTotallength);
}

void MainWindow::RSAPrivateFromPublic ()
{
	// Function to take a public key, and create a private key based on that public key.

	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	// Now Generate our own exchange key
	
	bRes = CryptGenKey (hProvider, CALG_RSA_KEYX, CRYPT_EXPORTABLE | 0x08000000, &hSecondGeneratedKey);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptGenKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptGenKey Ok.");
	}
	
	// First we need to import the public key
	m_memImportedPublicKey.ReadFromFile ("C:\\Temp\\RSACrypt\\ExportedPublicKey.dat");
	
	DWORD dwImportedKeyLength = m_memImportedPublicKey.GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) m_memImportedPublicKey.GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPublic);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	// Now export the PRIVATE key, but this time base it on the public key we imported
	DWORD dwPrivateKeyLen = 0;
	bRes = CryptExportKey (hSecondGeneratedKey, hImportedPublic, PUBLICKEYBLOB, 0, NULL, &dwPrivateKeyLen); // first get the size of the buffer we need
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Ok. Size needed: ", dwPrivateKeyLen);
		m_memExportedSecondPrivateKey.SetSize (dwPrivateKeyLen);
		
		// Now actually get the private key
		bRes = CryptExportKey (hSecondGeneratedKey, hImportedPublic, PUBLICKEYBLOB, 0, (BYTE *) m_memExportedSecondPrivateKey.GetBuffer (), &dwPrivateKeyLen);
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Failed - GetLastError(): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Ok. Bytes Exported: ", dwPrivateKeyLen);
			m_memExportedSecondPrivateKey.SaveToFile ("C:\\Temp\\RSACrypt\\SecondExportedPrivateKey.dat");
		}
	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

void MainWindow::SecurePasswordUsingCanvas ()
{
	// Function to encrypt the users password using the 2048 bit RSA Public key
	// held as a resource but this time in the public key canvas. Decryption of this password is possible using
	// the equivalent private key which would have been generated by the customer using our Distrubution setup program.

	m_memCanvasEncrypted.Clear ();
	m_memDataBuffer.Clear ();

	// Firstly, check if there is even a canvas key present
	if (m_bCanvasPublicKeypresent == false) {
		return; // No canvas key - exit out immediately.
	}

	m_pdiag->OutputText ("Securing password using canvas key...");

	// If we got here then the public key is ok
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	// Now import the public key
	DWORD dwImportedKeyLength = m_memCanvasPublicKey.GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) m_memCanvasPublicKey.GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPublic);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	// Now encrypt some data using this key
	m_memDataBuffer.SetSize (strlen (m_szPassword));
	m_memDataBuffer.Write (m_szPassword, 0, strlen (m_szPassword));
		
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();
	bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, NULL, &dwDataBufferLen, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Ok. - Size needed: ", dwDataBufferLen);
		// Set the size of the destination buffer, and copy the source plaintext data to the encryption buffer
		m_memCanvasEncrypted.SetSize (dwDataBufferLen);
		m_memCanvasEncrypted.Write (m_memDataBuffer.GetBuffer (), 0, m_memDataBuffer.GetSize ());
		dwDataBufferLen = m_memDataBuffer.GetSize (); // Important - this informs the function of how much data actually needs encrypting.

		// Now actually encrypt the data
		bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, (BYTE *) m_memCanvasEncrypted.GetBuffer (), &dwDataBufferLen, m_memCanvasEncrypted.GetSize ());
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptEncrypt Failed - GetLastError (): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptEncrypt - Ok. Bytes encrypted: ", dwDataBufferLen);
			//m_memEncrypted.SaveToFile ("C:\\Temp\\RSACrypt\\EncryptedDataFromPublicKey2.dat");
		}
	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}


void MainWindow::SecurePassword ()
{
	// Function to encrypt the users password using the 8192 bit RSA Public key
	// held as a resource. Decryption of this password is only possible using
	// the special CedeSoft Private key which is held by CedeSoft ONLY!

	// If the application is in text only mode, then we encrypt the users
	// password using the 384 bit key which is also included as a resource - the reason
	// for using a smaller key for text files is that we do not want the text encrypted
	// version to be too big, as these will be pasted into ie windows, email windows, im
	// windows and so fourth

	m_memImportedPublicKey.Clear ();
	m_memEncrypted.Clear ();
	m_memDataBuffer.Clear ();

	if (m_filemode == MODE_TEXT) {
		if (Resourcetomem (MAKEINTRESOURCE (IDB_TEXTPUBLICKEY), &m_memImportedPublicKey) != 0) {
			MessageBox (NULL, "Critical Error has occured.", "Serious Error", MB_ICONEXCLAMATION | MB_OK);
			PostQuitMessage (0);
		}
	} else {
		if (Resourcetomem (MAKEINTRESOURCE (IDB_PUBLICKEY), &m_memImportedPublicKey) != 0) {
			MessageBox (NULL, "Critical Error has occured.", "Serious Error", MB_ICONEXCLAMATION | MB_OK);
			PostQuitMessage (0);
		}	
	}
	

	// If we got here then the public key is ok
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	// Now import the public key
	DWORD dwImportedKeyLength = m_memImportedPublicKey.GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) m_memImportedPublicKey.GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPublic);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	// Now encrypt some data using this key
	m_memDataBuffer.SetSize (strlen (m_szPassword));
	m_memDataBuffer.Write (m_szPassword, 0, strlen (m_szPassword));
		
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();
	bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, NULL, &dwDataBufferLen, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Ok. - Size needed: ", dwDataBufferLen);
		// Set the size of the destination buffer, and copy the source plaintext data to the encryption buffer
		m_memEncrypted.SetSize (dwDataBufferLen);
		m_memEncrypted.Write (m_memDataBuffer.GetBuffer (), 0, m_memDataBuffer.GetSize ());
		dwDataBufferLen = m_memDataBuffer.GetSize (); // Important - this informs the function of how much data actually needs encrypting.

		// Now actually encrypt the data
		bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, (BYTE *) m_memEncrypted.GetBuffer (), &dwDataBufferLen, m_memEncrypted.GetSize ());
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptEncrypt Failed - GetLastError (): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptEncrypt - Ok. Bytes encrypted: ", dwDataBufferLen);
			//m_memEncrypted.SaveToFile ("C:\\Temp\\RSACrypt\\EncryptedDataFromPublicKey2.dat");
		}
	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

void MainWindow::EncryptUsingPublic ()
{
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}
	
	// First we need to import the public key
	m_memImportedPublicKey.ReadFromFile ("C:\\Temp\\RSACrypt\\ExportedPublicKey2.dat");
	
	DWORD dwImportedKeyLength = m_memImportedPublicKey.GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) m_memImportedPublicKey.GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPublic);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	// Now encrypt some data using this key
	m_memDataBuffer.ReadFromFile ("C:\\Temp\\RSACrypt\\PlaintextData.txt");
	
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();
	bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, NULL, &dwDataBufferLen, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Ok. - Size needed: ", dwDataBufferLen);
		// Set the size of the destination buffer, and copy the source plaintext data to the encryption buffer
		m_memEncrypted.SetSize (dwDataBufferLen);
		m_memEncrypted.Write (m_memDataBuffer.GetBuffer (), 0, m_memDataBuffer.GetSize ());
		dwDataBufferLen = m_memDataBuffer.GetSize (); // Important - this informs the function of how much data actually needs encrypting.

		// Now actually encrypt the data
		bRes = CryptEncrypt (hImportedPublic, 0, TRUE, NULL, (BYTE *) m_memEncrypted.GetBuffer (), &dwDataBufferLen, m_memEncrypted.GetSize ());
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptEncrypt Failed - GetLastError (): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptEncrypt - Ok. Bytes encrypted: ", dwDataBufferLen);
			m_memEncrypted.SaveToFile ("C:\\Temp\\RSACrypt\\EncryptedDataFromPublicKey2.dat");
		}
	}


	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

bool MainWindow::GetRecoveryPassword (MemoryBuffer *memPrivateKey, MemoryBuffer *memEncpassword)
{
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return false;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	DWORD dwImportedKeyLength = memPrivateKey->GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) memPrivateKey->GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPrivate);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		MessageBox (NULL, "Recovery Failed. Your recovery key may be invalid.", "Recovery Failed.", MB_ICONEXCLAMATION | MB_OK);
		return false;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	DWORD dwDataBufferLen = memEncpassword->GetSize ();

	// Now actually encrypt the data
	bRes = CryptDecrypt (hImportedPrivate, 0, TRUE, NULL, (BYTE *) memEncpassword->GetBuffer (), &dwDataBufferLen);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptDecrypt Failed - GetLastError (): ", GetLastError ());
		MessageBox (NULL, "Recovery Failed. Your recovery key may be invalid.", "Recovery Failed.", MB_ICONEXCLAMATION | MB_OK);
	} else {
		m_pdiag->OutputInt ("CryptDecrypt - Ok. Bytes decrypted: ", dwDataBufferLen);
		
		// Now copy the number of bytes actually decrypted into the decrypted buffer
		m_memDecrypted.SetSize (dwDataBufferLen);
		m_memDecrypted.Write (m_memDataBuffer.GetBuffer (), 0, dwDataBufferLen);
		
		char szRecoveredPassword[SIZE_STRING];
		ZeroMemory (szRecoveredPassword, SIZE_STRING);

		memcpy (szRecoveredPassword, (char *) m_memDecrypted.GetBuffer (), m_memDecrypted.GetSize ());

		MessageBox (NULL, szRecoveredPassword, "Recovered Password", MB_ICONINFORMATION | MB_OK);
		

	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
	
	return true;
}

void MainWindow::RecoverPassword ()
{
	// Function to recover the users password from the encrypted file.
	// This requires the 8192 bit RSA Private Key held by CedeSoft. Without this key
	// recovery will not be possible.
	m_appmode = MODE_DECRYPT;

	if (OpenSingleFile () == true) {
		if (OpenPrivateKey () == true) {
			
			if (m_fileinput.ReadFromFile (m_szInputfile) == true) {

				// u-int	   u-int		   int			  byte			 byte
				// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | ENCRYPTED FILEDATA

				int ipointer = 0;
				int ibdsize = 0;
				memcpy (&m_inputid, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
				ipointer+=sizeof (unsigned int);

				if (m_inputid == m_ccryptidprev1) {
					MessageBox (m_hwnd, "This file was encrypted using CedeCrypt v2.1, and is incompatible with this version. Please use CedeCrypt v2.1 to decrypt this file. A backward compatibility feature will be implemented in future versions.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
				} else {			
					if (m_inputid != m_ccryptid && m_inputid != m_ccryptid2) {
						MessageBox (m_hwnd, "This is not a valid CedeCrypt v2.3 encoded file. Please check the input filename and try again.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
					} else {

						memcpy (&m_inputchecksum, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
						ipointer+=sizeof (unsigned int);

						// Skip over the backdoor for decryption, but first we need it's size
						memcpy (&ibdsize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
						ipointer+=sizeof (int);

						m_memDataBuffer.SetSize (ibdsize);
						m_memDataBuffer.Write ((BYTE *) m_fileinput.GetBuffer ()+ipointer, 0, ibdsize);
						ipointer+=ibdsize; // the backdoor data

						// Now read in the private key
						if (m_memImportedPrivateKey.ReadFromFile (m_szPrivateKey) == true) {
							GetRecoveryPassword (&m_memImportedPrivateKey, &m_memDataBuffer);
						} else {
							OutputStatusText ("Unable to read input file: ", m_szPrivateKey);
					
							char szError [SIZE_STRING];
							ZeroMemory (szError, SIZE_STRING);

							strcpy_s (szError, SIZE_STRING, "Unable to open: ");
							strcat_s (szError, SIZE_STRING, m_szPrivateKey);

							MessageBox (NULL, szError, "Read Error", MB_OK | MB_ICONEXCLAMATION);
						}
					}
				}

			} else {

				OutputStatusText ("Unable to read input file: ", m_szInputfile);
				
				char szError [SIZE_STRING];
				ZeroMemory (szError, SIZE_STRING);

				strcpy_s (szError, SIZE_STRING, "Unable to open: ");
				strcat_s (szError, SIZE_STRING, m_szInputfile);

				MessageBox (NULL, szError, "Read Error", MB_OK | MB_ICONEXCLAMATION);
			}

		} else {
			m_pdiag->OutputText ("Unable to open private key.");
		}
	} else {
		m_pdiag->OutputText ("Unable to open input file.");
	}
}

void MainWindow::RecoverCanvasPassword ()
{
	// Function to recover the users password from the encrypted file.
	// This requires the 2048 bit RSA Private Key held by the customer which will have been generated using the distribution program. Without this key
	// recovery will only be possible by CedeSoft using the Cedesoft key.
	
	m_appmode = MODE_DECRYPT;

	if (OpenSingleFile () == true) {
		
		if (m_fileinput.ReadFromFile (m_szInputfile) == true) {
			// u-int	   u-int		   int			  byte			 int				  byte				   int            char       byte
			// VERSIONID | CHECKSUM-ORIG | BACKDOORSIZE | BACKDOORDATA | CANVASBACKDOORSIZE | CANVASBACKDOORDATA | DISTNAMESIZE | DISTNAME | ENCRYPTED FILEDATA

			int ipointer = 0;
			int ibdsize = 0;
			int ibdcanvassize = 0;
			int idistnamesize = 0;

			char szDistributionname[SIZE_STRING];

			memcpy (&m_inputid, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
			ipointer+=sizeof (unsigned int);

			if (m_inputid == m_ccryptidprev1) {
				MessageBox (m_hwnd, "This file was encrypted using CedeCrypt v2.1, and is incompatible with this version. Please use CedeCrypt v2.1 to decrypt this file. A backward compatibility feature will be implemented in future versions.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
			} else {			
				if (m_inputid != m_ccryptid) {
					MessageBox (m_hwnd, "Recovery is not possible on this version of the CedeCrypt encoded file. To recover this file please contact Cedesoft.", "CedeCrypt Invalid File", MB_OK | MB_ICONEXCLAMATION);
				} else {

					memcpy (&m_inputchecksum, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (unsigned int));
					ipointer+=sizeof (unsigned int);

					// Skip over the cedesoft backdoor for decryption, but first we need it's size
					memcpy (&ibdsize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
					ipointer+=sizeof (int);
					ipointer+=ibdsize; // skip the cedesoft backdoor data

					// Now get the canvas backdoor size and data (the user should have the private key for this)
					memcpy (&ibdcanvassize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
					ipointer+=sizeof (int);

					if (ibdcanvassize == 0) {
						MessageBox (NULL, "Recovery not possible. This file does not contain any customer Recovery data. To recover this file, please contact Cedesoft.", "Recovery not possible", MB_OK | MB_ICONEXCLAMATION);
						return;
					}

					m_memDataBuffer.SetSize (ibdcanvassize);
					m_memDataBuffer.Write ((BYTE *) m_fileinput.GetBuffer ()+ipointer, 0, ibdcanvassize);
					ipointer+=ibdcanvassize; // the backdoor data

					// Now get the name of this Distribution
					memcpy (&idistnamesize, (BYTE *) m_fileinput.GetBuffer ()+ipointer, sizeof (int));																	
					ipointer+=sizeof (int);

					ZeroMemory (szDistributionname, SIZE_STRING);
					strncpy_s (szDistributionname, SIZE_STRING, (char *) m_fileinput.GetBuffer ()+ipointer, idistnamesize);

					// Now display a message to the user.
					char szMessage[SIZE_STRING];
					sprintf_s (szMessage, SIZE_STRING, "Recovery data found - you must now provide the recovery key for the distribution: %s", szDistributionname);
					MessageBox (NULL, szMessage, "Recovery Information", MB_OK | MB_ICONINFORMATION);

					if (OpenPrivateKey () == true) {
						
						// Now read in the private key
						if (m_memImportedPrivateKey.ReadFromFile (m_szPrivateKey) == true) {
							GetRecoveryPassword (&m_memImportedPrivateKey, &m_memDataBuffer);
						} else {
							OutputStatusText ("Unable to read input file: ", m_szPrivateKey);
					
							char szError [SIZE_STRING];
							ZeroMemory (szError, SIZE_STRING);

							strcpy_s (szError, SIZE_STRING, "Unable to open: ");
							strcat_s (szError, SIZE_STRING, m_szPrivateKey);

							MessageBox (NULL, szError, "Read Error", MB_OK | MB_ICONEXCLAMATION);
						}

					} else {
						m_pdiag->OutputText ("Unable to open private key.");
					}
				}
			}				

		} else {
				OutputStatusText ("Unable to read input file: ", m_szInputfile);
				
				char szError [SIZE_STRING];
				ZeroMemory (szError, SIZE_STRING);

				strcpy_s (szError, SIZE_STRING, "Unable to open: ");
				strcat_s (szError, SIZE_STRING, m_szInputfile);

				MessageBox (NULL, szError, "Read Error", MB_OK | MB_ICONEXCLAMATION);
		}
		

	} else {
		m_pdiag->OutputText ("Unable to open input file.");
	}
}

void MainWindow::DecryptUsingPrivate ()
{
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}
	
	// First we need to import the private key
	m_memImportedPrivateKey.ReadFromFile ("C:\\Temp\\RSACrypt\\ExportedPrivateKey2.dat");
	
	DWORD dwImportedKeyLength = m_memImportedPrivateKey.GetSize ();
	bRes = CryptImportKey (hProvider, (BYTE *) m_memImportedPrivateKey.GetBuffer (), dwImportedKeyLength, 0, NULL, &hImportedPrivate);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptImportKey Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptImportKey Ok.");
	}

	// Now encrypt some data using this key
	m_memDataBuffer.ReadFromFile ("C:\\Temp\\RSACrypt\\EncryptedDataFromPublicKey2.dat");
	
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();

	// Now actually encrypt the data
	bRes = CryptDecrypt (hImportedPrivate, 0, TRUE, NULL, (BYTE *) m_memDataBuffer.GetBuffer (), &dwDataBufferLen);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptDecrypt Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptDecrypt - Ok. Bytes decrypted: ", dwDataBufferLen);
		
		// Now copy the number of bytes actually decrypted into the decrypted buffer
		m_memDecrypted.SetSize (dwDataBufferLen);
		m_memDecrypted.Write (m_memDataBuffer.GetBuffer (), 0, dwDataBufferLen);
		
		m_memDecrypted.SaveToFile ("C:\\Temp\\RSACrypt\\DecryptedFromPrivateKey2.txt");
	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

void MainWindow::RSAGenandEncrypt ()
{
	// Sample routine to generate a public and private key, and use these keys to encrypt
	// a file in C:\Temp.
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	// Now Generate an encryption key - Full RSA Exchange Key
	// 0x20000000 is a 8192 bit key - used for backdoor encryption of files and folders
	// 0x01800000 is a 384 bit key - used for backdoor encryption of text only encryption

	bRes = CryptGenKey (hProvider, CALG_RSA_KEYX, CRYPT_EXPORTABLE | 0x01800000, &hGeneratedKey);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptGenKey Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptGenKey Ok.");
	}

	// Now export the PUBLIC key only
	DWORD dwPublicKeyLen = 0;
	bRes = CryptExportKey (hGeneratedKey, 0, PUBLICKEYBLOB, NULL, NULL, &dwPublicKeyLen); // first get the size of the buffer we need
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptExportKey PUBLIC (sizeonly) Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptExportKey PUBLIC (sizeonly) Ok. Size needed: ", dwPublicKeyLen);
		m_memExportedPublicKey.SetSize (dwPublicKeyLen);
		// Now actually get the public key
		bRes = CryptExportKey (hGeneratedKey, 0, PUBLICKEYBLOB, NULL, (BYTE *) m_memExportedPublicKey.GetBuffer (), &dwPublicKeyLen);
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptExportKey PUBLIC Failed - GetLastError(): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptExportKey PUBLIC Ok. Bytes Exported: ", dwPublicKeyLen);
			m_memExportedPublicKey.SaveToFile ("C:\\Temp\\RSACrypt\\ExportedPublicKey2.dat");
		}
	}

	// Now export the PRIVATE key
	DWORD dwPrivateKeyLen = 0;
	bRes = CryptExportKey (hGeneratedKey, 0, PRIVATEKEYBLOB, NULL, NULL, &dwPrivateKeyLen); // first get the size of the buffer we need
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Ok. Size needed: ", dwPrivateKeyLen);
		m_memExportedPrivateKey.SetSize (dwPrivateKeyLen);
		// Now actually get the private key
		bRes = CryptExportKey (hGeneratedKey, 0, PRIVATEKEYBLOB, 0, (BYTE *) m_memExportedPrivateKey.GetBuffer (), &dwPrivateKeyLen);
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Failed - GetLastError(): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Ok. Bytes Exported: ", dwPrivateKeyLen);
			m_memExportedPrivateKey.SaveToFile ("C:\\Temp\\RSACrypt\\ExportedPrivateKey2.dat");
		}
	}

	// Now encrypt some data using this key
	m_memDataBuffer.ReadFromFile ("C:\\Temp\\RSACrypt\\PlaintextData.txt");
	
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();
	bRes = CryptEncrypt (hGeneratedKey, 0, TRUE, NULL, NULL, &dwDataBufferLen, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Ok. - Size needed: ", dwDataBufferLen);
		// Set the size of the destination buffer, and copy the source plaintext data to the encryption buffer
		m_memEncrypted.SetSize (dwDataBufferLen);
		m_memEncrypted.Write (m_memDataBuffer.GetBuffer (), 0, m_memDataBuffer.GetSize ());
		dwDataBufferLen = m_memDataBuffer.GetSize (); // Important - this informs the function of how much data actually needs encrypting.

		// Now actually encrypt the data
		bRes = CryptEncrypt (hGeneratedKey, 0, TRUE, NULL, (BYTE *) m_memEncrypted.GetBuffer (), &dwDataBufferLen, m_memEncrypted.GetSize ());
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptEncrypt Failed - GetLastError (): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptEncrypt - Ok. Bytes encrypted: ", dwDataBufferLen);
			m_memEncrypted.SaveToFile ("C:\\Temp\\RSACrypt\\EncryptedData2.dat");
		}
	}

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

void MainWindow::RSAGenerateKey ()
{
	// This is a generic function for development purposes only. It generates an RSA public and private key
	// The public key is stored in m_memGenPublic and the private key is stored in m_memGenPrivate. Initially
	// developed so that I could embed a public key into the key canvas which will be used by the distribution
	// setup package.

	// First we must aquire the encryption context from the encryption API.
	bool bRes = CryptAcquireContext (&hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

	if (bRes == false) {
		m_pdiag->OutputInt ("CryptAcquireContext Failed - GetLastError(): ", GetLastError ());
		return;
	} else {
		m_pdiag->OutputText ("CryptAquireContext Ok.");
	}

	// Now Generate an encryption key - Full RSA Exchange Key
	// 0x20000000 is a 8192 bit key - used for backdoor encryption of files and folders
	//			- users password is encrypted using this key and embedded into the encrypted file.
	// 0x01800000 is a 384 bit key - used for backdoor encryption of text only encryption
	// 0x08000000 is a 2048 bit key - used for backdoor encryption for inserted by setup distributor.

	bRes = CryptGenKey (hProvider, CALG_RSA_KEYX, CRYPT_EXPORTABLE | 0x08000000, &hGeneratedKey);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptGenKey Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptGenKey Ok.");
	}

	// Now export the PUBLIC key only
	DWORD dwPublicKeyLen = 0;
	bRes = CryptExportKey (hGeneratedKey, 0, PUBLICKEYBLOB, NULL, NULL, &dwPublicKeyLen); // first get the size of the buffer we need
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptExportKey PUBLIC (sizeonly) Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptExportKey PUBLIC (sizeonly) Ok. Size needed: ", dwPublicKeyLen);
		m_memGenPublic.SetSize (dwPublicKeyLen);
		// Now actually get the public key
		bRes = CryptExportKey (hGeneratedKey, 0, PUBLICKEYBLOB, NULL, (BYTE *) m_memGenPublic.GetBuffer (), &dwPublicKeyLen);
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptExportKey PUBLIC Failed - GetLastError(): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptExportKey PUBLIC Ok. Bytes Exported: ", dwPublicKeyLen);
			m_memGenPublic.SaveToFile ("C:\\Temp\\RSACrypt\\ExportedPublicKeyCustom.dat");
		}
	}

	// Now export the PRIVATE key
	DWORD dwPrivateKeyLen = 0;
	bRes = CryptExportKey (hGeneratedKey, 0, PRIVATEKEYBLOB, NULL, NULL, &dwPrivateKeyLen); // first get the size of the buffer we need
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptExportKey PRIVATE (sizeonly) Ok. Size needed: ", dwPrivateKeyLen);
		m_memGenPrivate.SetSize (dwPrivateKeyLen);
		// Now actually get the private key
		bRes = CryptExportKey (hGeneratedKey, 0, PRIVATEKEYBLOB, 0, (BYTE *) m_memGenPrivate.GetBuffer (), &dwPrivateKeyLen);
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Failed - GetLastError(): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptExportKey PRIVATE Ok. Bytes Exported: ", dwPrivateKeyLen);
			m_memGenPrivate.SaveToFile ("C:\\Temp\\RSACrypt\\GenPrivate.dat");
		}
	}

	/*
	// Now encrypt some data using this key
	m_memDataBuffer.ReadFromFile ("C:\\Temp\\RSACrypt\\PlaintextData.txt");
	
	DWORD dwDataBufferLen = m_memDataBuffer.GetSize ();
	bRes = CryptEncrypt (hGeneratedKey, 0, TRUE, NULL, NULL, &dwDataBufferLen, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Failed - GetLastError (): ", GetLastError ());
	} else {
		m_pdiag->OutputInt ("CryptEncrypt (sizeonly) Ok. - Size needed: ", dwDataBufferLen);
		// Set the size of the destination buffer, and copy the source plaintext data to the encryption buffer
		m_memEncrypted.SetSize (dwDataBufferLen);
		m_memEncrypted.Write (m_memDataBuffer.GetBuffer (), 0, m_memDataBuffer.GetSize ());
		dwDataBufferLen = m_memDataBuffer.GetSize (); // Important - this informs the function of how much data actually needs encrypting.

		// Now actually encrypt the data
		bRes = CryptEncrypt (hGeneratedKey, 0, TRUE, NULL, (BYTE *) m_memEncrypted.GetBuffer (), &dwDataBufferLen, m_memEncrypted.GetSize ());
		if (bRes == false) {
			m_pdiag->OutputInt ("CryptEncrypt Failed - GetLastError (): ", GetLastError ());
		} else {
			m_pdiag->OutputInt ("CryptEncrypt - Ok. Bytes encrypted: ", dwDataBufferLen);
			m_memEncrypted.SaveToFile ("C:\\Temp\\RSACrypt\\EncryptedData2.dat");
		}
	}
	*/

	// ******** END OF ENCRYPTION STUFF ***********
	bRes = CryptReleaseContext (hProvider, 0);
	if (bRes == false) {
		m_pdiag->OutputInt ("CryptReleaseContext Failed - GetLastError(): ", GetLastError ());
	} else {
		m_pdiag->OutputText ("CryptReleaseContext Ok.");
	}
}

void MainWindow::WritetoKeyCanvas ()
{
	// This function will take the generated public key and write into the appropriate
	// place in the key canvas. The file will then be saved to disk for inclusion
	// as a resource
	MemoryBuffer memKeyCanvas;

	char szKeytagstart[SIZE_STRING];
	char szKeytagend[SIZE_STRING];
	char szDatatagstart[SIZE_STRING];
	char szDatatagend[SIZE_STRING];

	ZeroMemory (szKeytagstart, SIZE_STRING);
	ZeroMemory (szKeytagend, SIZE_STRING);
	ZeroMemory (szDatatagstart, SIZE_STRING);
	ZeroMemory (szDatatagend, SIZE_STRING);

	strcpy_s (szKeytagstart, SIZE_STRING, "D169C884-9C83-11DD-8E99-DBE655D89593040257B6-9C84-11DD-81CB-80E855D89593");
	strcpy_s (szKeytagend, SIZE_STRING, "1106EEE0-9C84-11DD-B673-E8E855D89593163B798A-9C84-11DD-B19F-1EE955D89593");
	strcpy_s (szDatatagstart, SIZE_STRING, "57702DBA-9C84-11DD-906C-C8EC55D89593");
	strcpy_s (szDatatagend, SIZE_STRING, "75579822-9C84-11DD-9422-ABED55D89593");

	int iPointer = 0;

	if (Resourcetomem (MAKEINTRESOURCE (IDB_BINARY2), &memKeyCanvas) ==0 ) {
		
		// Now we have the resource
		//memKeyCanvas.SaveToFile ("C:\\Temp\\RSACrypt\\KeyCanvas.dat");
		iPointer += strlen (szKeytagstart);
		
		memKeyCanvas.Write ((char *) szDatatagstart, iPointer, strlen (szDatatagstart));
		iPointer += strlen (szDatatagstart);

		memKeyCanvas.Write (m_memGenPublic.GetBuffer (), iPointer, m_memGenPublic.GetSize ());
		iPointer += m_memGenPublic.GetSize ();

		memKeyCanvas.Write ((char *) szDatatagend, iPointer, strlen (szDatatagend));
		iPointer += strlen (szDatatagend);

		memKeyCanvas.SaveToFile ("C:\\Temp\\RSACrypt\\KeyCanvasPublicTest.dat");

	} else {
		MessageBox (NULL, "Write to canvas failed!", "Canvas Write Error", MB_OK);
	}

	memKeyCanvas.Clear ();
}

bool MainWindow::GetDistributionfromCanvas ()
{
	// This function will locate a distribution name from the distribution name canvas in the binary
	// resources. A distribution is basically just a piece of text identifying this particular distribution
	// of CedeCrypt now that we are allowing customers to generate their own recovery keys.
	MemoryBuffer memNameCanvas;
	
	char szDatatagstart[SIZE_STRING];
	char szDatatagend[SIZE_STRING];
	
	ZeroMemory (szDatatagstart, SIZE_STRING);
	ZeroMemory (szDatatagend, SIZE_STRING);
	ZeroMemory (m_szDistributionname, SIZE_STRING);

	strcpy_s (szDatatagstart, SIZE_STRING, "57702DBA-9C84-11DD-906C-C8EC55D89593");
	strcpy_s (szDatatagend, SIZE_STRING, "75579822-9C84-11DD-9422-ABED55D89593");

	int iPointer = 0;

	int iStartloc = 0;
	int iEndloc = 0;
	int i = 0;

	int iRealdatastart = 0;
	int iRealdatalength = 0;

	if (Resourcetomem (MAKEINTRESOURCE (IDB_BINARY3), &memNameCanvas) ==0 ) {
		
		for (i=0;i<memNameCanvas.GetSize ();i++) {
			
			if (memcmp ((char* ) szDatatagstart, (BYTE *) memNameCanvas.GetBuffer ()+i, strlen (szDatatagstart)) == 0) {
				iStartloc = i;
			}

			if (memcmp ((char *) szDatatagend, (BYTE *) memNameCanvas.GetBuffer ()+i, strlen (szDatatagend)) == 0) {
				iEndloc = i;
			}
		}


		// Finished scanning the file for tags, now display them
		m_pdiag->OutputInt ("Name Start Tag: ", iStartloc);
		m_pdiag->OutputInt ("Name End Tag: ", iEndloc);

		if (iStartloc > 0) { // Make sure the start tag is filled in
			
			if (iEndloc > iStartloc) { // Make sure the end tag is after the start tag
				
				iRealdatastart = iStartloc+strlen(szDatatagstart); // Setup the actual offset of the key data
				iRealdatalength = iEndloc - iRealdatastart; // Now get the length we need based on where end tag is

				if (iRealdatalength < SIZE_STRING) {
					strncpy_s (m_szDistributionname, SIZE_STRING,(char *) memNameCanvas.GetBuffer ()+iRealdatastart, iRealdatalength);
					m_pdiag->OutputText ("Distribution name from canvas successful: ", m_szDistributionname);
				} else {
					strcpy_s (m_szDistributionname, "CedeSoft Distribution.");
					m_pdiag->OutputText ("Distribution name exceeded max size");
				}
				
				
				return true;


			} else {
				strcpy_s (m_szDistributionname, "CedeSoft Distribution.");
				m_pdiag->OutputText ("Read from canvas failed! - End tag not found!");
				return false;
			}

		} else {
			strcpy_s (m_szDistributionname, "CedeSoft Distribution.");
			m_pdiag->OutputText ("Read from canvas failed! - Start tag not found!");
			return false;
		}

	} else {
		//MessageBox (NULL, "Read from Canvas failed!", "Canvas Read Error", MB_OK);
		strcpy_s (m_szDistributionname, "CedeSoft Distribution.");
		m_pdiag->OutputText ("Read from canvas failed! - Unable to read resource!");
		return false;
	}
}

bool MainWindow::GetKeyfromCanvas ()
{
	// This function will read the public key from the key canvas. The public key read from here
	// will be one that will have been generated and written by the distribution program.

	MemoryBuffer memKeyCanvas;
	
	char szDatatagstart[SIZE_STRING];
	char szDatatagend[SIZE_STRING];
	
	ZeroMemory (szDatatagstart, SIZE_STRING);
	ZeroMemory (szDatatagend, SIZE_STRING);
	
	strcpy_s (szDatatagstart, SIZE_STRING, "57702DBA-9C84-11DD-906C-C8EC55D89593");
	strcpy_s (szDatatagend, SIZE_STRING, "75579822-9C84-11DD-9422-ABED55D89593");

	int iPointer = 0;

	int iStartloc = 0;
	int iEndloc = 0;
	int i = 0;

	int iRealdatastart = 0;
	int iRealdatalength = 0;

	if (Resourcetomem (MAKEINTRESOURCE (IDB_BINARY2), &memKeyCanvas) ==0 ) {
		
		for (i=0;i<memKeyCanvas.GetSize ();i++) {
			
			if (memcmp ((char* ) szDatatagstart, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szDatatagstart)) == 0) {
				iStartloc = i;
			}

			if (memcmp ((char *) szDatatagend, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szDatatagend)) == 0) {
				iEndloc = i;
			}
		}


		// Finished scanning the file for tags, now display them
		m_pdiag->OutputInt ("Start Tag: ", iStartloc);
		m_pdiag->OutputInt ("End Tag: ", iEndloc);

		if (iStartloc > 0) { // Make sure the start tag is filled in
			
			if (iEndloc > iStartloc) { // Make sure the end tag is after the start tag
				
				iRealdatastart = iStartloc+strlen(szDatatagstart); // Setup the actual offset of the key data
				iRealdatalength = iEndloc - iRealdatastart; // Now get the length we need based on where end tag is

				m_memCanvasPublicKey.SetSize (iRealdatalength);
				m_memCanvasPublicKey.Write ((BYTE *) memKeyCanvas.GetBuffer ()+iRealdatastart, 0, iRealdatalength);

				// Now save the extracted public key to a file
				//m_memCanvasPublicKey.SaveToFile ("C:\\Temp\\RSACrypt\\ExtractedPublicKeyfromCanvas.dat");
				m_bCanvasPublicKeypresent = true;

				m_pdiag->OutputText ("Read from canvas successful - Public key extracted.");
				return true;


			} else {
				m_pdiag->OutputText ("Read from canvas failed! - End tag not found!");
				return false;
			}

		} else {
			m_pdiag->OutputText ("Read from canvas failed! - Start tag not found!");
			return false;
		}

	} else {
		//MessageBox (NULL, "Read from Canvas failed!", "Canvas Read Error", MB_OK);
		m_pdiag->OutputText ("Read from canvas failed! - Unable to read resource!");
		return false;
	}
}

bool MainWindow::FileExists (char *FileName)
{
    FILE* fp = NULL;

    //will not work if you do not have read permissions
    //to the file, but if you don't have read, it
    //may as well not exist to begin with.

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

void MainWindow::PrepareShellInfoPath ()
{
	int iRes = 0;
	char szAppData[SIZE_STRING];
	char szCompanyAppData[SIZE_STRING];
	char szProgramAppData[SIZE_STRING];
	char szInfoAppData[SIZE_STRING];

	ZeroMemory (szAppData, SIZE_STRING);
	ZeroMemory (szCompanyAppData, SIZE_STRING);
	ZeroMemory (szProgramAppData, SIZE_STRING);
	ZeroMemory (szInfoAppData, SIZE_STRING);

	if (GetEnvironmentVariable ("APPDATA", szAppData, SIZE_STRING) == 0) {
		// retrieving environment variable failed.
		//m_diag.OutputText ("Failed to retrieve APPDATA environment variable!");		
		return;
	} 

	strcpy_s (szCompanyAppData, SIZE_STRING, szAppData);
	strcat_s (szCompanyAppData, SIZE_STRING, "\\CedeSoft");

	strcpy_s (szProgramAppData, SIZE_STRING, szCompanyAppData);
	strcat_s (szProgramAppData, SIZE_STRING, "\\CedeCrypt");

	strcpy_s (szInfoAppData, SIZE_STRING, szProgramAppData);
	strcat_s (szInfoAppData, SIZE_STRING, "\\ShellCommand.dat");

	// Set our member variable which is used by the other functions
	ZeroMemory (m_szShellInfoPath, SIZE_STRING);
	strcpy_s (m_szShellInfoPath, SIZE_STRING, szInfoAppData);
	
	return;
}

void MainWindow::PrepareTempCommandFile (char *pszOrigPath)
{
	// This function prepares the path to the temporary file used
	// for previewing encrypted files. IE when an encrypted file has been
	// double clicked, we first get the temporary environment variable
	// and add to it the name of the temporary file. This file
	// must be read only so that if any changes are attempted to the encrypted
	// preview file then this will have to be saved to another file.

	char szTemp[SIZE_STRING];
	ZeroMemory (szTemp, SIZE_STRING);

	if (GetEnvironmentVariable ("TEMP", szTemp, SIZE_STRING) == 0) {
		// failed to retrieve the temp environment variable
		return;
	}

	strcat_s (szTemp, SIZE_STRING, "\\CCryptedPreview");

	bool bExtensionFound = false;
	char szOrigExtension[SIZE_NAME];
	ZeroMemory (szOrigExtension, SIZE_NAME);

	GetFileExtension (bExtensionFound, pszOrigPath, szOrigExtension);

	if (bExtensionFound == true) {
		strcat_s (szTemp, SIZE_STRING, szOrigExtension);
	}

	// Copy the full path to a global temporary variable.
	ZeroMemory (m_szCommandTempFile, SIZE_STRING);
	strcpy_s (m_szCommandTempFile, SIZE_STRING, szTemp);
	//MessageBox (NULL, szTemp, "Test", MB_OK);
	if (FileExists (m_szCommandTempFile) == true) {
		SetFileAttributes (m_szCommandTempFile, FILE_ATTRIBUTE_NORMAL);
		DeleteFile (m_szCommandTempFile);
	}

}

bool MainWindow::IsDirectory (char *szPathname)
{
	struct _finddata_t c_file;
	long hFile;

	// Function to either return true or false if the supplied pathname
	// is a directory or a file. This is used by the shell check command
	bool bResult = false;

	if( (hFile = _findfirst(szPathname, &c_file )) == -1L ) {
		m_pdiag->OutputText ("IsDirectory: No Files.");
		bResult = false;
	} else {
		if ((c_file.attrib & _A_SUBDIR) != 16) {
			//dlSourceFiles.AddItem (c_file.name);
			//OutputText (c_file.name, ": AFILE" );
			m_pdiag->OutputText (szPathname, ": FILE");
			bResult = false;
		} else {
			// Pathname refers to a directory
			m_pdiag->OutputText (szPathname, ": DIRECTORY");
			bResult = true;
		}
		_findclose( hFile );
	}
	return bResult;
}

bool MainWindow::DoSessionEncryption ()
{
	//MessageBox (NULL, "Session decryption would have started.", "Test", MB_OK);
	bool bproceed = false;

	m_appmode = MODE_ENCRYPT;
	
	m_bDeleteorig = true; // Delete the original files after being worked on
	m_filemode = MODE_FOLDER;
	m_iFilepointer = 0;

	m_dlFilelist.Clear ();

	if (CheckProtectedModeFiles (true) == true) {
		
		if (strcmp (m_registry.ReadStringSetting (PROTECTEDFOLDERSPROMPTPDN), "yes") == 0) {
			if (m_pfrequest.GetAnswered () == false) {
				bproceed = false;
				m_pfrequest.Initialise (m_hwnd, 0);
				m_pfrequest.SetStartupMessage (false);
				m_pfrequest.Show ();
			} else {
				bproceed = true;
			}
		} else {
			bproceed = true;
		}
		
		if (bproceed == true) {
			Show ();
			SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
			SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

			ZeroMemory (m_szInputfile, SIZE_STRING);
			ZeroMemory (m_szOutputfile, SIZE_STRING);

			m_dlFilelist.GetItem (m_szInputfile, m_iFilepointer);
			m_dlFilelist.GetItem (m_szOutputfile, m_iFilepointer);
			strcat_s (m_szOutputfile, SIZE_STRING, ".ccr");
			
			if (m_bSessionpasswordpresent == true) {
				m_bUsesessionpassword = true;
				DoEncryption ();
			} else {
				m_passwindow.Initialise (m_hwnd, 0);

				if (m_appmode == MODE_ENCRYPT) {
					m_passwindow.SetEncryptMode (true);
				} else {
					m_passwindow.SetEncryptMode (false);
				}

				m_passwindow.SetSessionMode (true, true);
				m_passwindow.Show ();	
			}
			return true;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

void MainWindow::CheckSessionAction ()
{
	if (FileExists ("D:\\Enc.txt") == true) {
		DeleteFile ("D:\\Enc.txt");

		DoSessionEncryption ();
	}

	if (FileExists ("D:\\Dec.txt") == true) {
		DeleteFile ("D:\\Dec.txt");

		DoSessionDecryption ();
	}
}

bool MainWindow::CheckProtectedModeFiles (bool bEncrypt)
{
	char szListfile[SIZE_STRING];
	MemoryBuffer memList;
	int iCount = 0;
	int iCurlen = 0;
	char szCuritem[SIZE_STRING];
	int c = 0;
	unsigned long lPointer = 0;

	m_pfolders.GetListFilePath (szListfile);

	m_pdiag->OutputText ("ProtectedModeCheck - List file is:", szListfile);

	if (FileExists (szListfile) == true) {
		if (memList.ReadFromFile (szListfile) == true) {
			
			// Get the number of items in the list
			memcpy (&iCount, (BYTE *) memList.GetBuffer ()+lPointer, sizeof (int));
			lPointer+=sizeof (int);

			for (c=0;c<iCount;c++) {
				
				memcpy (&iCurlen, (BYTE *) memList.GetBuffer ()+lPointer, sizeof (int));
				lPointer+=sizeof (int);

				ZeroMemory (szCuritem, SIZE_STRING);
				memcpy (szCuritem, (char *) memList.GetBuffer ()+lPointer, iCurlen);
				lPointer+=iCurlen;
	
				// Add the current item to the list of items
				//SendMessage (m_lstfolders, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szCuritem);
				
				// Now list all ccr files in the current directory
				if (bEncrypt == true) {
					m_Dirscanner.ListAllNONCCRFiles (szCuritem, &m_dlFilelist);
				} else {
					m_Dirscanner.ListAllCCRFiles (szCuritem, &m_dlFilelist);
				}
				
			}

			if (m_dlFilelist.GetNumItems () > 0) {
				// if there were any ccr files in the protected folders then return true
				m_pdiag->OutputText ("ProtectedModeCheck: Items found for decryption.");
				m_pdiag->OutputInt ("ProtectedModeCheck: Number of found items: ", m_dlFilelist.GetNumItems ());
				return true;
			} else {
				m_pdiag->OutputText ("ProtectedModeCheck: zero items found!");
				return false;
			}

		} else {
			m_pdiag->OutputText ("ProtectedModeCheck: cannot read list file!");
			return false;
		}
	} else {
		m_pdiag->OutputText ("ProtectedModeCheck: list file not found!");
		return false;
	}
}

void MainWindow::DoSessionDecryption ()
{
	// Function to decrypt any files found in protected/session mode
	// that need to be decrypted for this session
	bool bproceed = false;

	if (m_bDecryptionsessioncheck == false) {

		m_dlFilelist.Clear ();
		if (CheckProtectedModeFiles (false) == false) {
			return;
		}

	} else {
		m_bDecryptionsessioncheck = false;
	}

	// This is where we need to check and ask the user if they really want to decrypt their files in
	// session mode.
	if (strcmp (m_registry.ReadStringSetting (PROTECTEDFOLDERSPROMPTPUP), "yes") == 0) {
		if (m_pfrequest.GetAnswered () == false) {
			bproceed = false;
			m_pfrequest.Initialise (m_hwnd, 0);
			m_pfrequest.SetStartupMessage (true);
			m_pfrequest.Show ();
		} else {
			bproceed = true;
		}
	} else {
		bproceed = true;
	}

	if (bproceed == true) {
		Show ();

		//MessageBox (NULL, "Session decryption would have started.", "Test", MB_OK);

		m_appmode = MODE_DECRYPT;
		
		m_bDeleteorig = true; // Delete the original files after being worked on
		m_filemode = MODE_FOLDER;
		m_iFilepointer = 0;

		SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
		SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

		m_bUsesessionpassword = false;
		m_passwindow.Initialise (m_hwnd, 0);

		if (m_appmode == MODE_ENCRYPT) {
			m_passwindow.SetEncryptMode (true);
		} else {
			m_passwindow.SetEncryptMode (false);
		}

		m_passwindow.SetSessionMode (true, false);
		m_passwindow.Show ();
	}
}

bool MainWindow::DoesShellCommandExist ()
{
	// Quick function to check if the shell command exists
	// in which case some preparation is needed.
	PrepareShellInfoPath ();
	if (FileExists (m_szShellInfoPath) == true) {
		return true;
	} else {
		return false;
	}
}

void MainWindow::CheckDirectCommand ()
{
	m_dlFilelist.Clear (); // Clear the global list of all the files we have to process
	m_dlFilelist.AddItem (m_szCommandPath, strlen (m_szCommandPath), false);

	m_appmode = MODE_DECRYPT;
		
	m_bDeleteorig = false; // Delete the original files after being worked on
	m_filemode = MODE_FOLDER;
	m_iFilepointer = 0;

	SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
	SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

	m_passwindow.Initialise (m_hwnd, 0);

	if (m_appmode == MODE_ENCRYPT) {
		m_passwindow.SetEncryptMode (true);
	} else {
		m_passwindow.SetEncryptMode (false);
	}

	m_passwindow.SetSessionMode (false, false);
	m_passwindow.Show ();
}

unsigned long MainWindow::GetFileSize (char *szFilepath)
{
	struct _finddata_t c_file;
	long hFile;

	if( (hFile = _findfirst(szFilepath, &c_file )) == -1L ) {
		return 0;
	}
	else
	{
		return c_file.size;
	}
}

void MainWindow::InitialisePackage ()
{
	// This function sets up the executable package Memory buffer
	// in preparation for appending encrypted files to the package
	// Basically, it adds the signature, and then works out and sets the size
	// of the total package based on the size of the individual files in the
	// dlFileList.
	unsigned long lpsig1 = 7777777;
	unsigned long lpsig2 = 4747477;
	unsigned long lpsig3 = 4447755;
	unsigned long lpsig4 = 7007000;

	unsigned long lTotalfilesize = 0;
	unsigned long lSinglefilesize = 0;
	unsigned long lPackagesize = 0;
	int iNumfiles = 0;

	char szfilepath[SIZE_STRING];

	if (m_filemode == MODE_FOLDER) {
		for (int s=0;s<m_dlFilelist.GetNumItems ();s++) {
			ZeroMemory (szfilepath, SIZE_STRING);
			m_dlFilelist.GetItem (szfilepath, s);
			
			lSinglefilesize = GetFileSize (szfilepath);
			lTotalfilesize+= lSinglefilesize;
		}

		m_pdiag->OutputInt ("Total Package File Size: ", lTotalfilesize);
		m_pdiag->OutputInt ("Number of Packaged Files: ", m_dlFilelist.GetNumItems ());

		// Now work out the size of the memory buffer storing all the encrypted files
		lPackagesize+=SIZE_STRING*m_dlFilelist.GetNumItems ();
		lPackagesize+=sizeof (int)*m_dlFilelist.GetNumItems ();
		lPackagesize+=sizeof (unsigned long)*m_dlFilelist.GetNumItems ();
		lPackagesize+=lTotalfilesize+4000; // the additional 4000 bytes account for encrypted header info
		lPackagesize+=sizeof (int);
		lPackagesize+=sizeof (unsigned long)*4;
		
		

		m_pdiag->OutputInt ("Package Size Needed: ", lPackagesize);

		m_memPackage.Clear ();
		m_memPackage.SetSize (lPackagesize);

		iNumfiles = m_dlFilelist.GetNumItems ();

		m_memPackage.Append (&lpsig1, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig2, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig3, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig4, sizeof (unsigned long)); // Append the signature part

		m_memPackage.Append (&iNumfiles, sizeof (int)); // Append the number of files to the package;

		// The rest of the package is populated everytime SaveEncryptedFile is called using Append

		// SIG1 | SIG2 | SIG3 | SIG4 | NUMFILES | NAMELEN | NAME | FILELEN | FILE
		// ulong| ulong| ulong| ulong| int      | int     | char | ulong   | byte
	}

	if (m_filemode == MODE_FILE) {
	
		lTotalfilesize+= GetFileSize (m_szInputfile);

		m_pdiag->OutputInt ("Total Package File Size: ", lTotalfilesize);
		m_pdiag->OutputInt ("Number of Packaged Files: ", 1);

		// Now work out the size of the memory buffer storing all the encrypted files
		lPackagesize+=SIZE_STRING*1;
		lPackagesize+=sizeof (int)*1;
		lPackagesize+=sizeof (unsigned long)*1;
		lPackagesize+=lTotalfilesize+4000; // the additional 4000 bytes account for encrypted header info
		lPackagesize+=sizeof (int);
		lPackagesize+=sizeof (unsigned long)*4;
		
		m_pdiag->OutputInt ("Package Size Needed: ", lPackagesize);

		m_memPackage.Clear ();
		m_memPackage.SetSize (lPackagesize);

		iNumfiles = 1;

		m_memPackage.Append (&lpsig1, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig2, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig3, sizeof (unsigned long)); // Append the signature part
		m_memPackage.Append (&lpsig4, sizeof (unsigned long)); // Append the signature part

		m_memPackage.Append (&iNumfiles, sizeof (int)); // Append the number of files to the package;

	}


}

void MainWindow::CheckShellCommand ()
{
	DWORD dwMode = 0;
	int iNumpaths = 0;
	int iCurlength = 0;
	char szCurpath[SIZE_STRING];
	int iPointer = 0;

	PrepareShellInfoPath ();
	m_dlFilelist.Clear (); // Clear the global list of all the files we have to process

	if (FileExists (m_szShellInfoPath) == true) {
		
		MemoryBuffer memShellInfo;
		if (memShellInfo.ReadFromFile (m_szShellInfoPath) == true) {
			
			memcpy (&dwMode, memShellInfo.GetBuffer (), sizeof (DWORD));
			iPointer+=sizeof (DWORD);
	
			memcpy (&iNumpaths, (BYTE *) memShellInfo.GetBuffer ()+iPointer, sizeof (int));
			iPointer+=sizeof (int);

			for (int c=0;c<iNumpaths;c++) {
				
				memcpy (&iCurlength, (BYTE *) memShellInfo.GetBuffer ()+iPointer, sizeof (int));
				iPointer+=sizeof (int);

				ZeroMemory (szCurpath, SIZE_STRING);
				memcpy (szCurpath, (char *) memShellInfo.GetBuffer ()+iPointer, iCurlength);
				iPointer+=iCurlength;

				//MessageBox (NULL, szCurpath, "Shell Info", MB_OK);
				if (IsDirectory (szCurpath) == true) {
					// If this is a directory then recursively scan all files, and add all their paths to the list
					m_Dirscanner.ListAllFiles (szCurpath, &m_dlFilelist);
				} else {
					// If this is a file then add the path directly to the list
					m_dlFilelist.AddItem (szCurpath, strlen (szCurpath), false);
				}
			}
			
			// By this point we have gathered a list of all files to be worked on.
			// Now set some application flags - and DO IT!
			if (dwMode == 100) {
				m_appmode = MODE_ENCRYPT;
			}

			if (dwMode == 200) {
				m_appmode = MODE_DECRYPT;
			}

			if (dwMode == 300) {
				m_appmode = MODE_ENCRYPT;
				m_bcreatepackage = true;
			}

			if (dwMode == 400) {
								

				memShellInfo.Clear (); // Clear the shell info data
				DeleteFile (m_szShellInfoPath); // Delete the shell info file

				SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
				SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

				SendMessage(m_hwndprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
				SendMessage (m_hwndprogress, PBM_SETSTEP, 1, 0L);

				char szInfile[SIZE_STRING];

				char szMessage[SIZE_STRING];				
				ZeroMemory (szMessage, SIZE_STRING);

				sprintf_s (szMessage, SIZE_STRING, "CAUTION: You are about to securely delete %i file(s). Once deleted, they will be unrecoverable using any third-party recovery software. Are you sure you wish to proceed?", m_dlFilelist.GetNumItems ());
				if (MessageBox (NULL, szMessage, "Secure File Deletion", MB_ICONQUESTION | MB_YESNO) == IDYES) {
				
					for (int f=0;f<m_dlFilelist.GetNumItems ();f++) {
						ZeroMemory (szInfile, SIZE_STRING);
						m_dlFilelist.GetItem (szInfile, f);

						m_fileshred.SecureDelete (szInfile);

						SendMessage (m_hwndtotalprogress, PBM_SETPOS, f, 0L);
						SendMessage (m_hwndprogress, PBM_SETPOS, f, 0L);
					}

					PostQuitMessage (0);

				} else {
					PostQuitMessage (0);
				}								
			}

			if (dwMode != 400) {

				memShellInfo.Clear (); // Clear the shell info data
				DeleteFile (m_szShellInfoPath); // Delete the shell info file

				m_bDeleteorig = true; // Delete the original files after being worked on
				m_filemode = MODE_FOLDER;
				m_iFilepointer = 0;

				if (m_bcreatepackage == true) {
					// If we are in package creation mode, we need to work out the total size of all of the files				
					InitialisePackage ();
				}

				SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
				SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

				m_passwindow.Initialise (m_hwnd, 0);

				if (m_appmode == MODE_ENCRYPT) {
					m_passwindow.SetEncryptMode (true);
				} else {
					m_passwindow.SetEncryptMode (false);
				}

				m_passwindow.SetSessionMode (false, false);
				m_passwindow.Show ();

			}

		} else {
			m_pdiag->OutputText ("There was a problem reading the Shell Info file.");
		}
	}
}

void MainWindow::GetFileExtension (bool &bFound, char *pszFilePath, char *pszOutExtension)
{
	char szFullPath[SIZE_STRING];
	ZeroMemory (szFullPath, SIZE_STRING);
	strcpy_s (szFullPath, SIZE_STRING, pszFilePath);

	char szTmp[SIZE_NAME];

	int a = 0;
	int exloc = 0;
	bool bexfound = false;

	for (a=strlen(szFullPath);a>0;a--) {
		ZeroMemory (szTmp, SIZE_NAME);
		strncpy_s (szTmp, SIZE_NAME, szFullPath+a, 1);
		
		if (strcmp (szTmp, ".") == 0) {
			exloc = a;
			bexfound = true;
		}
	}

	if (bexfound == true) {
		ZeroMemory (pszOutExtension, SIZE_NAME);
		strcpy_s (pszOutExtension, SIZE_NAME, szFullPath+exloc);
		bFound = true;		
	} else {
		bFound = false;
	}
}

unsigned int MainWindow::GetVirtualKeyCode (char *szChar)
{
	int iWantedcharvalue = 0;
	int vk = 0;
	unsigned int iConvertedvalue = 0;

	if (strlen (szChar) > 0) {
		iWantedcharvalue = szChar[0];

		for (vk=0;vk<255;vk++) {
			iConvertedvalue = MapVirtualKey (vk, 2);

			if (LOWORD (iConvertedvalue) == iWantedcharvalue) {
				return vk;
			}
		}
	}
	return 0;
}

void MainWindow::Test ()
{
	m_pdiag->OutputText ("Test function beginning...");

	// Generate a public and private key
	//RSAGenerateKey ();
	//WritetoKeyCanvas ();

	//GetKeyfromCanvas ();

	//GetDistributionfromCanvas ();

	/*
	char szTest[SIZE_STRING];
	ZeroMemory (szTest, SIZE_STRING);

	char szSep[SIZE_NAME];
	ZeroMemory (szSep, SIZE_NAME);
	szSep[0] = 34;

	strcat_s (szTest, SIZE_STRING, "Test String");
	strcat_s (szTest, SIZE_STRING, szSep);
	strcat_s (szTest, SIZE_STRING, "Middle");
	strcat_s (szTest, SIZE_STRING, szSep);

	MessageBox (NULL, szTest, "Test String", MB_OK);
	*/

	/*
	unsigned long long ullNum = 18446744073709551615;

	m_pdiag->OutputInt ("sizeof ullNum: ", sizeof (ullNum));

	MemoryBuffer memPublic;	
	MemoryBuffer memPrivateOne;
	MemoryBuffer memPrivateTwo;

	m_keycrypt.CreateKeySeed ("omocronfive247onetwothreefourfivesevenni", &memPublic);
	m_keycrypt.CreateKeySeed ("deltafouronetwothreefourfivesevennineten", &memPrivateOne);
	//m_keycrypt.CreateKeySeed ("sevenalphatwoonetwothreefourfivesevennin", &memPrivateTwo);

	CombineKeys (&memPublic, &memPrivateOne, &memPrivateTwo);

	char szNum[SIZE_STRING];
	ZeroMemory (szNum, SIZE_STRING);

	unsigned long lVal = m_keycrypt.Raise (999, 9);
	//ultoa (ullNum, szNum, 10);
	_ui64toa (ullNum, szNum, 10);
	
	

	m_pdiag->OutputText ("szNum: ", szNum);

	m_pdiag->OutputText ("Key seed created.");

	memPublic.SaveToFile ("C:\\Temp\\Keyseed1.dat");
	memPublic.SaveToFile ("C:\\Temp\\Keyseed2.dat");
	memPrivateTwo.SaveToFile ("C:\\Temp\\KeyseedResult.dat");
	*/
	
	//RSAPrivateFromPublic ();
	//EncryptUsingPublic ();
	//DecryptUsingPrivate ();
	//RSAGenandEncrypt ();
	
	//m_Dirscanner.ListAllFiles ("C:\\Windows");
	//CheckShellCommand ();
	//m_registry.WriteString (
	//CreateFileAssociation ();
	//RemoveFileAssociation ();

	//ShellExecute (m_hwnd, "open", "C:\\windows\\sdelete.exe", "-p 3 C:\\temp\\data\\Acer01.bmp", NULL, SW_HIDE);
	

	//STARTUPINFO startinfo;
	//startinfo.cb = sizeof (STARTUPINFO);
	//startinfo.lpReserved = NULL;
	//startinfo.lpDesktop = NULL;
	//startinfo.lpTitle = NULL;
	//startinfo.dwFlags = NULL;
	//startinfo.wShowWindow = SW_SHOW;
	//startinfo.cbReserved2 = 0;
	//startinfo.lpReserved2 = NULL;

	//PROCESS_INFORMATION processinfo;

	// CREATE_NO_WINDOW
	//CreateProcess (NULL, "C:\\windows\\sdelete.exe -p 3 C:\\temp\\Data\\BlueBall.psd", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startinfo, &processinfo);
	//CreateProcess (NULL, "C:\\windows\\notepad.exe", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startinfo, &processinfo);

	//BYTE b1 = 0;
	//BYTE b2 = 0;

	//b1 = 147;
	//b2 = ~b1;

	//ShowInt (b1);
	//ShowInt (b2);

	//m_Dirscanner.IsFileCCR ("C:\\Temp\\Test1.ccr");

	//m_fileshred.SecureDelete ("C:\\temp\\Data\\SHRED\\shred_src\\ProgressDlg.cpp");

	//DeleteFile ("C:\\Temp\\Data\\SHRED\\license.txt");
	//ShowWaitDialog ();

	//MessageBox (NULL, "Test something", "Test", MB_OK);
	//m_passwindow.Initialise (m_hwnd, 0);

	//if (m_passwindow.GetSessionMode () == true) {
	//	m_passwindow.SetSessionMode (false, false);
	//} else {
	//	m_passwindow.SetSessionMode (true, false);
	//}

	
	//m_passwindow.Show ();
	//RSAGenandEncrypt ();

	//int test = 0;
	//char *stopstring;

	//test = strtol ("AA", &stopstring, 16);

	//BYTE bCur = 6;

	//char szTest[SIZE_NAME];
	//ZeroMemory (szTest, SIZE_NAME);

	//itoa (bCur, szTest, 16);

	//sprintf_s (szTest, SIZE_NAME, "%.2X", 254);

	//m_pdiag->OutputText ("Test value: ", szTest);

	//sprintf_s (szCaption, SIZE_STRING, "%s (%s)    %.2f GB used / %d%%", szVolumename, m_szDrive, fAmount, iPercent);
	//sprintf_s (szCaption2, SIZE_STRING, "%.2f GB free", fAvail);

	//m_pdiag->OutputInt ("Text value is: ", test);

	/*
	InternetHandler ih;

	MemoryBuffer memFile;

	if (ih.DoesWebConnectionExist () == true) {
		
		if (ih.InternetFiletoMemoryBuffer ("http://www.linearcreations.co.uk/USpace.exe", &memFile, 100000) == false) {
			MessageBox (NULL, "Download failed.", "File Download", MB_OK);
		} else {
			MessageBox (NULL, "Download Success.", "File Download", MB_OK);

			m_pdiag->OutputInt ("memFile size: ", memFile.GetSize ());

		}

	} else {
		MessageBox (NULL, "No Internet Available.", "Updates", MB_OK);
	}
	*/

	//MainWindow *pwnd;

	//pwnd = (MainWindow *) this;

	//m_registry.WriteStringCU ("SOFTWARE\\CedeSoft\\CedeCrypt", "TestValue", "12345");

	//CreateFileAssociation ();

	//m_pdiag->OutputInt ("Virtual Key code from E: ", GetVirtualKeyCode ("E"));
	//m_pdiag->OutputInt ("Virtual Key code from d: ", GetVirtualKeyCode ("["));
	
	//m_pfrequest.Initialise (m_hwnd, 0);
	//m_pfrequest.SetStartupMessage (false);
	//m_pfrequest.Show ();

	//return;
	DoUpdateCheck (false); // Launch in a separate thread
}

void MainWindow::DoAutomaticUpdateCheck ()
{
	// This is called at startup, and checks if the user has selected automatic updates
	// and if so, does a quick check online to see if there is a new version available
	// of itself.
	bool bproceed = false;

	if (m_registry.DoesSettingExist (AUTOUPDATECHECK) == true) {
		if (strcmp (m_registry.ReadStringSetting (AUTOUPDATECHECK), "yes") == 0) {
			bproceed = true;
		} else {
			bproceed = false;
		}
	} else {
		bproceed = true; // Default auto update setting.
	}

	if (bproceed == true) {
		if (IsUpdateCheckDateDifferent () == true) {
			DoUpdateCheck (true); // We specify true for unattended, no unnecessary prompts when checking automatically.
		} else {
			m_pdiag->OutputText ("Auto update check - check already done today.");
		}
	} else {
		m_pdiag->OutputText ("Auto update check - switched off by user.");
	}
}

void MainWindow::StoreUpdateCurrentDate ()
{
	SYSTEMTIME systemtime;
	GetLocalTime (&systemtime);
	char szDate[255];

	int iDay = systemtime.wDay;
	int iMonth = systemtime.wMonth;
	int iYear = systemtime.wYear;
	
	ZeroMemory (szDate, 255);

	sprintf_s (szDate, 255, "%d.%d.%d", iDay, iMonth, iYear);

	m_registry.WriteStringSetting (LASTUPDATECHECK, szDate);
}

bool MainWindow::IsUpdateCheckDateDifferent ()
{
	SYSTEMTIME systemtime;
	char szLastCheck[SIZE_STRING];
	ZeroMemory (szLastCheck, SIZE_STRING);

	GetLocalTime (&systemtime);
	
	char szDate[255];

	int iDay = systemtime.wDay;
	int iMonth = systemtime.wMonth;
	int iYear = systemtime.wYear;
	
	ZeroMemory (szDate, 255);

	sprintf_s (szDate, 255, "%d.%d.%d", iDay, iMonth, iYear);

	strcpy_s (szLastCheck, SIZE_STRING, m_registry.ReadStringSetting (LASTUPDATECHECK));

	if (strcmp (szDate, szLastCheck) == 0) {
		return false;
	} else {
		return true;
	}
}

void MainWindow::CheckApplicationUpdate (bool bunattended)
{
	// Function to check for application updates over the internet from the CedeSoft
	// website.
	AppUpdater upd;
	upd.SetDiagnostics (m_pdiag);

	if (upd.IsUpdateCheckPossible () == true) {
		if (upd.CheckForUpdate () == false) {
			if (bunattended == false) {
				MessageBox (NULL, "There are no updates available.", "CedeCrypt", MB_ICONINFORMATION | MB_OK);
			}
		} else {
			StoreUpdateCurrentDate (); // Store the current date so that we don't check again today if automatic updates are switched on
			if (upd.IsNewVersionAvailable () == true) {
				if (upd.ShowNewVersionInfo () == true) {
					// User selected yes, so download the latest version
					if (upd.UpdateFromInternet () == true) {
						MessageBox (NULL, "CedeCrypt was successfully updated. The changes will take effect the next time you restart your computer.", "CedeCrypt Internet Update", MB_OK | MB_ICONINFORMATION);
					} else {
						MessageBox (NULL, "CedeCrypt update failed. Please try again later.", "CedeCrypt Internet Update", MB_OK | MB_ICONEXCLAMATION);					
					}
				}
			} else {
				if (bunattended == false) {
					MessageBox (NULL, "Your version of CedeCrypt is up to date.", "CedeCrypt Update.", MB_OK | MB_ICONINFORMATION);
				}
			}
		}
	} else {
		if (bunattended == false) {
			MessageBox (NULL, "CedeCrypt was unable to establish a connection to the Internet.", "CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

DWORD WINAPI MainWindow::UpdateThreadProc (PVOID pParam)
{
	// The internet update check is done in a separate thread, this is to avoid
	// any blocking being made visible to the user.
	MainWindow *mainwind;
	mainwind = (MainWindow *) pParam;

	mainwind->CheckApplicationUpdate (false);

	return 0;
}

DWORD WINAPI MainWindow::UpdateThreadProcUnat (PVOID pParam)
{
	// The internet update check is done in a separate thread, this is to avoid
	// any blocking being made visible to the user.
	MainWindow *mainwind;
	mainwind = (MainWindow *) pParam;

	mainwind->CheckApplicationUpdate (true);

	return 0;
}

void MainWindow::DoUpdateCheck (bool unattended)
{
	// Spawn the thread that will check for application updates.
	HANDLE hThread;
	DWORD dwThreadID;

	if (unattended == true) {
		hThread = CreateThread (NULL, 0, UpdateThreadProcUnat, (void *) this, 0, &dwThreadID);
	} else {
		hThread = CreateThread (NULL, 0, UpdateThreadProc, (void *) this, 0, &dwThreadID);
	}
	
}

void MainWindow::CreateFileAssociation ()
{
	char szModule[SIZE_STRING];
	ZeroMemory (szModule, SIZE_STRING);
	GetModuleFileName (NULL, szModule, SIZE_STRING);
	
	char szIconPath[SIZE_STRING];
	ZeroMemory (szIconPath, SIZE_STRING);
	strcpy_s (szIconPath, SIZE_STRING, szModule);
	strcat_s (szIconPath, SIZE_STRING, ",0");

	char szCommandPath[SIZE_STRING];
	ZeroMemory (szCommandPath, SIZE_STRING);

	char szSep[SIZE_NAME];
	ZeroMemory (szSep, SIZE_NAME);
	szSep[0] = 34;

	strcpy_s (szCommandPath, SIZE_STRING, szSep);
	strcat_s (szCommandPath, SIZE_STRING, szModule);
	strcat_s (szCommandPath, SIZE_STRING, szSep);
	strcat_s (szCommandPath, SIZE_STRING, " /d %1");

	m_registry.WriteStringCR (".ccr", "", "ccryptfile");
	m_registry.WriteStringCR ("ccryptfile", "", "Encrypted CedeCrypt File");
	m_registry.WriteStringCR ("ccryptfile\\DefaultIcon", "", szIconPath);
	m_registry.WriteStringCR ("ccryptfile\\shell\\open\\command", "", szCommandPath);

	SHChangeNotify (SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

void MainWindow::CreateShellExtensionPath ()
{
	//Software\\CedeSoft\\CedeCrypt

	char szModulepath[SIZE_STRING];
	ZeroMemory (szModulepath, SIZE_STRING);
	GetModuleFileName (NULL, szModulepath, SIZE_STRING);

	m_registry.WriteStringCU ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath", szModulepath);
}

void MainWindow::RemoveFileAssociation ()
{
	RegDeleteKey (HKEY_CLASSES_ROOT, ".ccr");
	RegDeleteKey (HKEY_CLASSES_ROOT, "ccryptfile\\shell\\open\\command");
	RegDeleteKey (HKEY_CLASSES_ROOT, "ccryptfile\\shell\\open");
	RegDeleteKey (HKEY_CLASSES_ROOT, "ccryptfile\\shell");
	RegDeleteKey (HKEY_CLASSES_ROOT, "ccryptfile\\DefaultIcon");
	RegDeleteKey (HKEY_CLASSES_ROOT, "ccryptfile");

	SHChangeNotify (SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

void MainWindow::OnNotify (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
}

void MainWindow::GetClipboard () {
	//MessageBox (NULL, "Test", "Info", MB_OK);
	HANDLE hClip;
	UINT iFormat = 0;
	long lTotalSize = 0;
	long lBufPointer = 0;
	LPTSTR    lptstr;
	DWORD dwErr;
	//InitDataBuffer (); // Free the buffer that the clipboard will use
	
	char *szCliptext; // Temporary storage for the clipboard text
	
	CloseClipboard ();

	if (OpenClipboard (NULL) != 0) {
	
		//OpenClipboard (m_hwnd);

		iFormat  = EnumClipboardFormats (CF_TEXT);
		
		if (iFormat != 0) {
			hClip = GetClipboardData (CF_TEXT);
			if (hClip == NULL) {
				dwErr = GetLastError ();
				MessageBox (NULL, "GetClipboard - GetClipboardData failed.", "Error", MB_OK);
				m_pdiag->OutputInt ("Last Error: ", dwErr);
			}
			
			lptstr = (LPTSTR) GlobalLock(hClip);
			GlobalUnlock (hClip);
			
			szCliptext = (char *) calloc (strlen (lptstr)+1, 1);
			strcpy (szCliptext, lptstr);
			lTotalSize = strlen (lptstr)+1;
			
			//MessageBox (NULL, szCliptext, "Clipboard Text", MB_OK);
			m_pdiag->OutputText (szCliptext);
			
			m_memTextbuffer.Clear ();
			m_memTextbuffer.SetSize (lTotalSize);
			m_memTextbuffer.Write ((char *) szCliptext, 0, strlen (lptstr));
			//m_memTextbuffer.SaveToFile ("C:\\Temp\\Textbuffer.txt");

			//memcpy (g_bDataBuffer, &lTotalSize, sizeof (long));
			//lBufPointer+=sizeof (long);
			//memcpy (g_bDataBuffer+lBufPointer, szCliptext, strlen (lptstr)+1);
			//lBufPointer+=strlen (lptstr)+1;
			free (szCliptext);
			//GlobalFree (hClip);
			//g_lDataBufferSize = lBufPointer;
		}
		
		if (CloseClipboard () == 0) {
			dwErr = GetLastError ();
			MessageBox (NULL, "GetClipboard - CloseClipboard Failed.", "Error", MB_OK);
			m_pdiag->OutputInt ("Last Error: ", dwErr);
		}
	
	} else {
		MessageBox (NULL, "Open Clipboard Failed.", "Error", MB_OK);
		m_pdiag->OutputInt ("OpenClipboard GetLastError: ", GetLastError ());
	}
}

void MainWindow::SetShiftState(int sstate) {
	INPUT input[1];
	ZeroMemory(input, sizeof(input));    
    
	if (sstate == 1) {
		input[0].type =INPUT_KEYBOARD;
		input[0].ki.wVk = VK_LCONTROL;
		SendInput(1, input, sizeof(INPUT));
		return;
	}
	
	if (sstate == 0) {
		input[0].type =INPUT_KEYBOARD;
		input[0].ki.wVk = VK_LCONTROL;
		input[0].ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, input, sizeof(INPUT));
		return;
	}
}

void MainWindow::SendKeyState(int vkeycode) {
	INPUT input[2];
	ZeroMemory(input, sizeof(input));        
	input[0].type =INPUT_KEYBOARD;
	input[0].ki.wVk = vkeycode;
	input[1].type =INPUT_KEYBOARD;
	input[1].ki.wVk = vkeycode;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(2, input, sizeof(INPUT));
}

void MainWindow::OnHotKey (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1234) { // The encrypt Hot key was generated
		m_pdiag->OutputInt ("Hot Key ID: ", wParam);

		m_lastactivewindow = GetForegroundWindow (); // Get the handle to the window the user is working with

		SetShiftState (1);
		//SendKeyState (67);
		SendKeyState (88);
		SetShiftState (0);
		
		Sleep (1000);
		GetClipboard ();

		/*
		char szTest[SIZE_NAME];
		ZeroMemory (szTest, SIZE_NAME);

		strcpy_s (szTest, SIZE_NAME, "Hello this is destination clipboard.");

		MemoryBuffer memTest;

		memTest.SetSize (strlen(szTest));
		memTest.Write ((char *) szTest, 0, strlen (szTest));

		memTest.SaveToClipboard (m_hwnd);
		*/

		
		// Now set some application flags - and DO IT!		
		m_appmode = MODE_ENCRYPT;

		m_bDeleteorig = true; // Delete the original files after being worked on
		m_filemode = MODE_TEXT;
		m_iFilepointer = 0;

		SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
		SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);
		
		m_passwindow.Initialise (m_hwnd, 0);

		if (m_appmode == MODE_ENCRYPT) {
			m_passwindow.SetEncryptMode (true);
		} else {
			m_passwindow.SetEncryptMode (false);
		}

		m_passwindow.SetSessionMode (false, false);
		m_passwindow.Show ();

		// Make the password window the foreground window
		SetForegroundWindow (m_passwindow.GetBaseHWND ());
		
	}


	if (wParam == 1235) { // The decryption hot key was generated
		
		
		//MessageBox (NULL, "Decryption hot key pressed", "Test", MB_OK);

		m_pdiag->OutputInt ("Hot Key ID: ", wParam);

		SetShiftState (1);
		SendKeyState (67);
		SetShiftState (0);

		Sleep (1000);
		GetClipboard ();

		if (CheckEnclosingTags (&m_memTextbuffer) == true) {
			
			m_pdiag->OutputText ("Start and End tags verified ok.");

			if (CheckStartTag (&m_memTextbuffer) == true) {
				m_pdiag->OutputText ("Start CCRYPT tag verified.");

				int sloc = 0;
				int eloc = 0;

				sloc = GetStartTagLocation (&m_memTextbuffer);
				eloc = GetEndTagLocation (&m_memTextbuffer);

				if (sloc != -1 && eloc != -1) {
					
					if (eloc > sloc) { // Just a quick check to make sure that the end tag location is after the start tag location
						m_memTextparseable.Clear ();
						m_memTextparseable.SetSize (eloc-sloc);
						m_memTextparseable.Write ((char *) m_memTextbuffer.GetBuffer ()+sloc, 0, eloc-sloc);

						m_memTextparseable.SaveToFile ("C:\\Temp\\TextParseable.dat");

						MakeBinary (&m_memTextparseable, &m_memTextprocessed);

						m_memTextprocessed.SaveToFile ("C:\\Temp\\DecryptProcessed.dat");

						// Now start the decryption process
						m_appmode = MODE_DECRYPT;

						m_bDeleteorig = true; // Delete the original files after being worked on
						m_filemode = MODE_TEXT;
						m_iFilepointer = 0;

						SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
						SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);

						m_passwindow.Initialise (m_hwnd, 0);

						if (m_appmode == MODE_ENCRYPT) {
							m_passwindow.SetEncryptMode (true);
						} else {
							m_passwindow.SetEncryptMode (false);
						}

						m_passwindow.SetSessionMode (false, false);
						m_passwindow.Show ();

						// Make the password window the foreground window
						SetForegroundWindow (m_passwindow.GetBaseHWND ());

						m_pdiag->OutputText ("Text parseable created ok.");
					} else {
						m_pdiag->OutputText ("eloc is not greater than sloc!!!");

						MessageBox (NULL, "The text you have selected doesn't look like it was encrypted with CedeCrypt. Please make sure you highlight all of the text including the preceding < and ending >", "Unrecognised Text", MB_OK | MB_ICONINFORMATION);					
					}

				} else {
					m_pdiag->OutputText ("eloc or sloc was not found!!");
					MessageBox (NULL, "The text you have selected doesn't look like it was encrypted with CedeCrypt. Please make sure you highlight all of the text including the preceding < and ending >", "Unrecognised Text", MB_OK | MB_ICONINFORMATION);					
				}

			} else {
				m_pdiag->OutputText ("Unable to find start tag");
				MessageBox (NULL, "The text you have selected was not encrypted using CedeCrypt. Please make sure you have highlighted all of the encrypted text.", "Unrecognised Text", MB_OK | MB_ICONINFORMATION);
			}

		} else {
			m_pdiag->OutputText ("Start and ending < or > was not found!!");
			MessageBox (NULL, "The text you have selected doesn't look like it was encrypted with CedeCrypt. Please make sure you highlight all of the text including the preceding < and ending >", "Unrecognised Text", MB_OK | MB_ICONINFORMATION);
		}
	}

	
}

void MainWindow::OnCommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
		case ID_CHKDELETEORIG:
		{
			if (m_bDeleteorig == false) {
				m_bDeleteorig = true;
				SendMessage (m_chkkeepopen, BM_SETCHECK, 1, 0);

			} else {
				m_bDeleteorig = false;
				SendMessage (m_chkkeepopen, BM_SETCHECK, 0, 0);
			}
		}
		break;
		case ID_OPTFILE:
		{
			SendMessage (m_hwndoptfile, BM_SETCHECK, 1, 0);
			SendMessage (m_hwndoptfolder, BM_SETCHECK, 0, 0);
			m_filemode = MODE_FILE;

			SetDlgItemText (m_hwnd, ID_GROUPBOX2, "Source and Destination Files");

			ShowWindow (m_hwndeditdest, SW_SHOW);
			ShowWindow (m_hwndbtndstbrowse, SW_SHOW);
			m_busingownpackagepath = false;
		}
		break;
		case ID_OPTFOLDER:
		{

			//MessageBox (m_hwnd, "Demonstration version limitation. Folder Encryption is only available in the full product.", "CedeCrypt Demo Limitation", MB_OK | MB_ICONEXCLAMATION);

			SendMessage (m_hwndoptfile, BM_SETCHECK, 0, 0);
			SendMessage (m_hwndoptfolder, BM_SETCHECK, 1, 0);
			m_filemode = MODE_FOLDER;			
			
			SetDlgItemText (m_hwnd, ID_GROUPBOX2, "Source Directory");

			ShowWindow (m_hwndeditdest, SW_HIDE);
			ShowWindow (m_hwndbtndstbrowse, SW_HIDE);
			m_busingownpackagepath = false;
		}
		break;
		case ID_OPTENCRYPT:
		{
			SendMessage (m_hwndoptencrypt, BM_SETCHECK, 1, 0);
			SendMessage (m_hwndoptdecrypt, BM_SETCHECK, 0, 0);
			SendMessage (m_hwndoptpackage, BM_SETCHECK, 0, 0);
			m_appmode = MODE_ENCRYPT;
			m_bcreatepackage = false;
			SetDlgItemText (m_hwnd, ID_EDITSOURCE, "<No Source Selected>");
			SetDlgItemText (m_hwnd, ID_EDITDEST, "<No Destination Selected>");
			m_busingownpackagepath = false;
		}
		break;
		case ID_OPTDECRYPT:
		{
			SendMessage (m_hwndoptencrypt, BM_SETCHECK, 0, 0);
			SendMessage (m_hwndoptdecrypt, BM_SETCHECK, 1, 0);
			SendMessage (m_hwndoptpackage, BM_SETCHECK, 0, 0);
			m_appmode = MODE_DECRYPT;
			m_bcreatepackage = false;
			SetDlgItemText (m_hwnd, ID_EDITSOURCE, "<No Source Selected>");
			SetDlgItemText (m_hwnd, ID_EDITDEST, "<No Destination Selected>");
			m_busingownpackagepath = false;
		}
		break;
		case ID_OPTENCRYPTPACKAGE:
		{
			SendMessage (m_hwndoptencrypt, BM_SETCHECK, 0, 0);
			SendMessage (m_hwndoptdecrypt, BM_SETCHECK, 0, 0);
			SendMessage (m_hwndoptpackage, BM_SETCHECK, 1, 0);
			m_appmode = MODE_ENCRYPT;
			m_bcreatepackage = true;
			SetDlgItemText (m_hwnd, ID_EDITSOURCE, "<No Source Selected>");
			SetDlgItemText (m_hwnd, ID_EDITDEST, "<No Destination Selected>");
			m_busingownpackagepath = false;
		}
		break;
		case ID_BTNSRCBROWSE:
		{
			BrowseSource ();

			m_busingownpackagepath = false;

			if (m_filemode == MODE_FOLDER) {
				//GetFileList (m_szInputfolder);
				m_dlFilelist.Clear ();
				m_Dirscanner.ListAllFiles (m_szInputfolder, &m_dlFilelist);
			}
		}
		break;
		case ID_BTNDSTBROWSE:
		{
			BrowseDestination ();
		}
		break;
		case ID_BTNBEGIN:
		{
			m_iFilepointer = 0;

			SendMessage(m_hwndtotalprogress, PBM_SETRANGE32, 0, m_dlFilelist.GetNumItems ()-1);
			SendMessage (m_hwndtotalprogress, PBM_SETSTEP, 1, 0L);


			ValidateInput ();
			/*
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (0));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (9));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (15));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (55));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (755));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (1755));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (21755));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (721755));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (8721755));
			m_pdiag->OutputInt ("Num: ", m_crypt.GetNumDigits (28721755));
			*/
		}
		break;
		case ID_BTNCANCEL:
		{
			m_bAwaitingcancel = true;
		}
		break;
		case IDM_FILE_EXIT:
		{
			PostQuitMessage (0);
		}
		break;
		case IDM_HELP_ABOUT:
		{
			m_aboutwindow.SetDistributionName (m_szDistributionname);
			m_aboutwindow.Initialise (m_hwnd, 0);
			m_aboutwindow.Show ();
		}
		break;
		case IDM_TOOLS_OPTIONS:
		{
			m_optionswindow.SetDiagnostics (m_pdiag);
			m_optionswindow.Initialise (m_hwnd, 0);
			m_optionswindow.Show ();
		}
		break;
		case IDM_FILE_TEST:
		{
			//m_pdiag->OutputText ("Test has been clicked");
			//Test ();
			//StandardEncryption enc;
			
			//MemoryBuffer memFile;

			m_passwindow.Initialise (m_hwnd, 0);

			//if (m_appmode == MODE_ENCRYPT) {
				m_passwindow.SetEncryptMode (true);
			//} else {
			//	m_passwindow.SetEncryptMode (false);
			//}

			m_passwindow.SetSessionMode (true, true);
			m_passwindow.Show ();
			
		}
		break;
		case IDM_FILE_RECOVER:
		{
			m_pdiag->OutputText ("Recovery initiated.");
			
			if (m_bCanvasRecoverymode == true) {
				RecoverCanvasPassword ();
			} else {
				RecoverPassword ();
			}
			
		}
		break;
		case IDM_ENTERLICENSE:
		{
			ShowLicensePrompt ();
		}
		break;
		case IDM_TOOLS_UTE:
		{
			//MessageBox (NULL, "Universal Text Encryption", "Test", MB_OK);
			m_uteoptions.Initialise (m_hwnd, 0);
			m_uteoptions.Show ();
		}
		break;
		case IDM_FILE_PDIR:
		{
			m_pfolders.Initialise (m_hwnd, 0);
			m_pfolders.LoadList ();
			m_pfolders.Show ();
		}
		break;
		case IDM_CHECKUPDATES:
		{
			DoUpdateCheck (false); // Check for updates but with this time we want the additional prompts.
		}
		break;
		case ID_BTNEXIT:
		{
			PostQuitMessage (0);
			//m_sbcancelwaitdlg = true;
		}
		//break;
	}	
}

void MainWindow::OnUICommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int controlID = wParam;

	switch (controlID) {
		case CID_PASSWORDOK:
		{

			m_pdiag->OutputText ("Password was entered ok.");
			
			if (m_filemode == MODE_FOLDER) {

				if (m_appmode == MODE_DECRYPT) {
					ZeroMemory (m_szInputfile, SIZE_STRING);


					m_dlFilelist.GetItem (m_szInputfile, m_iFilepointer);
					
					
					m_pdiag->OutputText ("Infile: ", m_szInputfile);

					ZeroMemory (m_szOutputfile, SIZE_STRING);

					if (strncmp (m_szInputfile+strlen(m_szInputfile)-4, ".ccr", 4) == 0) {
						strncpy_s (m_szOutputfile, SIZE_STRING, m_szInputfile, strlen(m_szInputfile)-4);						
					} else {
						strcpy_s (m_szOutputfile, SIZE_STRING, m_szInputfile);
						strcat_s (m_szOutputfile, SIZE_STRING, ".dec");
					}
				}

				if (m_appmode == MODE_ENCRYPT) {
					ZeroMemory (m_szInputfile, SIZE_STRING);
					ZeroMemory (m_szOutputfile, SIZE_STRING);

					m_dlFilelist.GetItem (m_szInputfile, m_iFilepointer);
					m_dlFilelist.GetItem (m_szOutputfile, m_iFilepointer);
					strcat_s (m_szOutputfile, SIZE_STRING, ".ccr");
				}
				
			}
			
			if (m_filemode == MODE_TEXT) {
				DoTextEncryption ();	
			} else {
				m_starttime = GetTickCount ();
				DoEncryption ();			
			}			
		}
		break;
		case CID_PASSWORDCANCEL:
		{
			m_pdiag->OutputText ("Password was cancelled.");
			if (m_bSessionmode == true) {
				if (m_appmode == MODE_ENCRYPT) {
					MessageBox (m_hwnd, "You have not provided a password. Your protected folders will not be encrypted.", "CedeCrypt", MB_OK | MB_ICONINFORMATION);
					m_sbcancelwaitdlg = true;
					Hide ();
				}

				if (m_appmode == MODE_DECRYPT) {
					MessageBox (m_hwnd, "You have not provided your session password. Your protected folders will remain encrypted.", "CedeCrypt", MB_OK | MB_ICONINFORMATION);			
					Hide ();
				}
				
			} else {
				MessageBox (m_hwnd, "You must provide a password for Encryption / Decryption.", "CedeCrypt", MB_OK | MB_ICONINFORMATION);
			}
			
			if (m_bShellmode == true || m_bCommandmode == true) {
				PostQuitMessage (0);
			}
		}
		break;
		case CID_CHECKSHELL:
		{
			CheckShellCommand ();
		}
		break;
		case CID_CHECKCOMMAND:
		{
			CheckDirectCommand ();
		}
		break;
		case CID_SESSIONDECRYPT:
		{
			// Called on startup, and when protected mode files are present
			// and need to be decrypted for this session, so now we need to request
			// the session decryption password and decrypt the protected mode folders.
			m_pfrequest.SetAnswered (false);
			m_pfrequest.SetStartupMode (true);
			DoSessionDecryption ();
		}
		break;
		case CID_LICENSEWINDOWOK:
		{
			bool bOkinit = true;
			ZeroMemory (m_szUserlicense, SIZE_STRING);
			strcpy_s (m_szUserlicense, SIZE_STRING, m_licensewindow.GetEnteredLicense ());

			if (m_license.ValidateUserKey (m_szUserlicense) == true) {
				//MessageBox (NULL, "License is ok", "License", MB_OK);

				if (strcmp (m_license.GetUniqueProductID (), "11977557777777") == 0) {					

					if (m_license.GetTrialMode () == true) {
						
						m_pdiag->OutputText ("License Type: Trial/Evaluation");
						m_pdiag->OutputInt ("Evaluation Length (days): ", m_license.GetTrialDays ());
						
						m_pdiag->OutputText ("This CedeCrypt software is a Trial Version Only,");
						m_pdiag->OutputText ("To purchase the full product with ");
						m_pdiag->OutputText ("no limitations and enhanced features please visit ");
						m_pdiag->OutputText ("http://www.cedesoft.com ");

						m_license.CheckInstEx ();

						if (m_license.GetFirstInst () == false) {
							if (m_license.TrialValid () != 0) {
								bOkinit = false;
							}
						}

						m_pdiag->OutputInt ("Number of Days left: ",  m_license.GetTrialDays () - m_license.GetDaysInTrial ());

						if (bOkinit == true) {
							m_license.ClearAllKeys ();
							m_license.SaveMachineKey ();
							m_license.SaveUserKey (m_szUserlicense);

							m_licensewindow.SetAlwaysOnTop (false);
							MessageBox (NULL, "Trial License Accepted.", "License", MB_OK | MB_ICONINFORMATION);
							m_licensewindow.SetAlwaysOnTop (true);
							
							EnableWindow (m_hwnd, true);
							m_licensewindow.Hide ();
							sprintf_s (m_szWindowcomment, SIZE_STRING, "(Trial Version - %d days remaining)", (m_license.GetTrialDays () - m_license.GetDaysInTrial ()));
							UpdateWindowCaption ();
						}

					} else {
						m_pdiag->OutputText ("License Type: Full/Unlimited");
						m_license.ClearAllKeys ();
						m_license.SaveMachineKey ();
						m_license.SaveUserKey (m_szUserlicense);

						m_licensewindow.SetAlwaysOnTop (false);
						MessageBox (NULL, "Full License Key Accepted.", "License", MB_OK | MB_ICONINFORMATION);
						m_licensewindow.SetAlwaysOnTop (true);
						
						EnableWindow (m_hwnd, true);
						m_licensewindow.Hide ();
						sprintf_s (m_szWindowcomment, SIZE_STRING, "(Registered to %s)", m_license.GetOwner ());
						UpdateWindowCaption ();
					}


				} else {
					m_licensewindow.SetAlwaysOnTop (false);
					MessageBox (NULL, "The license key you have entered is not for this CedeSoft product. Please enter a valid license key.", "License Invalid.", MB_OK | MB_ICONEXCLAMATION);
					m_licensewindow.SetAlwaysOnTop (true);
				}
								
			} else {
				m_licensewindow.SetAlwaysOnTop (false);
				MessageBox (NULL, "Invalid License Key.", "License", MB_OK);
				m_licensewindow.SetAlwaysOnTop (true);
			}
		}
		break;
		case CID_LICENSEWINDOWCANCEL:
		{
			if (m_bvalidlicenseinitialised == false) {
				PostQuitMessage (0);
			}
		}
		break;
		case CID_PFOLDERREQUESTOK:
		{
			if (lParam == 1) {
				//MessageBox (NULL, "User has selected action.", "Selection", MB_OK);
				m_pfrequest.SetAnswered (true);

				if (m_pfrequest.GetStartMode () == true) {
					DoSessionDecryption ();
				}

				if (m_pfrequest.GetStartMode () == false) {
					if (DoSessionEncryption () == true) {
						//m_sbcancelwaitdlg = false;
						//ShowWaitDialog ();
					}
				}
			}
			if (lParam == 0) {
				//MessageBox (NULL, "User has selected nothing.", "Selection", MB_OK);			
				if (m_pfrequest.GetStartMode () == false) {
					m_sbcancelwaitdlg = true;
				} else {
					Hide ();
				}
			}
		}
		break;
	}
}

void MainWindow::OnTimer (WPARAM wParam)
{
	m_uihandler.NotifyTimer (wParam);

	switch (wParam) {
		case IDT_UPDATECAPTION:
		{
			KillTimer (m_hwnd, IDT_UPDATECAPTION);
			SetCaption (m_szWindowcaption, true);
			DoAutomaticUpdateCheck (); // Check online for updates if the user has chosen it.
		}
		break;
		case IDT_ACTIONCHECK:
		{
			//DeleteFile ("D:\\enc.txt");
			//DeleteFile ("D:\\dec.txt");
			CheckSessionAction ();
		}
		break;
	}
}

INT_PTR CALLBACK MainWindow::OptionsDlgProc (HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_INITDIALOG:
		{
			SetTimer (hWnd, IDT_DLGWAITCHECK, 1000, NULL);
		}
		break;
		case WM_TIMER:
		{
			switch (wParam)
			{
				case IDT_DLGWAITCHECK:
				{
					if (m_sbcancelwaitdlg == true) {
						EndDialog (hWnd, IDCANCEL);
					}
				}
				break;
			}
			break;
		}
		break;
		case WM_COMMAND:
		{
			switch (LOWORD (wParam))
			{
				case IDOK:
				{
				}
				break;
				//case IDCANCEL:
				//{
				//	EndDialog (hWnd, IDCANCEL);
				//}
				//break;
			}
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}


BOOL MainWindow::ShowWaitDialog ()
{
	int iRet = 0;

	iRet = DialogBox (GetModuleHandle (NULL), MAKEINTRESOURCE (IDD_DIALOG1), m_hwnd, OptionsDlgProc);
	return TRUE;
}

LRESULT MainWindow::OnPowerBroadcast(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (m_bSessionmode == true) {
		switch (wParam)
		{
			case PBT_APMQUERYSUSPEND:
			{
				//MessageBox (NULL, "Computer wants to suspend!", "Test", MB_OK);
				m_pfrequest.SetAnswered (false);
				m_pfrequest.SetStartupMode (false);

				if (DoSessionEncryption () == true) {
					m_sbcancelwaitdlg = false;
					ShowWaitDialog ();
				}								
			}
			break;
			case PBT_APMRESUMESUSPEND:
			{
				//MessageBox (NULL, "Computer is resuming from suspend!", "test", MB_OK);
				m_pfrequest.SetAnswered (false);
				m_pfrequest.SetStartupMode (true);				
				DoSessionDecryption ();
			}
			break;
		}
	}	
	return TRUE;
}

LRESULT MainWindow::OnEndSession (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT MainWindow::OnQueryEndSession (HWND hWnd, LPARAM lParam)
{
	//SetProcessShutdownParameters (511, SHUTDOWN_NORETRY);
	//MessageBox (NULL, "I will not shut down!", "Test", MB_ICONINFORMATION);
	//m_bSessionshuttingdown = true;
	
	if (m_bSessionmode == true) {
		m_pfrequest.SetAnswered (false);
		m_pfrequest.SetStartupMode (false);

		if (DoSessionEncryption () == true) {
			m_sbcancelwaitdlg = false;
			ShowWaitDialog ();
		}
	}

	//if (m_bShutdownencryptdone == false) {
		
	//	return FALSE;
	//} else {
		return TRUE;
	//}	
}

void MainWindow::OnPaint (HWND hWnd)
{
	m_uihandler.PaintControls (hWnd);	
}

void MainWindow::OnMouseMove (HWND hWnd, int mouseXPos, int mouseYPos)
{		
	m_uihandler.NotifyMouseMove (mouseXPos, mouseYPos);
}

void MainWindow::OnLButtonDown (HWND hWnd)
{
	m_uihandler.NotifyMouseDown ();
}

void MainWindow::OnLButtonDblClick (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_pdiag->OutputText ("Window Double Clicked.");
}

void MainWindow::OnLButtonUp (HWND hWnd)
{
	m_uihandler.NotifyMouseUp ();
}
