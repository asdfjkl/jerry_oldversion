; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{36B3AE1D-DBD1-4B2E-BC2C-629F5FC30B21}
AppName=Jerry - Das Schachprogramm
AppVersion=3.2.1
;AppVerName=Jerry - Chess GUI 3.1.0
AppPublisher=dkl
AppPublisherURL=http://jerry-chess.de
AppSupportURL=http://jerry-chess.de
AppUpdatesURL=http://jerry-chess.de
DefaultDirName={pf}\Jerry_Chess
DisableProgramGroupPage=yes
LicenseFile=C:\Users\user\MyFiles\workspace\jerry\LICENSE.TXT
OutputBaseFilename=SetupJerryChess
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
;Source: "C:\Users\user\MyFiles\workspace\build-jerry3-Desktop_Qt_5_9_1_MinGW_32bit-Release\release\Jerry.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\user\MyFiles\workspace\build-jerry3-Desktop_Qt_5_12_4_MinGW_32_bit-Release\release\Jerry.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Deployment\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\Jerry - Das Schachprogramm"; Filename: "{app}\Jerry.exe"; IconFilename: "{app}\res\icons\win_icon.ico"
Name: "{commondesktop}\Jerry - Das Schachprogramm"; Filename: "{app}\Jerry.exe"; Tasks: desktopicon; IconFilename: "{app}\res\icons\win_icon.ico"

[Run]
Filename: "{app}\Jerry.exe"; Description: "{cm:LaunchProgram,Jerry - Chess GUI}"; Flags: nowait postinstall skipifsilent

