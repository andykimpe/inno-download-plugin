[Setup]
AppName=My Program
AppVersion=1.5
DefaultDirName={pf}\My Program
DefaultGroupName=My Program
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.

[Files]
Source: "..\debug\statictest.exe"; DestDir: "{app}"
#ifdef UNICODE
Source: "..\debug\idp.dll"; Flags: dontcopy;
#else
Source: "..\debug ansi\idp.dll"; Flags: dontcopy;
#endif

[Icons]
Name: "{group}\My Program"; Filename: "{app}\statictest.exe"

[Code]
procedure idpAddFile(url: String; filename: String);     external 'idpAddFile@files:idp.dll cdecl';
procedure idpStartDownload;                              external 'idpStartDownload@files:idp.dll cdecl';
procedure idpConnectControl(name: String; handle: HWND); external 'idpConnectControl@files:idp.dll cdecl';

var
  TotalProgressBar: TNewProgressBar;
  FileProgressBar: TNewProgressBar;
  FileNameLabel: TNewStaticText;
  SpeedLabel: TNewStaticText;
  ElapsedTimeLabel: TNewStaticText;
  RemainingTimeLabel: TNewStaticText;
  TotalSizeLabel: TNewStaticText;
  FileSizeLabel: TNewStaticText;

procedure CustomForm_Activate(Page: TWizardPage);
begin
    idpAddFile('http://127.0.0.1/test1.rar', ExpandConstant('{tmp}\test1.rar'));
    idpAddFile('http://127.0.0.1/test2.rar', ExpandConstant('{tmp}\test2.rar'));
    idpAddFile('http://127.0.0.1/test3.rar', ExpandConstant('{tmp}\test3.rar'));

    idpConnectControl('progressBarTotal',   TotalProgressBar.handle);
    idpConnectControl('progressBarFile',    FileProgressBar.handle);
    idpConnectControl('fileNameLabel',      FileNameLabel.handle);
    idpConnectControl('speedLabel',         SpeedLabel.handle);
    idpConnectControl('elapsedTimeLabel',   ElapsedTimeLabel.handle);
    idpConnectControl('remainingTimeLabel', RemainingTimeLabel.handle);
    idpConnectControl('sizeLabelTotal',     TotalSizeLabel.handle);
    idpConnectControl('sizeLabelFile',      FileSizeLabel.handle);

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

  TotalProgressBar := TNewProgressBar.Create(Page);
  with TotalProgressBar do
  begin
    Parent := Page.Surface;
    Left := ScaleX(8);
    Top := ScaleY(24);
    Width := ScaleX(394);
    Height := ScaleY(20);
    Min := 0;
    Max := 100;
  end;
  
  FileProgressBar := TNewProgressBar.Create(Page);
  with FileProgressBar do
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

  FileNameLabel := TNewStaticText.Create(Page);
  with FileNameLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(8);
    Top := ScaleY(80);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  SpeedLabel := TNewStaticText.Create(Page);
  with SpeedLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(8);
    Top := ScaleY(100);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  ElapsedTimeLabel := TNewStaticText.Create(Page);
  with ElapsedTimeLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(8);
    Top := ScaleY(120);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  RemainingTimeLabel := TNewStaticText.Create(Page);
  with RemainingTimeLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(8);
    Top := ScaleY(140);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  TotalSizeLabel := TNewStaticText.Create(Page);
  with TotalSizeLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(150);
    Top := ScaleY(80);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  FileSizeLabel := TNewStaticText.Create(Page);
  with FileSizeLabel do
  begin
    Parent := Page.Surface;
    Caption := 'Hello';
    Left := ScaleX(150);
    Top := ScaleY(100);
    Width := ScaleX(100);
    Height := ScaleY(13);
  end;

  Result := Page.ID;
end;

procedure InitializeWizard();
begin
    CreateCustomForm(wpWelcome);
end;

