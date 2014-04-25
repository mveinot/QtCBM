; LabMentors.nsi
; Removes any previous file associations that may gum up the works
; Installs client exe and all other dependancies (DLLs)
;
; Sets up a new file association
; Creates an uninstaller 

!include "StrFunc.nsh"
!include "LogicLib.nsh"
!include "FileAssociation.nsh"
;!include "DotNetChecker.nsh"

;--------------------------------

SetCompressor /SOLID lzma

; The name of the installer
Name "QtCBM frontend for CBM Utils"

; The file to write
OutFile "setup.exe"

LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
;--------------------------------
;Version Information

  VIProductVersion "3.1.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "QtCBM"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Frontend for CBM utils"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "mvgrafx"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright Mark Veinot"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "QtCBM"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0"

;--------------------------------

; The default installation directory
InstallDir "$PROGRAMFILES\QtCBM"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\QtCBM" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Lab Mentors"

  SectionIn RO

  ; check for .Net4Framework
;  !insertmacro CheckNetFramework 40Client

  ; Remove existing file associations
;  ${unregisterExtension} ".elab" "eLab Session"
;  ${unregisterExtension} ".elab" "elab_auto_file"
;  ${unregisterExtension} ".elab" "ELAB_Session"
;  DeleteRegKey HKLM "SOFTWARE\Classes\eLab Session"
;  DeleteRegKey HKLM "SOFTWARE\Classes\elab_auto_file"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  !define NameStr "Write .D64 with QtCBM"
  WriteRegStr HKCR ".d64" "" "DirMaster"
  WriteRegStr HKCR "DirMaster\Shell\${NameStr}\Command" "" "$INSTDIR\QtCBM.exe $\"%1$\""
  ;File "$INSTDIR\QtCBM.exe"
  
  ; Put file there
  File "C:\Users\vmark\Google Drive\QtCBM\QtCBM.exe"
  File "C:\Users\vmark\Google Drive\QtCBM\icudt51.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\icuin51.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\icuuc51.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\libgcc_s_dw2-1.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\libstdc++-6.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\libwinpthread-1.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\Qt5Core.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\Qt5Gui.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\Qt5Widgets.dll"

  SetOutPath $INSTDIR\platforms
  
  File "C:\Users\vmark\Google Drive\QtCBM\platforms\qminimal.dll"
  File "C:\Users\vmark\Google Drive\QtCBM\platforms\qwindows.dll"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\QtCBM "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "DisplayName" "QtCBM"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "Publisher" "mvgrafx"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "DisplayIcon" '"$INSTDIR\QtCBM.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "URLInfoAbout" "http://www.mvgrafx.net"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  

  CreateShortCut "$SMPROGRAMS\QtCBM.lnk" "$INSTDIR\QtCBM.exe"

SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QtCBM"
  DeleteRegKey HKLM SOFTWARE\QtCBM

  ; Remove file association
;  ${unregisterExtension} ".elab" "ELAB_Session"

  ; Remove files and uninstaller
  Delete $INSTDIR\QtCBM.exe
  Delete $INSTDIR\icudt51.dll
  Delete $INSTDIR\icuin51.dll
  Delete $INSTDIR\icuuc51.dll
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libwinpthread-1.dll
  Delete $INSTDIR\Qt5Core.dll
  Delete $INSTDIR\Qt5Gui.dll
  Delete $INSTDIR\Qt5Widgets.dll
  Delete $INSTDIR\platforms\qminimal.dll
  Delete $INSTDIR\platforms\qwindows.dll
  
  ; Remove directories used
  RMDir "$INSTDIR"

SectionEnd
