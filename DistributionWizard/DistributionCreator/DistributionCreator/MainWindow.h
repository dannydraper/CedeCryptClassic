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
		int Resourcetomem (LPCTSTR ResourceName, MemoryBuffer *memResource);
		void RSAGenerateKey ();
		bool WritePublicKeytoResource ();
		bool SaveSetupFile ();
		bool SaveRecoveryFile (char *szInitialname);
	private:
		// Private Member Variables & objects
		
		// The UI Handler required for multiple handling of custom controls.
		UIHandler m_uihandler;

		// The Control Creater required for fast creation of controls
		ControlCreator m_ccontrols;
		
		//A test stack
		Stack m_stack;

		// Global hwnd
		HWND m_hwnd;
		HWND m_lblInformation;
		HWND m_lblname;
		HWND m_txtname;
		HWND m_btnbegin;

		// MSCryptAPI stuff
		HCRYPTPROV hProvider;
		HCRYPTKEY hGeneratedKey;
		MemoryBuffer m_memGenPublic;
		MemoryBuffer m_memGenPrivate;

		// Modified Setup binary
		MemoryBuffer m_memModBinary;

		char m_szDistname[SIZE_STRING];
		char m_szOutputfile[SIZE_STRING];

		// The header bitmap image
		UIBanner m_header;

		UILabel m_title;

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
