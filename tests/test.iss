[Setup]
AppName=My Program
AppVersion=1.5
DefaultDirName={pf}\My Program
DefaultGroupName=My Program
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir=userdocs:Inno Setup Examples Output

[Files]
Source: "..\release ansi\statictest.exe"; DestDir: "{app}"
#ifdef UNICODE
Source: "..\release\idp.dll"; Flags: dontcopy;
#else
Source: "..\release ansi\idp.dll"; Flags: dontcopy;
#endif

[Icons]
Name: "{group}\My Program"; Filename: "{app}\statictest.exe"

[Code]
procedure idpAddFile(url: String; filename: String);     external 'idpAddFile@files:idp.dll cdecl';
procedure idpStartDownload;                              external 'idpStartDownload@files:idp.dll cdecl';
procedure idpConnectControl(name: String; handle: HWND); external 'idpConnectControl@files:idp.dll cdecl';

var
  NewProgressBar1: TNewProgressBar;
  NewProgressBar2: TNewProgressBar;
  Label1: TNewStaticText;


procedure CustomForm_Activate(Page: TWizardPage);
begin
    idpAddFile('http://127.0.0.1/test1.rar', ExpandConstant('{tmp}\test1.rar'));
    idpAddFile('http://127.0.0.1/test2.rar', ExpandConstant('{tmp}\test2.rar'));
    idpAddFile('http://127.0.0.1/test3.rar', ExpandConstant('{tmp}\test3.rar'));

    idpConnectControl('progressBarTotal', NewProgressBar1.handle);
    idpConnectControl('progressBarFile',  NewProgressBar2.handle);
    idpConnectControl('fileNameLabel',    Label1.handle);

    idpStartDownload();
end;

function CustomForm_ShouldSkipPage(Page: TWizardPage): Boolean;
begin
    Result := False;
end;

function CustomForm_BackButtonClick(Page: TWizardPage): Boolean;
begin
    Result := True;
end;

function CustomForm_NextButtonClick(Page: TWizardPage): Boolean;
begin
    Result := True;
end;

procedure CustomForm_CancelButtonClick(Page: TWizardPage; var Cancel, Confirm: Boolean);
begin
    // enter code here...
end;

function CreateCustomForm(PreviousPageId: Integer): Integer;
var Page: TWizardPage;
begin
  Page := CreateCustomPage(PreviousPageId, 'Downloading', 'Downloading files');

  NewProgressBar1 := TNewProgressBar.Create(Page);
  with NewProgressBar1 do
  begin
    Parent := Page.Surface;
    Left := ScaleX(8);
    Top := ScaleY(24);
    Width := ScaleX(394);
    Height := ScaleY(20);
    Min := 0;
    Max := 100;
  end;
  
  NewProgressBar2 := TNewProgressBar.Create(Page);
  with NewProgressBar2 do
  begin
    Parent := Page.Surface;
    Left := ScaleX(8);
    Top := ScaleY(52);
    Width := ScaleX(394);
    Height := ScaleY(20);
    Min := 0;
    Max := 100;
  end;
	with Page do
	begin
		OnActivate := @CustomForm_Activate;
		OnShouldSkipPage := @CustomForm_ShouldSkipPage;
		OnBackButtonClick := @CustomForm_BackButtonClick;
		OnNextButtonClick := @CustomForm_NextButtonClick;
		OnCancelButtonClick := @CustomForm_CancelButtonClick;
	end;

  Label1 := TNewStaticText.Create(Page);
  with Label1 do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(8);
    Top := ScaleY(80);
    Width := ScaleX(31);
    Height := ScaleY(13);
  end;

  Result := Page.ID;
end;

procedure InitializeWizard();
begin
    CreateCustomForm(wpWelcome);
end;

