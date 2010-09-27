// This work is dedicated to the Lord our God. King of Heaven, Lord of Heaven's Armies.

#include "MainWindow.h"

MainWindow::MainWindow ()
{
		
}

MainWindow::~MainWindow ()
{
}

void MainWindow::Initialise (HWND hWnd)
{	
	SetParentHWND (hWnd);
	//m_hwnd = hWnd;
	SetBgColor (GetSysColor (COLOR_BTNFACE));
	SetCaption (TEXT ("CedeCrypt Distribution Wizard"));
	SetWindowStyle (FS_STYLESTANDARD);
	CreateAppWindow ("CRYPTWindowClass", 70, 0, 500, 300, false);
	m_uihandler.SetWindowProperties (0, 0, 0, 0, RGB (237, 237, 237));
	SetWindowPosition (FS_CENTER);
	Show ();
}

void MainWindow::SetDiagnostics (Diagnostics *pdiag)
{
	m_pdiag = pdiag;
}

void MainWindow::OnCreate (HWND hWnd)
{
	m_hwnd = hWnd;

	HFONT hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

	m_header.SetBitmapResources (IDB_BANNER);
	m_header.SetBitmapProperties (0, 0, 170, 256);
	m_header.SetProperties (hWnd, CID_HEADER, 0, 0, 170, 256);
	m_uihandler.AddDirectControl (&m_header);
	
	
	m_title.SetProperties (m_hwnd, 9875, 180, 10, 300, 35);
	m_title.SetTextProperties ("Arial", 120, 0, 0, 0, "CedeCrypt Distribution Wizard", RGB (0, 0, 0));
	m_uihandler.AddDirectControl (&m_title);


	m_lblInformation = CreateWindow ("static", "This wizard allows you to create multiple distributions of the CedeCrypt Setup Application. For each setup application created, the wizard will generate a separate Recovery key. It is extremely important that this Recovery key is kept in a secure and safe location && absolutely not disclosed. This recovery key allows you to recover CedeCrypt encoded files which have been created by the distribution.", WS_CHILD | WS_VISIBLE, 180, 50, 250, 150, hWnd, (HMENU) ID_LBLINFO, GetModuleHandle (NULL), NULL);
	SendMessage (m_lblInformation, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	m_lblname = CreateWindow ("static", "Distribution name:", WS_CHILD | WS_VISIBLE, 180, 170, 250, 150, hWnd, (HMENU) ID_LBLINFO, GetModuleHandle (NULL), NULL);
	SendMessage (m_lblname, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	m_txtname = CreateWindow ("edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 180, 190, 200, 19, hWnd, (HMENU) ID_EDITNAME, GetModuleHandle (NULL), NULL);
	SendMessage (m_txtname, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	m_btnbegin = CreateWindow ("button", "Create Setup", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 385, 188, 70, 23, hWnd, (HMENU) IDB_BTNBEGIN, GetModuleHandle (NULL), NULL);
	SendMessage (m_btnbegin, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));	



	m_mainmenu.CreateMainMenu (hWnd);

	//m_connectpanel.Hide ();
	
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

void MainWindow::RSAGenerateKey ()
{
	// This function generates an RSA public and private key
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
			//m_memGenPublic.SaveToFile ("C:\\Temp\\RSACrypt\\ExportedPublicKeyCustom.dat");
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
			//m_memGenPrivate.SaveToFile ("C:\\Temp\\RSACrypt\\GenPrivate.dat");
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


bool MainWindow::WritePublicKeytoResource ()
{
	// This function will take the public key that has been generated using RSAGenerateKey (). The public key read from here
	// will be one that will have been generated and it will be written to the key canvas in the Setup file
	// held as a resource. It also takes the member variable for the distribution name and also writes that to
	// the naming canvas in the binary after searching the start and end tags.


	// Long GUIDS are used as tags to mark the start and the end of each canvas, the public
	// key one and the naming one. Once the tags are located, the data is then written to the binary
	// and then the buffer is copied to the member variable memModBinary.

	MemoryBuffer memKeyCanvas;
	
	m_memModBinary.Clear (); // Clear the member binary buffer start

	char szKeytagstart[SIZE_STRING];
	char szKeytagend[SIZE_STRING];

	char szNamekeytagstart[SIZE_STRING];
	char szNamekeytagend[SIZE_STRING];

	char szDatatagstart[SIZE_STRING];
	char szDatatagend[SIZE_STRING];
	
	ZeroMemory (szKeytagstart, SIZE_STRING);
	ZeroMemory (szKeytagend, SIZE_STRING);

	ZeroMemory (szNamekeytagstart, SIZE_STRING);
	ZeroMemory (szNamekeytagend, SIZE_STRING);

	ZeroMemory (szDatatagstart, SIZE_STRING);
	ZeroMemory (szDatatagend, SIZE_STRING);
	
	strcpy_s (szKeytagstart, SIZE_STRING, "D169C884-9C83-11DD-8E99-DBE655D89593040257B6-9C84-11DD-81CB-80E855D89593");
	strcpy_s (szKeytagend, SIZE_STRING, "1106EEE0-9C84-11DD-B673-E8E855D89593163B798A-9C84-11DD-B19F-1EE955D89593");
	
	strcpy_s (szNamekeytagstart, SIZE_STRING, "2E279BFA-9C84-11DD-9673-12EA55D8959334ECBBA0-9C84-11DD-9D92-66EA55D89593");
	strcpy_s (szNamekeytagend, SIZE_STRING, "3CCCBDAC-9C84-11DD-A3EC-8EEB55D895934957B0E0-9C84-11DD-A4A3-44EC55D89593");

	strcpy_s (szDatatagstart, SIZE_STRING, "57702DBA-9C84-11DD-906C-C8EC55D89593");
	strcpy_s (szDatatagend, SIZE_STRING, "75579822-9C84-11DD-9422-ABED55D89593");

	int iPointer = 0;

	int iStartloc = 0;
	int iEndloc = 0;

	int iNamestartloc = 0;
	int iNameendloc = 0;

	int i = 0;

	int iRealdatastart = 0;
	int iRealdatalength = 0;

	int iRealnamedatastart = 0;
	int iRealnamedatalength = 0;

	if (Resourcetomem (MAKEINTRESOURCE (IDB_SETUPBINARY), &memKeyCanvas) ==0 ) {
		
		for (i=0;i<memKeyCanvas.GetSize ();i++) {
			
			if (memcmp ((char* ) szKeytagstart, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szKeytagstart)) == 0) {
				iStartloc = i;
			}

			if (memcmp ((char *) szKeytagend, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szKeytagend)) == 0) {
				iEndloc = i;
			}

			if (memcmp ((char* ) szNamekeytagstart, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szNamekeytagstart)) == 0) {
				iNamestartloc = i;
			}

			if (memcmp ((char *) szNamekeytagend, (BYTE *) memKeyCanvas.GetBuffer ()+i, strlen (szNamekeytagend)) == 0) {
				iNameendloc = i;
			}
		}


		// Finished scanning the file for tags, now display them
		m_pdiag->OutputInt ("Start Tag: ", iStartloc);
		m_pdiag->OutputInt ("End Tag: ", iEndloc);

		m_pdiag->OutputInt ("Name Start Tag: ", iNamestartloc);
		m_pdiag->OutputInt ("Name End Tag: ", iNameendloc);

		int iPointer = 0;

		if (iStartloc > 0) { // Make sure the start tag is filled in
			
			if (iEndloc > iStartloc) { // Make sure the end tag is after the start tag
				
				iRealdatastart = iStartloc+strlen(szKeytagstart); // Setup the actual offset of the key data
				iRealdatalength = iEndloc - iRealdatastart; // Now get the length we need based on where end tag is				
		
				if (m_memGenPublic.GetSize () >= iRealdatalength) {
					m_pdiag->OutputText ("Write to canvas failed! - Generated public key is larger than canvas size!!");
					return false;
				}

				iPointer = iRealdatastart;

				memKeyCanvas.Write ((char *) szDatatagstart, iPointer, strlen (szDatatagstart));
				iPointer += strlen (szDatatagstart);

				memKeyCanvas.Write (m_memGenPublic.GetBuffer (), iPointer, m_memGenPublic.GetSize ());
				iPointer += m_memGenPublic.GetSize ();

				memKeyCanvas.Write ((char *) szDatatagend, iPointer, strlen (szDatatagend));
				iPointer += strlen (szDatatagend);

				//m_memCanvasPublicKey.SetSize (iRealdatalength);
				//m_memCanvasPublicKey.Write ((BYTE *) memKeyCanvas.GetBuffer ()+iRealdatastart, 0, iRealdatalength);

				// Now save the extracted public key to a file
				//m_memCanvasPublicKey.SaveToFile ("C:\\Temp\\RSACrypt\\ExtractedPublicKeyfromCanvas.dat");
				//m_bCanvasPublicKeypresent = true;

				m_pdiag->OutputText ("Write to canvas successful - Public key written.");


				if (iNamestartloc > 0) {
					if (iNameendloc > iNamestartloc) { // Make sure the dist name end tag is after the start tag
						iRealnamedatastart = iNamestartloc + strlen (szNamekeytagstart);
						iRealnamedatalength = iNameendloc - iRealnamedatastart;

						iPointer = iRealnamedatastart;

						memKeyCanvas.Write ((char *) szDatatagstart, iPointer, strlen (szDatatagstart));
						iPointer += strlen (szDatatagstart);

						memKeyCanvas.Write ((char *) m_szDistname, iPointer, strlen (m_szDistname));
						iPointer += strlen (m_szDistname);						

						memKeyCanvas.Write ((char *) szDatatagend, iPointer, strlen (szDatatagend));
						iPointer += strlen (szDatatagend);

						m_pdiag->OutputText ("Write to name canvas successful - Distribution name written.");

					} else {
						m_pdiag->OutputText ("Read from canvas failed! - Name End tag not found!");
					}
				} else {
					m_pdiag->OutputText ("Read from canvas failed! - Name Start tag not found!");
				}

				// Write the modified setup package with public key to our member buffer
				m_memModBinary.SetSize (memKeyCanvas.GetSize ());
				m_memModBinary.Write (memKeyCanvas.GetBuffer (), 0, memKeyCanvas.GetSize ());

				// Clear the local canvas
				memKeyCanvas.Clear ();

				m_pdiag->OutputText ("Member binary buffer written ok.");

				
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

void MainWindow::OnCryptEvent (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Notify the communications object of an encryption event message.
	
}

void MainWindow::OnNotify (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
}

bool MainWindow::SaveSetupFile ()
{	
	ZeroMemory (m_szOutputfile, SIZE_STRING);	
	strcpy_s (m_szOutputfile, SIZE_STRING, "CedeCryptSetup.exe");

	char szFileonly[SIZE_STRING];
	ZeroMemory (szFileonly, SIZE_STRING);
	strcpy_s (szFileonly, SIZE_STRING, "CedeCryptSetup.exe");

	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szOutputfile;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;
	ofn.lpstrFilter = "Executable Setup Application (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";			
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = szFileonly;
	ofn.nMaxFileTitle = SIZE_STRING;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetSaveFileName (&ofn) != 0) {
		return true;
	} else {
		return false;
	}
}

bool MainWindow::SaveRecoveryFile (char *szInitialname)
{
	ZeroMemory (m_szOutputfile, SIZE_STRING);	
	strcpy_s (m_szOutputfile, SIZE_STRING, szInitialname);

	char szFileonly[SIZE_STRING];
	ZeroMemory (szFileonly, SIZE_STRING);
	strcpy_s (szFileonly, SIZE_STRING, szInitialname);

	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szOutputfile;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;
	ofn.lpstrFilter = "Recovery Key Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";			
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = szFileonly;
	ofn.nMaxFileTitle = SIZE_STRING;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetSaveFileName (&ofn) != 0) {
		return true;
	} else {
		return false;
	}
}

void MainWindow::OnCommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case IDM_FILE_EXIT:
		{
			PostQuitMessage (0);
		}
		break;
		case IDM_HELP_ABOUT:
		{
			MessageBox (NULL, "CedeCrypt Distribution Wizard. Copyright (c) 2008 Cedesoft Ltd. All Rights Reserved.", "About", MB_OK | MB_ICONINFORMATION);
		}
		break;
		case IDB_BTNBEGIN:
		{
			ZeroMemory (m_szDistname, SIZE_STRING);
			GetDlgItemText (m_hwnd, ID_EDITNAME, m_szDistname, SIZE_STRING);

			char szRecoverykeyfilename[SIZE_STRING];
			ZeroMemory (szRecoverykeyfilename, SIZE_STRING);

			if (strlen (m_szDistname) > 3) {				

				strcpy_s (szRecoverykeyfilename, SIZE_STRING, m_szDistname);
				strcat_s (szRecoverykeyfilename, SIZE_STRING, "-RecoveryKey.dat");

				RSAGenerateKey ();
				if (WritePublicKeytoResource () == true) {
					// Time to save the resource, prompt the user, then save the private recovery key
					if (SaveSetupFile () == true) {
						m_pdiag->OutputText ("Output Filename: ", m_szOutputfile);

						m_memModBinary.SaveToFile (m_szOutputfile);

						MessageBox (NULL, "You will now be prompted to save the Recovery key for this CedeCrypt distribution. Please keep this recovery key in a safe & secure place, and ensure that it is not disclosed.", "Recovery Key Information", MB_OK | MB_ICONINFORMATION);

						if (SaveRecoveryFile (szRecoverykeyfilename) == true) {
							m_pdiag->OutputText ("Output Filename: ", m_szOutputfile);

							m_memGenPrivate.SaveToFile (m_szOutputfile);

						}
					}
					

				} else {
					MessageBox (NULL, "There was a problem generating the Setup application. Please quit and try again.", "Setup Creation Problem", MB_OK | MB_ICONEXCLAMATION);
				}
			} else {
				MessageBox (NULL, "Please enter a name for this CedeCrypt Distribution.", "Name Required.", MB_OK | MB_ICONINFORMATION);
			}			
		}
		break;
	}
}

void MainWindow::OnUICommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int controlID = wParam;

	switch (controlID) {
		
	}
}

void MainWindow::OnTimer (WPARAM wParam)
{
	m_uihandler.NotifyTimer (wParam);
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
