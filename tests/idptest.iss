#pragma include  __INCLUDE__  + ";" + ReadReg(HKLM, "Software\Mitrich Software\Inno Download Plugin", "InstallDir")

[Setup]
AppName=My Program
AppVersion=1.5
DefaultDirName={pf}\My Program
DefaultGroupName=My Program
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: ru; MessagesFile: compiler:Languages\Russian.isl

#include <idp.iss>
#include <idplang\russian.iss>

[Files]
Source: "idptest.iss"; DestDir: "{app}"

[Icons]
Name: "{group}\{cm:UninstallProgram,My Program}"; Filename: "{uninstallexe}"

[Code]
procedure InitializeWizard();
begin
    idpAddFile('http://127.0.0.1/test1.rar', ExpandConstant('{tmp}\test1.rar'));
    idpAddFile('http://127.0.0.1/test2.rar', ExpandConstant('{tmp}\test2.rar'));
    idpAddFile('http://127.0.0.1/test3.rar', ExpandConstant('{tmp}\test3.rar'));

    idpDownloadAfter(wpWelcome);
end;
