;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  SetCompress off
  CRCCheck off
  ;Name and file
  Name "CedeCrypt Distribution Wizard"
  OutFile "DistributionSetupWizard.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\CedeSoft\CedeCryptDistributionWizard"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\CedeSoft\CedeCryptDistributionWizard" ""

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "D:\Work\Programming\Win32Work\CedeCrypt\DistributionWizard\Installer\DWLicense.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\CedeSoft\CedeCryptDistributionWizard" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Distribution Wizard" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File "D:\Work\Programming\Win32Work\CedeCrypt\DistributionWizard\Installer\DistributionCreator.exe"
  File "D:\Work\Programming\Win32Work\CedeCrypt\DistributionWizard\Installer\Readme.rtf"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\CedeSoft\CedeCryptDistributionWizard" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetShellVarContext current
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\CedeCrypt Distribution Wizard.lnk" "$INSTDIR\DistributionCreator.exe"    
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme.lnk" "$INSTDIR\Readme.rtf"
    CreateShortCut "$DESKTOP\CedeCrypt Distribution Wizard.lnk" "$INSTDIR\DistributionCreator.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "CedeCrypt Distribution Wizard - Manage CedeCrypt Recovery across your whole organisation."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\Uninstall.exe"
  Delete /REBOOTOK "$INSTDIR\DistributionCreator.exe"
  Delete "$INSTDIR\Readme.rtf"  
  
  RMDir /REBOOTOK "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
SetShellVarContext current


  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\CedeCrypt Distribution Wizard.lnk"  
  Delete "$SMPROGRAMS\$MUI_TEMP\Readme.lnk"
  Delete "$DESKTOP\CedeCrypt Distribution Wizard.lnk"
  
  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    IfErrors startMenuDeleteLoopDone
  
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\CedeSoft\CedeCryptDistributionWizard"

SectionEnd