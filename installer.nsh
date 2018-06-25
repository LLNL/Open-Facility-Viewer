;OFV installation script for NSIS.
;Originally based on NSIS Modern User Interface - Start Menu Folder Selection Example Script
;Also Orignally based on Delta3d installer and SceneWorks installer
;Written by John Goforth

; Include paths found by CMake
; Anything that needs to be installed from our development release
; Folders is found using cmake, with the paths written into this header
; file. This is needed becaues you can't use variables in the NSIS File
; command and CMake already knows where they are anyway.
!include cmake.nsh

; Use zlib if you want compression time faster for testing.
; decompression should still be fast with lzma
SetCompressor /SOLID /FINAL lzma
SetCompressorDictSize 32

   


!ifndef EXENAME
  !define EXENAME 'OFV' ; most recent rev between all trees
 !endif
 
 !ifndef VERSION
  !define VERSION '2.0.4' ; ALSO CHANGE VERSION IN MAINWINDOW.CPP !!!!!!
!endif


!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile OFV_${VERSION}_x64_setup.exe
!endif

;Name and file
Name "Open Facility Viewer"


; MUI 1.67 compatible ------
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"


  ;Default installation folder
  InstallDir "$PROGRAMFILES64\${EXENAME}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\OFV" ""

  ;Request application privileges for Windows Vista/7
  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder
  


;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE ".\LICENSE-OFV.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\OFV" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

  

;Installer Sections

!define EXCLUDES "/x *svn /x *d.exe /x *d.dll /x *debug* /x *.pdb /x *.idb /x *.exp"
Section "OFV Software" SecAll

  
  ;;;;;;;;;;;; OFV BINS ;;;;;;;;;;;;;;;;;;;
  ;Add the binaries 
  SetOutPath $INSTDIR
  File .\bin\OFV.exe
  File /r /x *d.dll .\bin\*.dll
  File .\README.txt
  File .\*.png
  
  ;;;;;;;;;;; EXTERNAL BINS ;;;;;;;;;;;;;;;;;;;;
   SetOutPath $INSTDIR
   
  ;Add the Delta3d stuff
   File ${EXCLUDES} ${DELTA_DIR}\bin\*dt*.dll
   File ${EXCLUDES} ${DELTA_DIR}\ext\bin\*.dll
   File ${EXCLUDES} ${DELTA_DIR}\ext\bin\osgPlugins-3.4.0\*.dll
   File ${DELTA_DIR}\ext\bin\cal3d.dll   ;previous cmd forgot this one
   ;extra tools
   File ${EXCLUDES} ${DELTA_DIR}\bin\STAGE.exe
   File ${EXCLUDES} ${DELTA_DIR}\bin\ObjectViewer.exe
   ;File ${EXCLUDES} .\ext\OpenSceneGraph-3.2.3_x64\bin\osgconv.exe
   ;File ${EXCLUDES} .\ext\OpenSceneGraph-3.2.3_x64\bin\osgviewer.exe

;   File ${EXCLUDES} ${DELTA_DIR}\..\gdal-1.9.2\bin\gdal\apps\gdal_translate.exe

   ;no ext dir yet!
   ;File ${EXCLUDES} ${OFV_EXT_DIR}\bin\*.dll
   ;File ${EXCLUDES} ${OFV_EXT_DIR}\bin\*.exe
   
       
   ;Add the Qt Stuff only if we are not an SDK install.
   SetOutPath $INSTDIR
   File /x *d4.dll ${QT_BIN}\*.dll
 
   ;;;;;;;;;;; EXTERNAL DATA (delta3d) ;;;;;;;;;;;;;;;;;;;;
   SetOutPath $INSTDIR\data
   File /r /x *svn ${DELTA_DIR}\data\*.*
   
 

  
  ;Store installation folder
  WriteRegStr HKCU "Software\OFV" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
	SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	SetShellVarContext all
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	


	SetOutPath $INSTDIR ;needed to get the working directory right.
	SetShellVarContext all
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\READ ME FIRST.lnk" "notepad.exe" "$INSTDIR\README.txt"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Open Facility Viewer.lnk" "$INSTDIR\${EXENAME}.exe"
	
	
 !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd



Section "Shapash Scenario" IAEA

;;;;;;;;;;;; Processing SCENARIO DATA ;;;;;;;;;;;;;;;;;;;;;
  ;add the config.xml file one directory up from bin!


  SetOutPath $INSTDIR
  ;File .\config.xml
  
  ;Add the IAEA Data
  ;SetOutPath $INSTDIR\data
  ;File /r /x *git /x *death_valley* /x *iaea* .\data\*.*
  ;File /r /x *git .\data\*.*
  
  ;SFX files should include 

   SetOutPath $INSTDIR
   ; ExecToLog is silent, no console opens up, puts output to our log window
	;nsExec::ExecToLog '"$EXEDIR\TerrainSFX.exe" x -y'

	MessageBox MB_OK "About to extract scenario data. This may take a few minutes, please wait until finished."
	
    SetOutPath $INSTDIR
    ;ExecToLog is silent, no console opens up, puts output to our log window
	nsExec::ExecToLog '"$EXEDIR\ModelSFX.exe" x -y'
	
SectionEnd ;"IAEA Scenarios" 


	
	
SectionGroup /e "Additional Installations"


	

	Section "VCRedist Packages" VCRedistSection
	  ; TODO: Maybe temp would be a better path?
	  SetOutPath $INSTDIR\redist
	  File /nonfatal "..\OFV\redist\VS2015_x64\vcredist_x64.exe"
	  SetOutPath $TEMP
	  ExecWait "$INSTDIR\redist\VS2015_x64\vcredist_x64.exe /Q"
	  
	SectionEnd
	
	
	; Environment Variables	
	Section "Set Environment Variables" EnvironmentVariableSection
	!define ALL_USERS
	!define DELTA_DATA "$INSTDIR\data"

	   
	   Push "DELTA_DATA"
	   Push '${DELTA_DATA}'
	   Call WriteEnvExpStr

	   ;THIS CAUSES STAGE TO NOT WORK.
	   ;Push "DELTA_THREADING"
	   ;Push 'DrawThreadPerContext'
	   ;Call WriteEnvExpStr
	   

	SectionEnd
	
SectionGroupEnd ;"Additional Installations"

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecAll ${LANG_ENGLISH} "All the Software and Data files."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAll} $(DESC_SecAll)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components? WARNING: This will remove ALL files in the install directory." IDYES +2
  Abort
FunctionEnd

Section Uninstall



  ;environment variables
  MessageBox MB_YESNO "Remove DELTA_DATA Environment Variable ?" IDYES removeEnvVars IDNO next
  removeEnvVars:
 ; Push "DELTA_ROOT"
 ; Call un.DeleteEnvStr
  Push "DELTA_DATA"
  Call un.DeleteEnvStr
  

  
  ;Push "$INSTDIR\bin"
  ;Call un.RemoveFromPath
  ;Push "$INSTDIR\ext\bin"
  ;Call un.RemoveFromPath
  next:

  RMDIR /r $INSTDIR
  

  
 ;my uninstall code
 
  ;environment variables


  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
 
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\OFV"
   SetAutoClose true
SectionEnd




;------------------------------------------------------

!ifndef _WriteEnvStr_nsh
!define _WriteEnvStr_nsh

!include WinMessages.nsh

!ifndef WriteEnvStr_RegKey
  !ifdef ALL_USERS
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
  !else
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
  !endif
!endif

#
# WriteEnvStr - Writes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Push "C:\New Home Dir\"  # value
#  Call WriteEnvStr
#
Function WriteEnvStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2

  WriteRegStr ${WriteEnvStr_RegKey} $0 $1
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
	0 "STR:Environment" /TIMEOUT=5000

 
    Pop $2
    Pop $0
    Pop $1
FunctionEnd

#
# WriteEnvExpStr - Writes an environment variable as an expanding string type
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Push "C:\New Home Dir\"  # value
#  Call WriteEnvExpStr
#
Function WriteEnvExpStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2
  
	WriteRegExpandStr ${WriteEnvStr_RegKey} $0 $1
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
	0 "STR:Environment" /TIMEOUT=5000


    Pop $2
    Pop $0
    Pop $1
FunctionEnd


#
# un.DeleteEnvStr - Removes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Call un.DeleteEnvStr
#
Function un.DeleteEnvStr
  Exch $0 ; $0 now has the name of the variable
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5

 
  
    DeleteRegValue ${WriteEnvStr_RegKey} $0
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
      0 "STR:Environment" /TIMEOUT=5000

  
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

!ifndef IsNT_KiCHiK
!define IsNT_KiCHiK

#
# [un.]IsNT - Pushes 1 if running on NT, 0 if not
#
# Example:
#   Call IsNT
#   Pop $0
#   StrCmp $0 1 +3
#     MessageBox MB_OK "Not running on NT!"
#     Goto +2
#     MessageBox MB_OK "Running on NT!"
#
!macro IsNT UN
Function ${UN}IsNT
  Push $0
  ReadRegStr $0 HKLM \
    "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0  
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."

!endif ; IsNT_KiCHiK

!endif ; _WriteEnvStr_nsh









!ifndef _AddToPath_nsh
!define _AddToPath_nsh

!verbose 3
!include "WinMessages.NSH"
!verbose 4

!ifndef WriteEnvStr_RegKey
  !ifdef ALL_USERS
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
  !else
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
  !endif
!endif




!ifndef IsNT_KiCHiK
!define IsNT_KiCHiK

###########################################
#            Utility Functions            #
###########################################

; IsNT
; no input
; output, top of the stack = 1 if NT or 0 if not
;
; Usage:
;   Call IsNT
;   Pop $R0
;  ($R0 at this point is 1 or 0)

!macro IsNT un
Function ${un}IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."

!endif ; IsNT_KiCHiK

; StrStr
; input, top of stack = string to search for
;        top of stack-1 = string to search in
; output, top of stack (replaces with the portion of the string remaining)
; modifies no other variables.
;
; Usage:
;   Push "this is a long ass string"
;   Push "ass"
;   Call StrStr
;   Pop $R0
;  ($R0 at this point is "ass string")

!macro StrStr un
Function ${un}StrStr
Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrCpy $R4 0
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len(needle)
  ; $R4=cnt
  ; $R5=tmp
  loop:
    StrCpy $R5 $R2 $R3 $R4
    StrCmp $R5 $R1 done
    StrCmp $R5 "" done
    IntOp $R4 $R4 + 1
    Goto loop
done:
  StrCpy $R1 $R2 "" $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
FunctionEnd
!macroend
!insertmacro StrStr ""
!insertmacro StrStr "un."

!endif ; _AddToPath_nsh

Function Trim ; Added by Pelaca
	Exch $R1
	Push $R2
Loop:
	StrCpy $R2 "$R1" 1 -1
	StrCmp "$R2" " " RTrim
	StrCmp "$R2" "$\n" RTrim
	StrCmp "$R2" "$\r" RTrim
	StrCmp "$R2" ";" RTrim
	GoTo Done
RTrim:
	StrCpy $R1 "$R1" -1
	Goto Loop
Done:
	Pop $R2
	Exch $R1
FunctionEnd
