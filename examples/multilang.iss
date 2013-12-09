; Uncomment one of following lines, if you haven't checked "Add IDP include path to ISPPBuiltins.iss" option
;#pragma include __INCLUDE__ + ";" + ReadReg(HKLM, "Software\Mitrich Software\Inno Download Plugin", "InstallDir")
;#pragma include __INCLUDE__ + ";" + "c:\lib\InnoDownloadPlugin"

[Setup]
AppName          = My Program
AppVersion       = 1.0
DefaultDirName   = {pf}\My Program
DefaultGroupName = My Program
OutputDir        = userdocs:Inno Setup Examples Output

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"

#include <idp.iss>
; Language file must be included AFTER idp.iss
#include <idplang\russian.iss>
#include <idplang\german.iss>

; Let's change some of standard strings:
[CustomMessages]
en.IDP_FormCaption = Downloading lot of files...

[Icons]
Name: "{group}\{cm:UninstallProgram,My Program}"; Filename: "{uninstallexe}"

[Code]
procedure InitializeWizard();
begin
  idpAddFile('http://127.0.0.1/test1.zip', ExpandConstant('{tmp}\test1.zip'));
  idpAddFile('http://127.0.0.1/test2.zip', ExpandConstant('{tmp}\test2.zip'));
  idpAddFile('http://127.0.0.1/test3.zip', ExpandConstant('{tmp}\test3.zip'));

  idpDownloadAfter(wpReady);
end;
