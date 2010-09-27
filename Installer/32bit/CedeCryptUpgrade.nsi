;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "CedeCrypt"
  OutFile "CedeCryptUpgrade.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\CedeSoft\CedeCrypt"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\CedeSoft\CedeCrypt" ""

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "D:\Work\Programming\Win32Work\CedeCrypt\Installer\CCUpgrade.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\CedeSoft\CedeCrypt" 
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

Section "CedeCrypt" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File "D:\Work\Programming\Win32Work\CedeCrypt\Installer\CedeCrypt.exe"
  ;File "D:\Work\Programming\Win32Work\CedeCrypt\Installer\CedeCryptShell.dll"
  ;File "D:\Work\Programming\Win32Work\CedeCrypt\Installer\Readme.rtf"

  SetOutPath "$WINDIR"
  File "D:\Work\Programming\Win32Work\CedeCrypt\Installer\CedeCrypt.exe"
  ;File "D:\Work\Programming\Win32Work\CedeCrypt\Installer\CCrypt.dll"
  
  ;Store installation folder
  ;WriteRegStr HKCU "Software\CedeSoft\CedeCrypt" "" $INSTDIR
  
  ;Create uninstaller
  ;WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ;!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    ;CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\CedeCrypt.lnk" "$INSTDIR\CedeCrypt.exe"    
    ;CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme.lnk" "$INSTDIR\Readme.rtf"
    ;CreateShortCut "$DESKTOP\CedeCrypt.lnk" "$INSTDIR\CedeCrypt.exe"
  ;!insertmacro MUI_STARTMENU_WRITE_END

  ; Create the file association
  ;Exec '"$INSTDIR\CedeCrypt.exe" /i'

  ; Register the Shell COM Object
  ;Exec '"regsvr32" /s "$INSTDIR\CedeCryptShell.dll"'

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "CedeCrypt - Powerful encryption to secure your data."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

;Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  ;Exec '"regsvr32" /u /s "$INSTDIR\CedeCryptShell.dll"'
  ;Exec '"$INSTDIR\CedeCrypt.exe" /u'

  ;Delete "$INSTDIR\Uninstall.exe"
  ;Delete "$INSTDIR\CedeCrypt.exe"
  ;Delete "$WINDIR\CedeCrypt.exe"
  ;Delete "$WINDIR\CCrypt.DLL"
  ;Delete "$INSTDIR\Readme.rtf"
  
  ;RMDir "$INSTDIR"
  
  ;!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
    
  ;Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  ;Delete "$SMPROGRAMS\$MUI_TEMP\CedeCrypt.lnk"
  ;Delete "$SMPROGRAMS\$MUI_TEMP\Readme.lnk"
  ;Delete "$DESKTOP\CedeCrypt.lnk"
  
  ;Delete empty start menu parent diretories
  ;StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
  ;startMenuDeleteLoop:
	;ClearErrors
    ;RMDir $MUI_TEMP
    ;GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    ;IfErrors startMenuDeleteLoopDone
  
    ;StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  ;startMenuDeleteLoopDone:

  ;DeleteRegKey /ifempty HKCU "Software\CedeSoft\CedeCrypt"

;SectionEnd