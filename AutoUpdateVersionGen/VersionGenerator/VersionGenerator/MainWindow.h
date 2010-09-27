#include <windows.h>
#include <io.h>
#include <commctrl.h>
#include "UIWindow.h"
#include "UIHandler.h"
#include "ControlCreator.h"
#include "UIPicButton.h"
#include "UIBanner.h"
#include "Diagnostics.h"
#include "UILabel.h"
#include "MenuHandler.h"
#include "Stack.h"

class MainWindow : public UIWindow
{
	public:
		MainWindow ();
		~MainWindow ();
		
		void Initialise (HWND hWnd);
		void SetDiagnostics (Diagnostics *pdiag);
		
	private:
		// Private Member Variables & objects
		bool OpenSingleFile ();
		bool SaveSingleFile ();
		void FormToFile ();
		void FileToForm ();

		char m_szInputfile[SIZE_STRING];
		char m_szInputfiletitle[SIZE_STRING];
		char m_szOutputfile[SIZE_STRING];


		// The UI Handler required for multiple handling of custom controls.
		UIHandler m_uihandler;

		// The Control Creater required for fast creation of controls
		ControlCreator m_ccontrols;
		
		//A test stack
		Stack m_stack;

		// Global hwnd
		HWND m_hwnd;		
		
		HWND m_webversion;
		HWND m_latestupdates;
		HWND m_numfiles;
		HWND m_localfile1;
		HWND m_weblocation1;
		HWND m_localfile2;
		HWND m_weblocation2;
		HWND m_localfile3;
		HWND m_weblocation3;
		HWND m_localfile4;
		HWND m_weblocation4;
		HWND m_localfile5;
		HWND m_weblocation5;
		HWND m_localfile6;
		HWND m_weblocation6;
		HWND m_localfile7;
		HWND m_weblocation7;
		HWND m_static;
		HWND m_btnsave;
		HWND m_btnload;

		// The header bitmap image
		UIBanner m_header;

		// The main menu class
		MenuHandler m_mainmenu;

		// Pointer to the global diagnostics window
		Diagnostics *m_pdiag;

		// Temporary input output buffers
		//MemoryBuffer m_inBuffer;
		//MemoryBuffer m_outBuffer;

		// event notification from base class
		void OnCreate (HWND hWnd);
		void OnNotify (HWND hWnd, WPARAM wParam, LPARAM lParam);
		void OnCommand (HWND hWnd, WPARAM wParam, LPARAM lParam);
		void OnUICommand (HWND hWnd, WPARAM wParam, LPARAM lParam);		
		void OnPaint (HWND hWnd);
		void OnTimer (WPARAM wParam);
		void OnMouseMove (HWND hWnd, int mouseXPos, int mouseYPos);
		void OnLButtonDown (HWND hWnd);
		void OnLButtonDblClick (HWND hWnd, WPARAM wParam, LPARAM lParam);				
		void OnCryptEvent (HWND hWnd, WPARAM wParam, LPARAM lParam);
		void OnLButtonUp (HWND hWnd);
};
