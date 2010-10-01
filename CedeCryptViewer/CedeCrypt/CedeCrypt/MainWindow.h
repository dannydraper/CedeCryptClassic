#include <windows.h>
#include <io.h>
#include <shlobj.h>
#include <commctrl.h>
#include <winable.h>

#include "UIWindow.h"
#include "UIHandler.h"
#include "ControlCreator.h"
#include "UIPicButton.h"
#include "UIBanner.h"
#include "Diagnostics.h"
#include "UILabel.h"
#include "MenuHandler.h"
#include "Stack.h"
#include "PasswordWindow.h"
#include "Encryption.h"
#include "MemoryBuffer.h"
#include "AboutWindow.h"
#include "DirScanner.h"
#include "FileShredder.h"
#include "ProtectedFoldersWindow.h"
#include "TextWindow.h"
#include "UTEOptionsWindow.h"
#include "InternetHandler.h"
#include "AppUpdater.h"
#include "ToolsOptionsWindow.h"
#include "Constants.h"
#include "PFolderRequestWindow.h"

#define MODE_FILE	7000
#define MODE_FOLDER	7001
#define MODE_TEXT	7004

#define MODE_ENCRYPT 7002
#define MODE_DECRYPT 7003
#define APP_ENCRYPT 9000
#define APP_DECRYPT	9001

class MainWindow : public UIWindow
{
	public:
		MainWindow ();
		~MainWindow ();
		
		void Initialise (HWND hWnd, LPSTR lpCmdLine);
		void SetDiagnostics (Diagnostics *pdiag);
		BOOL MySystemShutdown();
		void CheckApplicationUpdate (bool bunattended);
		bool ValidateOS ();
		
	private:
		// Private Member Variables & objects
		static INT_PTR CALLBACK OptionsDlgProc (HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		static bool m_sbcancelwaitdlg;
		//bool m_sbcancelwaitdlg;

		BOOL ShowWaitDialog ();

		bool OpenSingleFile ();
		bool SaveSingleFile ();
		bool BrowseForFolder (LPCTSTR szTitle, char *szOutPath);
		void OutputStatusInt (LPCSTR lpszText, int iValue);
		void OutputStatusText (LPCSTR lpszText);
		void OutputStatusText (LPCSTR lpszName, LPCSTR lpszValue);
		bool DoesFileExist (char *szFilepath, char *szFilename);
		void ValidateInput ();
		void DoEncryption ();
		void BrowseSource ();
		void BrowseDestination ();
		void SaveEncryptedFile ();
		unsigned int CalcInputChecksum ();
		unsigned int CalcOutputChecksum ();
		void ParseCommandLine (char *pszCmdline);
		void GetFileList (char *szFolderPath);
		void Test ();
		void CombineKeys (MemoryBuffer *memKeyOne, MemoryBuffer *memKeyTwo, MemoryBuffer *memKeyOut);
		int Resourcetomem (LPCTSTR ResourceName, MemoryBuffer *memResource);
		void PrepareShellInfoPath ();
		void PrepareTempCommandFile (char *pszOrigPath);
		bool FileExists (char *FileName);
		void CheckShellCommand ();
		bool CheckProtectedModeFiles (bool bEncrypt);
		void DoSessionDecryption ();
		bool DoSessionEncryption ();

		bool DoesShellCommandExist ();
		void RSAGenandEncrypt ();

		void RSAGenerateKey ();
		void WritetoKeyCanvas ();
		bool GetKeyfromCanvas ();
		bool GetDistributionfromCanvas ();

		void RSAPrivateFromPublic ();
		void EncryptUsingPublic ();
		void DecryptUsingPrivate ();
		bool IsDirectory (char *szPathname);
		void SecurePassword ();
		void SecurePasswordUsingCanvas ();
		void RecoverPassword ();
		bool GetRecoveryPassword (MemoryBuffer *memPrivateKey, MemoryBuffer *memEncpassword);
		void RecoverCanvasPassword ();
		bool OpenPrivateKey ();
		void CheckDirectCommand ();
		void CheckSessionAction ();

		void CreateFileAssociation ();
		void RemoveFileAssociation ();

		void GetFileExtension (bool &bFound, char *pszFilePath, char *pszOutExtension);
		void SendKeyState(int vkeycode);
		void SetShiftState(int sstate);
		void GetClipboard ();
		void DoTextEncryption ();
		void SaveEncryptedText ();
		void MakeReadable (MemoryBuffer *memInput, MemoryBuffer *memOutput);
		bool CheckEnclosingTags (MemoryBuffer *memReadable);
		bool CheckStartTag (MemoryBuffer *memReadable);
		int GetStartTagLocation (MemoryBuffer *memReadable);
		int GetEndTagLocation (MemoryBuffer *memReadable);
		void MakeBinary (MemoryBuffer *memInput, MemoryBuffer *memOutput);
		void CreateShellExtensionPath ();
		unsigned int GetVirtualKeyCode (char *szChar);
		void RegisterEncryptionHotkey (bool bUseCtrl, bool bUseAlt, LPCSTR szHotkey);
		void RegisterDecryptionHotkey (bool bUseCtrl, bool bUseAlt, LPCSTR szHotkey);

		void DoUpdateCheck (bool unattended);
		static DWORD WINAPI UpdateThreadProc (PVOID pParam);
		static DWORD WINAPI UpdateThreadProcUnat (PVOID pParam);
		void StoreUpdateCurrentDate ();
		bool IsUpdateCheckDateDifferent ();
		void DoAutomaticUpdateCheck ();

		// The primary encryption class
		Encryption m_crypt;

		// File input and output buffers
		MemoryBuffer m_fileinput;
		MemoryBuffer m_fileinputex;
		MemoryBuffer m_fileoutput;
		MemoryBuffer m_fileoutputex;

		// The UI Handler required for multiple handling of custom controls.
		UIHandler m_uihandler;


		// Test Encryption object
		Encryption m_keycrypt;
		
		//*******************************************
		// MSCryptAPI stuff
		HCRYPTPROV hProvider;
		HCRYPTKEY hGeneratedKey;
		MemoryBuffer m_memExportedPublicKey;
		MemoryBuffer m_memExportedPrivateKey;

		MemoryBuffer m_memDataBuffer; // plaintext data
		MemoryBuffer m_memEncrypted; // encrypted data
		MemoryBuffer m_memCanvasEncrypted; // encrypted data
		MemoryBuffer m_memDecrypted; // decrypted data

		MemoryBuffer m_memImportedPublicKey;
		HCRYPTKEY hImportedPublic;

		MemoryBuffer m_memImportedPrivateKey;
		HCRYPTKEY hImportedPrivate;

		HCRYPTKEY hSecondGeneratedKey;
		MemoryBuffer m_memExportedSecondPrivateKey;

		MemoryBuffer m_memEncpassword;

		MemoryBuffer m_memGenPublic;
		MemoryBuffer m_memGenPrivate;
		MemoryBuffer m_memCanvasPublicKey;
		bool m_bCanvasPublicKeypresent;

		//*******************************************

		// The Control Creater required for fast creation of controls
		ControlCreator m_ccontrols;
		
		//*****************************

		// Recursive Directory Scanner class
		DirScanner m_Dirscanner;

		// File shredding class for secure
		// deletions
		FileShredder m_fileshred;

		// ******** PROTECTED MODE (SESSION) ***
		// protected folders window
		ProtectedFoldersWindow m_pfolders;
		bool m_bSessionmode;
		bool m_bProtectedfilesexist;
		char m_szSessionpassword[SIZE_NAME];
		bool m_bUsesessionpassword;
		bool m_bSessionpasswordpresent;
		bool m_bDecryptionsessioncheck;
		bool m_bSessionshuttingdown;
		bool m_bShutdownencryptdone;
		// *************************************


		// Text encryption stuff
		MemoryBuffer m_memTextbuffer;
		MemoryBuffer m_memTextoutput;
		MemoryBuffer m_memTextprocessed;		
		MemoryBuffer m_memTextreadable;
		MemoryBuffer m_memTextpasteable;
		MemoryBuffer m_memTextparseable;
		MemoryBuffer m_memTextfinished;

		TextWindow m_textwindow;
		UTEOptionsWindow m_uteoptions;

		HWND m_lastactivewindow;

		// File flags
		unsigned int m_inputchecksum;
		unsigned int m_inputid;
		unsigned int m_checksum;
		unsigned int m_ccryptid;
		unsigned int m_ccryptid2;
		unsigned int m_ccryptid3;
		unsigned int m_ccryptidprev1;

		// The password request window
		PasswordWindow m_passwindow;

		// The about window
		AboutWindow m_aboutwindow;

		// Tools Options Window
		ToolsOptionsWindow m_optionswindow;

		// Protected Folders Request window (if the user has turned on the Protected Folders prompt)
		PFolderRequestWindow m_pfrequest;

		// The user password
		char m_szPassword[SIZE_NAME];

		// Windows controls
		HWND m_hwndgroup;
		HWND m_hwndgroup1;
		HWND m_hwndgroup2;
		HWND m_hwndgroup3;
		HWND m_hwndoptfile;
		HWND m_hwndoptfolder;
		HWND m_hwndoptencrypt;
		HWND m_hwndoptdecrypt;
		HWND m_hwndeditsource;
		HWND m_hwndeditdest;
		HWND m_hwndbtnbegin;
		HWND m_hwndbtnsrcbrowse;
		HWND m_hwndbtndstbrowse;
		HWND m_hwndbtnexit;
		HWND m_hwndlststatus;
		HWND m_hwndprogress;
		HWND m_hwndtotalprogress;
		HWND m_hwndphase;
		HWND m_chkkeepopen;
		HWND m_btncancel;
		HWND m_hwndlbltotal;

		// Application flags
		unsigned int m_filemode;
		unsigned int m_appmode;
		bool m_btextmode;
		char m_szInputfile[SIZE_STRING];
		char m_szInputfiletitle[SIZE_STRING];
		char m_szOutputfile[SIZE_STRING];

		char m_szInputfolder[SIZE_STRING];
		char m_szPrivateKey[SIZE_STRING];
		char m_szPrivateKeyTitle[SIZE_STRING];

		char m_szCommandPath[SIZE_STRING];
		char m_szCommandTempFile[SIZE_STRING];
		// Flag to indicate whether we
		// delete the original files
		bool m_bDeleteorig;

		// The name of this particular CedeCrypt distribution
		// this will be either set to CedeSoft or read from
		// the distribution string in the binary resource
		// BINARY3
		char m_szDistributionname[SIZE_STRING];

		// Flag to indicate if we're in shell mode
		bool m_bShellmode;
		bool m_bCommandmode;
		bool m_bCreatewindow;

		// Application mode
		unsigned int m_cmdmode;

		// Folder pointer
		int m_iFilepointer;

		// A flag to indicate the user wishes
		// to cancel the operation
		bool m_bAwaitingcancel;

		// Flag indicating that recovery mode
		// is activated
		bool m_bRecoverymode;
		bool m_bCanvasRecoverymode;

		//A test stack
		Stack m_stack;

		// Global hwnd
		HWND m_hwnd;		

		// The header bitmap image
		UIBanner m_header;

		// The registry handler
		RegistryHandler m_registry;

		// The main menu class
		MenuHandler m_mainmenu;

		// Pointer to the global diagnostics window
		Diagnostics *m_pdiag;

		// List of input files if used in folder
		// mode
		DynList m_dlFilelist;

		// The shell info path - the path for the shell
		// command file if one is present
		char m_szShellInfoPath[SIZE_STRING];

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
		void OnHotKey (HWND hWnd, WPARAM wParam, LPARAM lParam);
		void OnMouseMove (HWND hWnd, int mouseXPos, int mouseYPos);
		void OnLButtonDown (HWND hWnd);
		void OnLButtonDblClick (HWND hWnd, WPARAM wParam, LPARAM lParam);				
		void OnCryptEvent (HWND hWnd, WPARAM wParam, LPARAM lParam);
		void OnLButtonUp (HWND hWnd);
		LRESULT OnQueryEndSession (HWND hWnd, LPARAM lParam);
		LRESULT OnEndSession (HWND hWnd, WPARAM wParam, LPARAM lParam);
		LRESULT	OnPowerBroadcast (HWND hWnd, WPARAM wParam, LPARAM lParam);
};
