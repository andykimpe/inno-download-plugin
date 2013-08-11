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

[CustomMessages]
DownloadFormCaption=DownloadForm Caption
DownloadFormDescription=DownloadForm Description
DownloadForm_TotalProgressLabel_Caption0=Total progress
DownloadForm_CurrentFileLabel_Caption0=Current file
DownloadForm_TotalDownloaded_Caption0=1000 of 1000 KB
DownloadForm_FileDownloaded_Caption0=1000 of 1000 KB
DownloadForm_FileNameLabel_Caption0=File:
DownloadForm_SpeedLabel_Caption0=Speed:
DownloadForm_StatusLabel_Caption0=Status:
DownloadForm_ElapsedTimeLabel_Caption0=Elapsed time:
DownloadForm_RemainingTimeLabel_Caption0=Remaining time:
DownloadForm_FileName_Caption0=FileName
DownloadForm_Speed_Caption0=Speed
DownloadForm_Status_Caption0=Status
DownloadForm_ElapsedTime_Caption0=ElapsedTime
DownloadForm_RemainingTime_Caption0=RemainingTime
DownloadForm_DetailsButton_Caption0=Hide

[Code]
procedure idpAddFile(url: String; filename: String);     external 'idpAddFile@files:idp.dll cdecl';
function  idpFilesCount: Integer;                        external 'idpFilesCount@files:idp.dll cdecl';
function  idpFilesDownloaded: Boolean;                   external 'idpFilesDownloaded@files:idp.dll cdecl';
procedure idpStartDownload;                              external 'idpStartDownload@files:idp.dll cdecl';
procedure idpConnectControl(name: String; handle: HWND); external 'idpConnectControl@files:idp.dll cdecl';

var
  TotalProgressBar: TNewProgressBar;
  TotalProgressLabel: TNewStaticText;
  CurrentFileLabel: TNewStaticText;
  FileProgressBar: TNewProgressBar;
  TotalDownloaded: TNewStaticText;
  FileDownloaded: TNewStaticText;
  FileNameLabel: TNewStaticText;
  SpeedLabel: TNewStaticText;
  StatusLabel: TNewStaticText;
  ElapsedTimeLabel: TNewStaticText;
  RemainingTimeLabel: TNewStaticText;
  FileName: TNewStaticText;
  Speed: TNewStaticText;
  Status: TNewStaticText;
  ElapsedTime: TNewStaticText;
  RemainingTime: TNewStaticText;
  DetailsButton: TButton;

procedure DownloadFormActivate(Page: TWizardPage);
begin
    idpConnectControl('TotalProgressBar', TotalProgressBar.handle);
    idpConnectControl('FileProgressBar',  FileProgressBar.handle);
    idpConnectControl('FileName',         FileName.handle);
    idpConnectControl('Speed',            Speed.handle);
    idpConnectControl('Status',           Status.handle);
    idpConnectControl('ElapsedTime',      ElapsedTime.handle);
    idpConnectControl('RemainingTime',    RemainingTime.handle);
    idpConnectControl('TotalDownloaded',  TotalDownloaded.handle);
    idpConnectControl('FileDownloaded',   FileDownloaded.handle);
    idpConnectControl('WizardForm',       WizardForm.handle);
    idpConnectControl('BackButton',       WizardForm.BackButton.handle);
    idpConnectControl('NextButton',       WizardForm.NextButton.handle);

    WizardForm.BackButton.Caption := 'Retry';
    idpStartDownload;
end;

function DownloadFormShouldSkipPage(Page: TWizardPage): Boolean;
begin
    Result := (idpFilesCount = 0) or idpFilesDownloaded;
end;

function DownloadFormBackButtonClick(Page: TWizardPage): Boolean; // Retry button
begin
    idpStartDownload; 
    Result := False;
end;

function DownloadFormNextButtonClick(Page: TWizardPage): Boolean;
begin
    Result := True;
end;

procedure DownloadFormCancelButtonClick(Page: TWizardPage; var Cancel, Confirm: Boolean);
begin
    // enter code here...
end;

function CreateDownloadForm(PreviousPageId: Integer): Integer;
var Page: TWizardPage;
begin
    Page := CreateCustomPage(PreviousPageId,
        ExpandConstant('{cm:DownloadFormCaption}'),
        ExpandConstant('{cm:DownloadFormDescription}'));

{ TotalProgressBar }
  TotalProgressBar := TNewProgressBar.Create(Page);
  with TotalProgressBar do
  begin
    Parent := Page.Surface;
    Left := ScaleX(0);
    Top := ScaleY(16);
    Width := ScaleX(410);
    Height := ScaleY(20);
    Min := 0;
    Max := 100;
  end;
  
  { TotalProgressLabel }
  TotalProgressLabel := TNewStaticText.Create(Page);
  with TotalProgressLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_TotalProgressLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(0);
    Width := ScaleX(200);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 1;
  end;
  
  { CurrentFileLabel }
  CurrentFileLabel := TNewStaticText.Create(Page);
  with CurrentFileLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_CurrentFileLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(48);
    Width := ScaleX(200);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 2;
  end;
  
  { FileProgressBar }
  FileProgressBar := TNewProgressBar.Create(Page);
  with FileProgressBar do
  begin
    Parent := Page.Surface;
    Left := ScaleX(0);
    Top := ScaleY(64);
    Width := ScaleX(410);
    Height := ScaleY(20);
    Min := 0;
    Max := 100;
  end;
  
  { TotalDownloaded }
  TotalDownloaded := TNewStaticText.Create(Page);
  with TotalDownloaded do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_TotalDownloaded_Caption0}');
    Left := ScaleX(328);
    Top := ScaleY(0);
    Width := ScaleX(80);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 4;
  end;
  
  { FileDownloaded }
  FileDownloaded := TNewStaticText.Create(Page);
  with FileDownloaded do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_FileDownloaded_Caption0}');
    Left := ScaleX(328);
    Top := ScaleY(48);
    Width := ScaleX(80);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 5;
  end;
  
  { FileNameLabel }
  FileNameLabel := TNewStaticText.Create(Page);
  with FileNameLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_FileNameLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(100);
    Width := ScaleX(116);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 6;
  end;
  
  { SpeedLabel }
  SpeedLabel := TNewStaticText.Create(Page);
  with SpeedLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_SpeedLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(116);
    Width := ScaleX(116);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 7;
  end;
  
  { StatusLabel }
  StatusLabel := TNewStaticText.Create(Page);
  with StatusLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_StatusLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(132);
    Width := ScaleX(116);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 8;
  end;
  
  { ElapsedTimeLabel }
  ElapsedTimeLabel := TNewStaticText.Create(Page);
  with ElapsedTimeLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_ElapsedTimeLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(148);
    Width := ScaleX(116);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 9;
  end;
  
  { RemainingTimeLabel }
  RemainingTimeLabel := TNewStaticText.Create(Page);
  with RemainingTimeLabel do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_RemainingTimeLabel_Caption0}');
    Left := ScaleX(0);
    Top := ScaleY(164);
    Width := ScaleX(116);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 10;
  end;
  
  { FileName }
  FileName := TNewStaticText.Create(Page);
  with FileName do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_FileName_Caption0}');
    Left := ScaleX(120);
    Top := ScaleY(100);
    Width := ScaleX(180);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 11;
  end;
  
  { Speed }
  Speed := TNewStaticText.Create(Page);
  with Speed do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_Speed_Caption0}');
    Left := ScaleX(120);
    Top := ScaleY(116);
    Width := ScaleX(180);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 12;
  end;
  
  { Status }
  Status := TNewStaticText.Create(Page);
  with Status do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_Status_Caption0}');
    Left := ScaleX(120);
    Top := ScaleY(132);
    Width := ScaleX(180);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 13;
  end;
  
  { ElapsedTime }
  ElapsedTime := TNewStaticText.Create(Page);
  with ElapsedTime do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_ElapsedTime_Caption0}');
    Left := ScaleX(120);
    Top := ScaleY(148);
    Width := ScaleX(180);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 14;
  end;
  
  { RemainingTime }
  RemainingTime := TNewStaticText.Create(Page);
  with RemainingTime do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_RemainingTime_Caption0}');
    Left := ScaleX(120);
    Top := ScaleY(164);
    Width := ScaleX(180);
    Height := ScaleY(14);
    AutoSize := False;
    TabOrder := 15;
  end;
  
  { DetailsButton }
  DetailsButton := TButton.Create(Page);
  with DetailsButton do
  begin
    Parent := Page.Surface;
    Caption := ExpandConstant('{cm:DownloadForm_DetailsButton_Caption0}');
    Left := ScaleX(336);
    Top := ScaleY(184);
    Width := ScaleX(75);
    Height := ScaleY(23);
    TabOrder := 16;
  end;
    with Page do
    begin
        OnActivate          := @DownloadFormActivate;
        OnShouldSkipPage    := @DownloadFormShouldSkipPage;
        OnBackButtonClick   := @DownloadFormBackButtonClick;
        OnNextButtonClick   := @DownloadFormNextButtonClick;
        OnCancelButtonClick := @DownloadFormCancelButtonClick;
    end;
  
    Result := Page.ID;
end;

procedure idpDownloadAfter(PageAfterId: Integer);
begin
    CreateDownloadForm(PageAfterId);
end;

procedure InitializeWizard();
begin
    idpAddFile('http://127.0.0.1/test1.rar', ExpandConstant('{tmp}\test1.rar'));
    idpAddFile('http://127.0.0.1/test2.rar', ExpandConstant('{tmp}\test2.rar'));
    idpAddFile('http://127.0.0.1/test3.rar', ExpandConstant('{tmp}\test3.rar'));

    idpDownloadAfter(wpWelcome);
end;
