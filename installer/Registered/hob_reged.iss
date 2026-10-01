; -- hob_reged.iss --
; Installer-Script for registered Version of Bubblet v1.0
; Copyright (c) 1999 House of Bytes Software


; Same as Sample1.iss, but creates its icon in the Programs folder of the
; Start Menu instead of in a subfolder, and also creates a desktop icon.

[Setup]
Bits=32
AppName=Bubblet! v1.0 - Registered
AppVerName=Bubblet! v1.0 - Registered
AppCopyright=Copyright (c) 1999 House of Bytes Software
DefaultDirName={pf}\House of Bytes\Bubblet
DefaultGroupName=Bubblet
MinVersion=4,4

;Modification for multiple disks!
DiskSpanning= 1

[Files]
Source: "BUBBLET.EXE"; DestDir: "{app}"
Source: "BUBBLET.CFG"; DestDir: "{app}"
Source: "CW3215.DLL";  DestDir: "{app}"
Source: "WORKLOG.TXT"; DestDir: "{app}"
Source: "CLICK.WAV";   DestDir: "{app}"
Source: "HIGSCORE.WAV";DestDir: "{app}"
Source: "INTRO.WAV";   DestDir: "{app}"
Source: "MEGAPOP.WAV"; DestDir: "{app}"
Source: "POP.WAV";     DestDir: "{app}"
Source: "SCORE.WAV";   DestDir: "{app}"
Source: "SHAREWAR.WAV";DestDir: "{app}"
Source: "README.TXT";  DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{userdesktop}\Bubblet v1.0 - Registered"; Filename: "{app}\BUBBLET.EXE"; WorkingDir: "{app}"; Comment: "Bubblet v1.0 - Registered"
Name: "{group}\Bubblet v1.0 Registered"; Filename: "{app}\BUBBLET.EXE"; WorkingDir: "{app}"; Comment: "Bubblet v1.0 - Registered"
Name: "{group}\Readme"; Filename: "{app}\README.TXT"; WorkingDir: "{app}"; Comment: "Readme"

; Registry isn't used by Bubblet!
;[Registry]
;Root: HKCU; Subkey: "Software\My Company"; Flags: uninsdeletekeyifempty
;Root: HKCU; Subkey: "Software\My Company\My Program"; Flags: uninsdeletekey
;Root: HKLM; Subkey: "Software\My Company"; Flags: uninsdeletekeyifempty
;Root: HKLM; Subkey: "Software\My Company\My Program"; Flags: uninsdeletekey
;Root: HKLM; Subkey: "Software\My Company\My Program\Settings"; ValueType: string; ValueName: "Path"; ValueData: "{app}"
