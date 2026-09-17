; ==============================================================================
; Call & Response MIDI v1.0.5 - Inno Setup Windows Installer Script
; Viral Samples
; ==============================================================================

#define MyAppName "Call & Response MIDI"
#define MyAppVersion "1.0.5"
#define MyAppPublisher "Viral Samples"
#define MyAppURL "https://viralsamples.com"
#define MyAppExeName "Call & Response MIDI.exe"

[Setup]
AppId={{D6494F77-9E40-4228-B08B-93FE139DF4AC}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf64}\Viral Samples\Call & Response MIDI
DefaultGroupName=Viral Samples\{#MyAppName}
DisableProgramGroupPage=yes
OutputBaseFilename=Call & Response MIDI v{#MyAppVersion} Windows Setup
OutputDir=..\..\dist
Compression=lzma2/ultra64
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
PrivilegesRequired=admin

[Types]
Name: "full"; Description: "Full Installation (VST3, Standalone & Documentation)"
Name: "vst3only"; Description: "VST3 Plugin Only"
Name: "custom"; Description: "Custom Installation"; Flags: iscustom

[Components]
Name: "vst3"; Description: "64-bit VST3 Plugin (DAW support: FL Studio, Ableton Live, Cubase, Studio One, Reaper)"; Types: full vst3only custom; Flags: checkable_alone
Name: "standalone"; Description: "Standalone Application (.exe)"; Types: full custom
Name: "docs"; Description: "User Manual & Documentation (PDF, HTML, TXT)"; Types: full custom

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Components: standalone; Flags: unchecked

[Files]
; 64-bit VST3 Plugin bundle into Common Files\VST3
Source: "..\..\build\CallAndResponseMIDI_artefacts\Release\VST3\Call & Response MIDI.vst3\*"; DestDir: "{commoncf64}\VST3\Call & Response MIDI.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: vst3

; Standalone Application
Source: "..\..\build\CallAndResponseMIDI_artefacts\Release\Standalone\Call & Response MIDI.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: standalone

; Documentation
Source: "..\..\User Manual.pdf"; DestDir: "{app}\Documentation"; Flags: ignoreversion; Components: docs
Source: "..\..\Read Me.pdf"; DestDir: "{app}\Documentation"; Flags: ignoreversion; Components: docs
Source: "..\..\Read Me.txt"; DestDir: "{app}\Documentation"; Flags: ignoreversion; Components: docs
Source: "..\..\Documentation\Call_and_Response_MIDI_User_Manual.html"; DestDir: "{app}\Documentation"; Flags: ignoreversion; Components: docs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Components: standalone
Name: "{group}\User Manual (PDF)"; Filename: "{app}\Documentation\User Manual.pdf"; Components: docs
Name: "{group}\Read Me (PDF)"; Filename: "{app}\Documentation\Read Me.pdf"; Components: docs
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; Components: standalone

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Components: standalone
