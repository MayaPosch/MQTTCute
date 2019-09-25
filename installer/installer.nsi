/* !include "MUI.nsh"

!define MUI_ABORTWARNING # This will warn the user if he exits from the installer.

!insertmacro MUI_PAGE_WELCOME # Welcome to the installer page.
!insertmacro MUI_PAGE_DIRECTORY # In which folder to install page.
!insertmacro MUI_PAGE_INSTFILES # Installing page.
!insertmacro MUI_PAGE_FINISH # Finished installation page.

!insertmacro MUI_LANGUAGE "English"

Name "MQTTCute" # Name of the installer (usually the name of the application to install).
OutFile "MQTTCute_installer.exe" # Name of the installer's file.
InstallDir "$PROGRAMFILES\MQTTCute" # Default installing folder ($PROGRAMFILES is Program Files folder).
ShowInstDetails show # This will always show the installation details.

Section "MQTTCute" # In this section add your files or your folders.
  # Add your files with "File (Name of the file)", example: "File "$DESKTOP\MyApp.exe"" ($DESKTOP is Desktop folder); or add your folders always with "File (Name of the folder)\*", always add your folders with an asterisk, example: "File /r $DESKTOP\MyApp\*" (this will add its files and (with /r its subfolders)).
  File "mosquitto.dll"
  File "mosquittopp.dll"
  File "MQTTCute.exe"
  File "pthreadVC2.dll"
  File "Qt5Core.dll"
  File "Qt5Gui.dll"
  File "Qt5Widgets.dll"
SectionEnd */

;NSIS Modern User Interface
;MQTTCute install script

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

	;Name and file
	Name "MQTTCute"
	OutFile "MQTTCute_installer.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\MQTTCute"

	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\MQTTCute" ""

	;Request application privileges for Windows Vista+
	RequestExecutionLevel admin

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	;!insertmacro MUI_PAGE_LICENSE "LICENSE"
	;!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES

	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages

	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "MQTTCute"
	SetOutPath $INSTDIR

	File "MQTTCute.exe"
	File /r *.dll
	;File "iconengines/*.dll"
	;File "platforms/*.dll"
	;File "styles/*.dll"
	;File "translations/*.qm"
	

	;Store installation folder
	WriteRegStr HKLM "Software\MQTTCute" "" $INSTDIR

	;Create uninstaller
	WriteUninstaller "$INSTDIR\uninstall.exe"
  
	; Create a shortcuts in the start menu programs directory.
	CreateDirectory "$SMPROGRAMS\MQTTCute"
    CreateShortCut "$SMPROGRAMS\MQTTCute\MQTTCute.lnk" "$INSTDIR\MQTTCute.exe"
    CreateShortCut "$SMPROGRAMS\MQTTCute\Uninstall MQTTCute.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

;--------------------------------
;Descriptions

	;Language strings
	;LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

	;Assign language strings to sections
	;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	; !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
	;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "un.Uninstall Section"
	Delete "MQTTCute.exe"
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\iconengines\*.dll
	Delete $INSTDIR\imageformats\*.dll
	Delete $INSTDIR\platforms\*.dll
	Delete $INSTDIR\styles\*.dll
	Delete $INSTDIR\translations\*.qm

	Delete "$INSTDIR\uninstall.exe"

	RMDir /r "$INSTDIR"

	DeleteRegKey /ifempty HKLM "Software\MQTTCute"
  
	Delete "$SMPROGRAMS\MQTTCute\MQTTCute.lnk"
    Delete "$SMPROGRAMS\MQTTCute\Uninstall MQTTCute.lnk"

SectionEnd