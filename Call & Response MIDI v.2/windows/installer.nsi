; ==============================================================================
; Call & Response MIDI v2.0.0 - NSIS Windows Installer Script
; Viral Samples
; ==============================================================================
!include "MUI2.nsh"
!include "x64.nsh"

Target x86-ansi
SetCompressor /SOLID lzma

Name "Call & Response MIDI v2.0.0"
OutFile "..\Call & Response MIDI v2.0.0 Windows Setup.exe"
InstallDir "$PROGRAMFILES64\Viral Samples\Call & Response MIDI"
InstallDirRegKey HKLM "Software\Viral Samples\Call & Response MIDI" "Install_Dir"
RequestExecutionLevel admin

; UI settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section "VST3 Plugin (64-bit)" SecVST3
    SectionIn 1 RO
    ${If} ${RunningX64}
        SetOutPath "$COMMONFILES64\VST3\Call & Response MIDI.vst3\Contents\x86_64-win"
        File "..\build-win\CallAndResponseMIDI_artefacts\Release\VST3\Call & Response MIDI.vst3\Contents\x86_64-win\Call & Response MIDI.vst3"
        DetailPrint "Installing 64-bit VST3 plugin..."
    ${Else}
        MessageBox MB_OK|MB_ICONSTOP "Call & Response MIDI requires 64-bit Windows."
        Abort
    ${EndIf}
SectionEnd

Section "Standalone Application" SecStandalone
    SectionIn 1
    SetOutPath "$INSTDIR"
    File "..\build-win\CallAndResponseMIDI_artefacts\Release\Standalone\Call & Response MIDI.exe"
    
    CreateDirectory "$SMPROGRAMS\Viral Samples\Call & Response MIDI"
    CreateShortCut "$SMPROGRAMS\Viral Samples\Call & Response MIDI\Call & Response MIDI.lnk" "$INSTDIR\Call & Response MIDI.exe"
    CreateShortCut "$DESKTOP\Call & Response MIDI.lnk" "$INSTDIR\Call & Response MIDI.exe"
SectionEnd

Section "User Manual & Documentation" SecDocs
    SectionIn 1
    SetOutPath "$INSTDIR\Documentation"
    File "..\User Manual.pdf"
    File "..\Read Me.pdf"
    File "..\Read Me.txt"
    File "..\Call_and_Response_MIDI_User_Manual.html"
    File "..\Read_Me.html"
    File "..\Call_and_Response_MIDI_User_Manual.md"
    File "..\Read Me.md"
    
    CreateShortCut "$SMPROGRAMS\Viral Samples\Call & Response MIDI\User Manual.lnk" "$INSTDIR\Documentation\User Manual.pdf"
    CreateShortCut "$SMPROGRAMS\Viral Samples\Call & Response MIDI\Read Me.lnk" "$INSTDIR\Documentation\Read Me.pdf"
SectionEnd

Section -Post
    WriteUninstaller "$INSTDIR\uninstall.exe"
    CreateShortCut "$SMPROGRAMS\Viral Samples\Call & Response MIDI\Uninstall Call & Response MIDI.lnk" "$INSTDIR\uninstall.exe"
    
    WriteRegStr HKLM "Software\Viral Samples\Call & Response MIDI" "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI" "DisplayName" "Call & Response MIDI v2.0.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI" "DisplayIcon" '"$INSTDIR\Call & Response MIDI.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI" "Publisher" "Viral Samples"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI" "DisplayVersion" "2.0.0"
SectionEnd

Section "Uninstall"
    Delete "$COMMONFILES64\VST3\Call & Response MIDI.vst3\Contents\x86_64-win\*.*"
    RMDir "$COMMONFILES64\VST3\Call & Response MIDI.vst3\Contents\x86_64-win"
    RMDir "$COMMONFILES64\VST3\Call & Response MIDI.vst3\Contents"
    RMDir "$COMMONFILES64\VST3\Call & Response MIDI.vst3"
    
    Delete "$INSTDIR\*.*"
    Delete "$INSTDIR\Documentation\*.*"
    RMDir "$INSTDIR\Documentation"
    RMDir "$INSTDIR"
    
    Delete "$SMPROGRAMS\Viral Samples\Call & Response MIDI\*.*"
    RMDir "$SMPROGRAMS\Viral Samples\Call & Response MIDI"
    Delete "$DESKTOP\Call & Response MIDI.lnk"
    
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CallAndResponseMIDI"
    DeleteRegKey HKLM "Software\Viral Samples\Call & Response MIDI"
SectionEnd
