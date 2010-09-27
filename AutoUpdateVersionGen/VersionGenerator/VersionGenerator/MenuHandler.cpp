#include "MenuHandler.h"

MenuHandler::MenuHandler ()
{

}

MenuHandler::~MenuHandler ()
{
	
}

void MenuHandler::SetDiagnostics (Diagnostics *pdiag)
{
	m_pdiag = pdiag;
}

void MenuHandler::CreateMainMenu (HWND hwndParent)
{
	HMENU hMenu, hSubMenu, hSubMenu2, hSubMenu3, hSubMenu4;
	
	hMenu = CreateMenu ();
		
	hSubMenu = CreatePopupMenu ();
	AppendMenu (hSubMenu, MF_STRING, IDM_FILE_EXIT, "E&xit");
	AppendMenu (hMenu, MF_STRING | MF_POPUP, (UINT) hSubMenu, "&File");

	hSubMenu2 = CreatePopupMenu ();
	AppendMenu (hSubMenu2, MF_STRING, IDM_TOOLS_OPTIONS, "O&ptions");
	AppendMenu (hMenu, MF_STRING | MF_POPUP, (UINT) hSubMenu2, "&Tools");
	
	hSubMenu4 = CreatePopupMenu ();
	AppendMenu (hSubMenu4, MF_STRING, IDM_HELP_ABOUT, "&About");
	AppendMenu (hMenu, MF_STRING | MF_POPUP, (UINT) hSubMenu4, "&Help");

	SetMenu (hwndParent, hMenu);
}

