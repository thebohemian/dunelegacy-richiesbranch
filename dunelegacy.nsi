SetCompressor /SOLID lzma

!include MUI2.nsh

Name "Dune Legacy"
!define INSTALLATIONNAME "Dune Legacy"
OutFile "Dune Legacy Setup.exe"
InstallDir "$PROGRAMFILES\${INSTALLATIONNAME}"

RequestExecutionLevel user

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "COPYING"

!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!insertmacro MUI_PAGE_DIRECTORY

VAR DunePAKDirectory
!define MUI_DIRECTORYPAGE_VARIABLE $DunePAKDirectory
!define MUI_PAGE_HEADER_TEXT "Dune II Pak-Files"
!define MUI_PAGE_HEADER_SUBTEXT "Choose the directory where the installer can copy the Dune II Pak-Files from"
!define MUI_DIRECTORYPAGE_TEXT_TOP "Dune Legacy needs the PAK-Files from original Dune II. These files can be found in the Dune II folder. The installer will copy the following files from there to the Dune Legacy folder:$\n$\tHARK.PAK$\t$\tSCENARIO.PAK$\t$\tINTRO.PAK$\n$\tATRE.PAK$\t$\tMENTAT.PAK$\t$\tINTROVOC.PAK$\n$\tORDOS.PAK$\t$\tVOC.PAK$\t$\tSOUND.PAK$\n$\tENGLISH.PAK$\t$\tMERC.PAK$\t$\tGERMAN.PAK (if available)$\n$\tDUNE.PAK$\t$\tFINALE.PAK$\t$\tFRENCH.PAK (if available)"
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Dune II Pak-Files Directory"
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE OnDunePakDirectorySelectionLeave
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Function OnDunePakDirectorySelectionLeave
  IfFileExists "$DunePAKDirectory\HARK.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\ATRE.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\ORDOS.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\ENGLISH.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\DUNE.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\SCENARIO.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\MENTAT.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\VOC.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\MERC.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\FINALE.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\INTRO.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\INTROVOC.PAK" 0 FileNotFound
  IfFileExists "$DunePAKDirectory\SOUND.PAK" 0 FileNotFound

  CreateDirectory "$INSTDIR"

  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\HARK.PAK" "$INSTDIR" 424
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\ATRE.PAK" "$INSTDIR" 402
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\ORDOS.PAK" "$INSTDIR" 452
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\ENGLISH.PAK" "$INSTDIR" 88
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\DUNE.PAK" "$INSTDIR" 399
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\SCENARIO.PAK" "$INSTDIR" 310
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\MENTAT.PAK" "$INSTDIR" 561
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\VOC.PAK" "$INSTDIR" 257
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\MERC.PAK" "$INSTDIR" 45
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\FINALE.PAK" "$INSTDIR" 569
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\INTRO.PAK" "$INSTDIR" 1196
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\INTROVOC.PAK" "$INSTDIR" 953
  CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\SOUND.PAK" "$INSTDIR" 1280

  IfFileExists "$DunePAKDirectory\GERMAN.PAK" 0 +2
    CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\GERMAN.PAK" "$INSTDIR"

  IfFileExists "$DunePAKDirectory\FRENCH.PAK" 0 +2
    CopyFiles /SILENT /FILESONLY "$DunePAKDirectory\FRENCH.PAK" "$INSTDIR"

  Return

  FileNotFound:
    MessageBox MB_OK 'Cannot find the needed PAK-Files in "$DunePAKDirectory"'
    Abort
FunctionEnd


Section ""
  SetOutPath $INSTDIR\maps\singleplayer
  File "data\maps\singleplayer\2P - 32x128 - Canyon.ini"
  File "data\maps\singleplayer\2P - 64x64 - North vs. South.ini"

  SetOutPath $INSTDIR\maps\multiplayer
  File "data\maps\multiplayer\2P - 32x32 - X-Factor.ini"

  SetOutPath $INSTDIR
  File data\dunelegacy.exe
  File data\LEGACY.PAK
  File data\Dune2-Versions.txt
  File data\SDL.dll
  File data\SDL_mixer.dll
  File data\libogg-0.dll
  File data\libvorbis-0.dll
  File data\libvorbisfile-3.dll
  File data\smpeg.dll

  File COPYING
  Push "$INSTDIR\COPYING"
  Push "$INSTDIR\License.txt"
  Call unix2dos

  File AUTHORS
  Push "$INSTDIR\AUTHORS"
  Push "$INSTDIR\Authors.txt"
  Call unix2dos

  File README
  Push "$INSTDIR\README"
  Push "$INSTDIR\Readme.txt"
  Call unix2dos

  WriteUninstaller $INSTDIR\uninstall.exe
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "DisplayName" "${INSTALLATIONNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "NoRepair" 1
SectionEnd

Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\${INSTALLATIONNAME}"
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\Dune Legacy.lnk" "$INSTDIR\dunelegacy.exe" "" "$INSTDIR\dunelegacy.exe" 0
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\Readme.lnk" "$INSTDIR\Readme.txt"
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\License.lnk" "$INSTDIR\License.txt"
  
  WriteINIStr "$INSTDIR\Dune Legacy Website.URL" "InternetShortcut" "URL" "http://dunelegacy.sourceforge.net/"
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\Dune Legacy Website.lnk" "$INSTDIR\Dune Legacy Website.URL"
  
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}"
  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

  Delete "$SMPROGRAMS\${INSTALLATIONNAME}\*.*"
  RMDir "$SMPROGRAMS\${INSTALLATIONNAME}"
SectionEnd


Function unix2dos
    ; strips all CRs
    ; and then converts all LFs into CRLFs
    ; (this is roughly equivalent to "cat file | dos2unix | unix2dos")
    ;
    ; usage:
    ;    Push "infile"
    ;    Push "outfile"
    ;    Call unix2dos
    ;
    ; beware that this function destroys $0 $1 $2

    ClearErrors

    Pop $2
    FileOpen $1 $2 w 

    Pop $2
    FileOpen $0 $2 r

    Push $2 ; save name for deleting

    IfErrors unix2dos_done

    ; $0 = file input (opened for reading)
    ; $1 = file output (opened for writing)

unix2dos_loop:
    ; read a byte (stored in $2)
    FileReadByte $0 $2
    IfErrors unix2dos_done ; EOL
    ; skip CR
    StrCmp $2 13 unix2dos_loop
    ; if LF write an extra CR
    StrCmp $2 10 unix2dos_cr unix2dos_write

unix2dos_cr:
    FileWriteByte $1 13

unix2dos_write:
    ; write byte
    FileWriteByte $1 $2
    ; read next byte
    Goto unix2dos_loop

unix2dos_done:

    ; close files
    FileClose $0
    FileClose $1

    ; delete original
    Pop $0
    Delete $0

FunctionEnd
