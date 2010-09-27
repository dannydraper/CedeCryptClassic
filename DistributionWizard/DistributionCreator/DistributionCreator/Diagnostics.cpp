// This work is dedicated to the Lord our God. King of Heaven, Lord of Heaven's Armies.

#include "Diagnostics.h"

Diagnostics::Diagnostics ()
{
}

Diagnostics::~Diagnostics ()
{
}

void Diagnostics::Initialise (HWND hWnd)
{	
	SetParentHWND (hWnd);
	SetBgColor (GetSysColor (COLOR_BTNFACE));
	SetParentHWND (hWnd);
	SetCaption (TEXT ("Communicator Diagnostics"));
	SetWindowStyle (FS_STYLESTANDARD);
	CreateAppWindow ("COMMDiagnosticsClass", 70, 0, 600, 350, true);
	//m_uihandler.SetWindowProperties (0, 70, 40, 443, RGB (200, 200, 200));		
	//SetWindowPosition (FS_TOPLEFT);
	//Show ();
}

void Diagnostics::OnCreate (HWND hWnd)
{			
	
	m_header.SetBitmapResources (IDB_DIAGHEADER);
	m_header.SetBitmapProperties (0, 0, 591, 41);
	m_header.SetProperties (hWnd, CID_HEADER, 1, 1, 591, 41);
	m_uihandler.AddDirectControl (&m_header);
	
	HFONT hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);
	m_hwnddiaglist = CreateWindow ("listbox", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, 1, 43, 591, 283, hWnd, (HMENU) ID_DIAGLIST, GetModuleHandle (NULL), NULL) ;
	SendMessage (m_hwnddiaglist, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
	OutputInt ("Diagnostics Ready: ", 0);
}

void Diagnostics::OutputInt (LPCSTR lpszText, int iValue)
{
	char szInteger[SIZE_INTEGER];
	ZeroMemory (szInteger, SIZE_INTEGER);
	sprintf_s (szInteger, SIZE_INTEGER, "%d", iValue);

	char szText[SIZE_STRING];
	ZeroMemory (szText, SIZE_STRING);
	strcpy_s (szText, SIZE_STRING, lpszText);

	strcat_s (szText, SIZE_STRING, szInteger);

	SendMessage (m_hwnddiaglist, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwnddiaglist, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwnddiaglist, LB_SETCURSEL, lCount-1, 0);
}

void Diagnostics::OutputText (LPCSTR lpszText)
{
	char szText[SIZE_STRING];
	ZeroMemory (szText, SIZE_STRING);
	strcpy_s (szText, SIZE_STRING, lpszText);
	SendMessage (m_hwnddiaglist, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwnddiaglist, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwnddiaglist, LB_SETCURSEL, lCount-1, 0);
}

void Diagnostics::OutputText (LPCSTR lpszName, LPCSTR lpszValue)
{
	char szText[SIZE_STRING*2];
	ZeroMemory (szText, SIZE_STRING*2);	
	strcat_s (szText, SIZE_STRING*2, lpszName);
	strcat_s (szText, SIZE_STRING*2, lpszValue);

	SendMessage (m_hwnddiaglist, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) &szText);

	int lCount = SendMessage (m_hwnddiaglist, LB_GETCOUNT, 0, 0);
	SendMessage (m_hwnddiaglist, LB_SETCURSEL, lCount-1, 0);
}

void Diagnostics::OnCommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
}

void Diagnostics::OnUICommand (HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void Diagnostics::OnTimer (WPARAM wParam)
{	
	m_uihandler.NotifyTimer (wParam);
}

void Diagnostics::OnPaint (HWND hWnd)
{
	m_uihandler.PaintControls (hWnd);	
}

void Diagnostics::OnMouseMove (HWND hWnd, int mouseXPos, int mouseYPos)
{	
	m_uihandler.NotifyMouseMove (mouseXPos, mouseYPos);
}

void Diagnostics::OnLButtonDown (HWND hWnd)
{
	m_uihandler.NotifyMouseDown ();
}

void Diagnostics::OnLButtonUp (HWND hWnd)
{
	m_uihandler.NotifyMouseUp ();
}