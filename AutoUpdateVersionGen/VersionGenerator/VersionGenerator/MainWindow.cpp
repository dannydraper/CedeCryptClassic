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
	SetCaption (TEXT ("CedeCrypt VerGen v2.1"));
	SetWindowStyle (FS_STYLESTANDARD);
	CreateAppWindow ("CRYPTWindowClass", 70, 0, 420, 750, false);
	m_uihandler.SetWindowProperties (0, 70, 40, 443, GetSysColor (COLOR_BTNFACE));
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

	//m_header.SetBitmapResources (IDB_HEADER);
	//m_header.SetBitmapProperties (0, 0, 238, 64);
	//m_header.SetProperties (hWnd, CID_HEADER, 3, 3, 238, 64);
	//m_uihandler.AddDirectControl (&m_header);
	
	m_mainmenu.CreateMainMenu (hWnd);

	//m_connectpanel.Hide ();
	HFONT hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

	int textwidth = 390;
	int textheight = 19;
	int ypos = 23;
	int xpos = 10;
	int offset = 46;

	// Destination edit box
	m_webversion = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_WEBVERSION, GetModuleHandle (NULL), NULL);
	SendMessage (m_webversion, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;
	
	m_numfiles = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL, xpos, ypos, textwidth, textheight*4, hWnd, (HMENU) ID_LATESTUPDATES, GetModuleHandle (NULL), NULL);
	SendMessage (m_numfiles, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=(offset*3);

	m_numfiles = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_NUMFILES, GetModuleHandle (NULL), NULL);
	SendMessage (m_numfiles, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_localfile1 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_LOCALFILE1, GetModuleHandle (NULL), NULL);
	SendMessage (m_localfile1, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_weblocation1 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_WEBLOCATION1, GetModuleHandle (NULL), NULL);
	SendMessage (m_weblocation1, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;


		m_localfile2 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_LOCALFILE2, GetModuleHandle (NULL), NULL);
	SendMessage (m_localfile2, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_weblocation2 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_WEBLOCATION2, GetModuleHandle (NULL), NULL);
	SendMessage (m_weblocation2, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

		m_localfile3 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_LOCALFILE3, GetModuleHandle (NULL), NULL);
	SendMessage (m_localfile3, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_weblocation3 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_WEBLOCATION3, GetModuleHandle (NULL), NULL);
	SendMessage (m_weblocation3, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

		m_localfile4 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_LOCALFILE4, GetModuleHandle (NULL), NULL);
	SendMessage (m_localfile4, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_weblocation4 = CreateWindow ("edit", "<Blank Field>", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_WEBLOCATION4, GetModuleHandle (NULL), NULL);
	SendMessage (m_weblocation4, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;


	ypos = 2;
	offset = 46;

	m_static = CreateWindow ("static", "Web Version", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Latest Update News", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=(offset*3);

	m_static = CreateWindow ("static", "Number of Updated Files", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Local File 1", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Web Location 1", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Local File 2", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Web Location 2", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

		m_static = CreateWindow ("static", "Local File 3", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Web Location 3", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

		m_static = CreateWindow ("static", "Local File 4", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;

	m_static = CreateWindow ("static", "Web Location 4", WS_CHILD | WS_VISIBLE, xpos, ypos, textwidth, textheight, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
	SendMessage (m_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	ypos+=offset;


	ypos+=offset;
	// Save button
	m_btnsave = CreateWindow ("button", "Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, xpos, ypos, 70, 19, hWnd, (HMENU) ID_BTNSAVE, GetModuleHandle (NULL), NULL);
	SendMessage (m_btnsave, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

	m_btnload = CreateWindow ("button", "Open", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, xpos*12, ypos, 70, 19, hWnd, (HMENU) ID_BTNLOAD, GetModuleHandle (NULL), NULL);
	SendMessage (m_btnload, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

}

bool MainWindow::SaveSingleFile ()
{

	ZeroMemory (m_szOutputfile, SIZE_STRING);

	char szFileonly[SIZE_STRING];
	ZeroMemory (szFileonly, SIZE_STRING);

	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = m_szOutputfile;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = SIZE_STRING;

	ofn.lpstrFilter = "Version Info File (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";

	
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileonly;
	ofn.nMaxFileTitle = SIZE_STRING;
	//ofn.lpstrInitialDir = szPathonly;	
	ofn.Flags = OFN_OVERWRITEPROMPT;
	
	strcat_s (m_szOutputfile, SIZE_STRING, ".bin");

	if (GetSaveFileName (&ofn) != 0) {
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
	ofn.lpstrFilter = "Version Info File (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
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

void MainWindow::FormToFile ()
{
	char szWebversion[SIZE_STRING];
	ZeroMemory (szWebversion, SIZE_STRING);

	char szLatestupdates[SIZE_STRING];
	ZeroMemory (szLatestupdates, SIZE_STRING);

	char szNumfiles[SIZE_STRING];
	ZeroMemory (szNumfiles, SIZE_STRING);

	char szLocalfile1[SIZE_STRING];
	ZeroMemory (szLocalfile1, SIZE_STRING);

	char szWebloc1[SIZE_STRING];
	ZeroMemory (szWebloc1, SIZE_STRING);

	char szLocalfile2[SIZE_STRING];
	ZeroMemory (szLocalfile2, SIZE_STRING);

	char szWebloc2[SIZE_STRING];
	ZeroMemory (szWebloc2, SIZE_STRING);

	char szLocalfile3[SIZE_STRING];
	ZeroMemory (szLocalfile3, SIZE_STRING);

	char szWebloc3[SIZE_STRING];
	ZeroMemory (szWebloc3, SIZE_STRING);

	char szLocalfile4[SIZE_STRING];
	ZeroMemory (szLocalfile4, SIZE_STRING);

	char szWebloc4[SIZE_STRING];
	ZeroMemory (szWebloc4, SIZE_STRING);

	

	GetDlgItemText (m_hwnd, ID_WEBVERSION, szWebversion, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_LATESTUPDATES, szLatestupdates, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_NUMFILES, szNumfiles, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_LOCALFILE1, szLocalfile1, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_WEBLOCATION1, szWebloc1, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_LOCALFILE2, szLocalfile2, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_WEBLOCATION2, szWebloc2, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_LOCALFILE3, szLocalfile3, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_WEBLOCATION3, szWebloc3, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_LOCALFILE4, szLocalfile4, SIZE_STRING);
	GetDlgItemText (m_hwnd, ID_WEBLOCATION4, szWebloc4, SIZE_STRING);
	
	int imagicnumber = 47715;
	int iwebversion = atoi (szWebversion);
	int ilatestupdateslen = strlen (szLatestupdates);
	int inumfiles = atoi (szNumfiles);
	
	int ilocalfile1len = strlen (szLocalfile1);
	int iwebfile1len = strlen (szWebloc1);
	
	int ilocalfile2len = strlen (szLocalfile2);
	int iwebfile2len = strlen (szWebloc2);
	
	int ilocalfile3len = strlen (szLocalfile3);
	int iwebfile3len = strlen (szWebloc3);
	
	int ilocalfile4len = strlen (szLocalfile4);
	int iwebfile4len = strlen (szWebloc4);


	m_pdiag->OutputText (szWebversion);
	m_pdiag->OutputText (szLatestupdates);
	m_pdiag->OutputText (szNumfiles);
	m_pdiag->OutputText (szLocalfile1);
	m_pdiag->OutputText (szWebloc1);
	m_pdiag->OutputText (szLocalfile2);
	m_pdiag->OutputText (szWebloc2);
	m_pdiag->OutputText (szLocalfile3);
	m_pdiag->OutputText (szWebloc3);
	m_pdiag->OutputText (szLocalfile4);
	m_pdiag->OutputText (szWebloc4);
	
	MemoryBuffer memFile;

	memFile.SetSize ((sizeof (int) * 12) + ilatestupdateslen + ilocalfile1len + iwebfile1len + ilocalfile2len + iwebfile2len + ilocalfile3len + iwebfile3len + ilocalfile4len + iwebfile4len);
	
	memFile.Append (&imagicnumber, sizeof (int));
	memFile.Append (&iwebversion, sizeof (int));
	memFile.Append (&ilatestupdateslen, sizeof (int));
	memFile.Append ((char *) szLatestupdates, ilatestupdateslen);
	memFile.Append (&inumfiles, sizeof (int));

	memFile.Append (&ilocalfile1len, sizeof (int));
	memFile.Append ((char *) szLocalfile1, ilocalfile1len);
	memFile.Append (&iwebfile1len, sizeof (int));
	memFile.Append ((char *) szWebloc1, iwebfile1len);

	memFile.Append (&ilocalfile2len, sizeof (int));
	memFile.Append ((char *) szLocalfile2, ilocalfile2len);
	memFile.Append (&iwebfile2len, sizeof (int));
	memFile.Append ((char *) szWebloc2, iwebfile2len);

	memFile.Append (&ilocalfile3len, sizeof (int));
	memFile.Append ((char *) szLocalfile3, ilocalfile3len);
	memFile.Append (&iwebfile3len, sizeof (int));
	memFile.Append ((char *) szWebloc3, iwebfile3len);

	memFile.Append (&ilocalfile4len, sizeof (int));
	memFile.Append ((char *) szLocalfile4, ilocalfile4len);
	memFile.Append (&iwebfile4len, sizeof (int));
	memFile.Append ((char *) szWebloc4, iwebfile4len);

	memFile.SaveToFile (m_szOutputfile);
}

void MainWindow::FileToForm ()
{
	char szWebversion[SIZE_STRING];
	ZeroMemory (szWebversion, SIZE_STRING);

	char szLatestupdates[SIZE_STRING];
	ZeroMemory (szLatestupdates, SIZE_STRING);

	char szNumfiles[SIZE_STRING];
	ZeroMemory (szNumfiles, SIZE_STRING);

	char szLocalfile1[SIZE_STRING];
	ZeroMemory (szLocalfile1, SIZE_STRING);

	char szWebloc1[SIZE_STRING];
	ZeroMemory (szWebloc1, SIZE_STRING);

	char szLocalfile2[SIZE_STRING];
	ZeroMemory (szLocalfile2, SIZE_STRING);

	char szWebloc2[SIZE_STRING];
	ZeroMemory (szWebloc2, SIZE_STRING);

	char szLocalfile3[SIZE_STRING];
	ZeroMemory (szLocalfile3, SIZE_STRING);

	char szWebloc3[SIZE_STRING];
	ZeroMemory (szWebloc3, SIZE_STRING);

	char szLocalfile4[SIZE_STRING];
	ZeroMemory (szLocalfile4, SIZE_STRING);

	char szWebloc4[SIZE_STRING];
	ZeroMemory (szWebloc4, SIZE_STRING);

	int imagicnumber = 0;
	int iwebversion = 0;
	int ilatestupdateslen = 0;
	int inumfiles = 0;
	
	int ilocalfile1len = 0;
	int iwebfile1len = 0;
	
	int ilocalfile2len = 0;
	int iwebfile2len = 0;
	
	int ilocalfile3len = 0;
	int iwebfile3len = 0;
	
	int ilocalfile4len = 0;
	int iwebfile4len = 0;

	int ipointer = 0;

	MemoryBuffer memFile;
	memFile.ReadFromFile (m_szInputfile);

	memcpy (&imagicnumber, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
	ipointer+=sizeof (int);

	if (imagicnumber == 47715) {

		memcpy (&iwebversion, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);
		
		memcpy (&ilatestupdateslen, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szLatestupdates, (char *) memFile.GetBuffer ()+ipointer, ilatestupdateslen);
		ipointer+=ilatestupdateslen;

		memcpy (&inumfiles, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);


		itoa (iwebversion, szWebversion, 10);
		itoa (inumfiles, szNumfiles, 10);


		memcpy (&ilocalfile1len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szLocalfile1, (char *) memFile.GetBuffer ()+ipointer, ilocalfile1len);
		ipointer+=ilocalfile1len;

		memcpy (&iwebfile1len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szWebloc1, (char *) memFile.GetBuffer ()+ipointer, iwebfile1len);
		ipointer+=iwebfile1len;



		memcpy (&ilocalfile2len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szLocalfile2, (char *) memFile.GetBuffer ()+ipointer, ilocalfile2len);
		ipointer+=ilocalfile2len;

		memcpy (&iwebfile2len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szWebloc2, (char *) memFile.GetBuffer ()+ipointer, iwebfile2len);
		ipointer+=iwebfile2len;


		
		memcpy (&ilocalfile3len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szLocalfile3, (char *) memFile.GetBuffer ()+ipointer, ilocalfile3len);
		ipointer+=ilocalfile3len;

		memcpy (&iwebfile3len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szWebloc3, (char *) memFile.GetBuffer ()+ipointer, iwebfile3len);
		ipointer+=iwebfile3len;



		memcpy (&ilocalfile4len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szLocalfile4, (char *) memFile.GetBuffer ()+ipointer, ilocalfile4len);
		ipointer+=ilocalfile4len;

		memcpy (&iwebfile4len, (BYTE *) memFile.GetBuffer ()+ipointer, sizeof (int));
		ipointer+=sizeof (int);

		memcpy (szWebloc4, (char *) memFile.GetBuffer ()+ipointer, iwebfile4len);
		ipointer+=iwebfile4len;	

		SetDlgItemText (m_hwnd, ID_WEBVERSION, szWebversion);
		SetDlgItemText (m_hwnd, ID_LATESTUPDATES, szLatestupdates);
		SetDlgItemText (m_hwnd, ID_NUMFILES, szNumfiles);
		SetDlgItemText (m_hwnd, ID_LOCALFILE1, szLocalfile1);
		SetDlgItemText (m_hwnd, ID_WEBLOCATION1, szWebloc1);
		SetDlgItemText (m_hwnd, ID_LOCALFILE2, szLocalfile2);
		SetDlgItemText (m_hwnd, ID_WEBLOCATION2, szWebloc2);
		SetDlgItemText (m_hwnd, ID_LOCALFILE3, szLocalfile3);
		SetDlgItemText (m_hwnd, ID_WEBLOCATION3, szWebloc3);
		SetDlgItemText (m_hwnd, ID_LOCALFILE4, szLocalfile4);
		SetDlgItemText (m_hwnd, ID_WEBLOCATION4, szWebloc4);
	} else {
		MessageBox (NULL, "Invalid File Format.", "Invalid File.", MB_OK | MB_ICONEXCLAMATION);
	}
}

void MainWindow::OnCryptEvent (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Notify the communications object of an encryption event message.
	
}

void MainWindow::OnNotify (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
}

void MainWindow::OnCommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case ID_BTNLOAD:
		{
			OpenSingleFile ();

			m_pdiag->OutputText (m_szInputfile);

			if (strlen (m_szInputfile) > 1) {
				FileToForm ();
			}
		}
		break;
		case ID_BTNSAVE:
		{
			SaveSingleFile ();

			m_pdiag->OutputText (m_szOutputfile);

			if (strlen (m_szOutputfile) > 1) {
				FormToFile ();
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
