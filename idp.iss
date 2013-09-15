; Inno Download Plugin
; (c)2013 Mitrich Software
; http://mitrich.net23.net/
; https://code.google.com/p/inno-download-plugin/

[CustomMessages]
IDP_FormCaption       =Downloading additional files
IDP_FormDescription   =Please wait, while setup downloading additional files...
IDP_TotalProgress     =Total progress
IDP_CurrentFile       =Current file
IDP_File              =File:
IDP_Speed             =Speed:
IDP_Status            =Status:
IDP_ElapsedTime       =Elapsed time:
IDP_RemainingTime     =Remaining time:
IDP_DetailsButton     =Details
IDP_HideButton        =Hide
IDP_RetryButton       =Retry
IDP_KBs               =KB/s
IDP_BytesDownloaded   =%d of %d KB
IDP_Initializing      =Initializing...
IDP_QueryingFileSizes =Querying file sizes...
IDP_StartingDownload  =Starting download...
IDP_Connecting        =Connecting...
IDP_Downloading       =Downloading...
IDP_Done              =Done
IDP_Error             =Error
IDP_CannotConnect     =Cannot connect
IDP_ExitSetupMessage  =Setup is not complete. If you exit now, the program will not be installed.%n%nYou may run Setup again at another time to complete the installation.%n%nExit Setup?
IDP_CancellingDownload=Cancelling download...
IDP_Unknown           =Unknown
IDP_ActionCancelled   =Action cancelled

#define IDPROOT ExtractFilePath(__PATHFILENAME__)

#ifdef UNICODE
    #pragma include __INCLUDE__ + ";" + IDPROOT + "\unicode"
#else
    #pragma include __INCLUDE__ + ";" + IDPROOT + "\ansi"
#endif         

[Files]
#ifdef UNICODE
Source: "{#IDPROOT}\unicode\idp.dll"; Flags: dontcopy;
#else
Source: "{#IDPROOT}\ansi\idp.dll"; Flags: dontcopy;
#endif

[Code]
procedure idpAddFile(url: String; filename: String);                  external 'idpAddFile@files:idp.dll cdecl';
procedure idpClearFiles;                                              external 'idpClearFiles@files:idp.dll cdecl';
function  idpFilesCount: Integer;                                     external 'idpFilesCount@files:idp.dll cdecl';
function  idpFilesDownloaded: Boolean;                                external 'idpFilesDownloaded@files:idp.dll cdecl';
function  idpDownloadFile(url, filename: String): Boolean;            external 'idpDownloadFile@files:idp.dll cdecl';
function  idpDownloadFiles: Boolean;                                  external 'idpDownloadFiles@files:idp.dll cdecl';
procedure idpStartDownload;                                           external 'idpStartDownload@files:idp.dll cdecl';
procedure idpStopDownload;                                            external 'idpStopDownload@files:idp.dll cdecl';
procedure idpConnectControl(name: String; Handle: HWND);              external 'idpConnectControl@files:idp.dll cdecl';
procedure idpAddMessage(name, message: String);                       external 'idpAddMessage@files:idp.dll cdecl';
procedure idpSetInternalOption(name, value: String);                  external 'idpSetInternalOption@files:idp.dll cdecl';

#ifdef UNICODE
procedure idpAddFileSize(url: String; filename: String; size: Int64); external 'idpAddFileSize@files:idp.dll cdecl';
function  idpGetFileSize(url: String; var size: Int64): Boolean;      external 'idpGetFileSize@files:idp.dll cdecl';
function  idpGetFilesSize(var size: Int64): Boolean;                  external 'idpGetFilesSize@files:idp.dll cdecl';
#else
procedure idpAddFileSize(url: String; filename: String; size: Dword); external 'idpAddFileSize32@files:idp.dll cdecl';
function  idpGetFileSize(url: String; var size: Dword): Boolean;      external 'idpGetFileSize32@files:idp.dll cdecl';
function  idpGetFilesSize(var size: Dword): Boolean;                  external 'idpGetFilesSize32@files:idp.dll cdecl';
#endif

type IDPOptions = record
        DetailedMode   : Boolean;
        NoDetailsButton: Boolean;
        NoRetryButton  : Boolean;
    end;

var TotalProgressBar  : TNewProgressBar;
    FileProgressBar   : TNewProgressBar;
    TotalProgressLabel: TNewStaticText;
    CurrentFileLabel  : TNewStaticText;
    TotalDownloaded   : TNewStaticText;
    FileDownloaded    : TNewStaticText;
    FileNameLabel     : TNewStaticText;
    SpeedLabel        : TNewStaticText;
    StatusLabel       : TNewStaticText;
    ElapsedTimeLabel  : TNewStaticText;
    RemainingTimeLabel: TNewStaticText;
    FileName          : TNewStaticText;
    Speed             : TNewStaticText;
    Status            : TNewStaticText;
    ElapsedTime       : TNewStaticText;
    RemainingTime     : TNewStaticText;
    DetailsButton     : TButton;
    DetailsVisible    : Boolean;
    Options           : IDPOptions;

procedure idpSetOption(name, value: String);
var key: String;
begin
    key := LowerCase(name);

    if      key = 'detailedmode'  then Options.DetailedMode    := StrToInt(value) > 0
    else if key = 'detailsbutton' then Options.NoDetailsButton := StrToInt(value) = 0
    else if key = 'retrybutton'   then 
    begin
        Options.NoRetryButton := StrToInt(value) = 0;
        idpSetInternalOption('RetryButton', value);
    end
    else
        idpSetInternalOption(name, value);
end;

procedure ShowDetails(show: Boolean);
begin
    FileProgressBar.Visible    := show;  
    CurrentFileLabel.Visible   := show;  
    FileDownloaded.Visible     := show;    
    FileNameLabel.Visible      := show;     
    SpeedLabel.Visible         := show;        
    StatusLabel.Visible        := show;       
    ElapsedTimeLabel.Visible   := show;  
    RemainingTimeLabel.Visible := show;
    FileName.Visible           := show;          
    Speed.Visible              := show;             
    Status.Visible             := show;            
    ElapsedTime.Visible        := show;       
    RemainingTime.Visible      := show;
    
    DetailsVisible := show;
    
    if DetailsVisible then
    begin
        DetailsButton.Caption := ExpandConstant('{cm:IDP_HideButton}');
        DetailsButton.Top := ScaleY(184);
    end
    else
    begin
        DetailsButton.Caption := ExpandConstant('{cm:IDP_DetailsButton}');
        DetailsButton.Top := ScaleY(44);
    end;
end;

procedure DetailsButtonClick(Sender: TObject);
begin
    ShowDetails(not DetailsVisible);
end;

procedure DownloadFormActivate(Page: TWizardPage);
begin
    if not Options.NoRetryButton then
        WizardForm.BackButton.Caption := ExpandConstant('{cm:IDP_RetryButton}');
         
    ShowDetails(Options.DetailedMode);
    DetailsButton.Visible := not Options.NoDetailsButton;
    idpStartDownload;
end;

function DownloadFormShouldSkipPage(Page: TWizardPage): Boolean;
begin
    Result := (idpFilesCount = 0) or idpFilesDownloaded;
end;

function DownloadFormBackButtonClick(Page: TWizardPage): Boolean;
begin
    if not Options.NoRetryButton then // Retry button clicked
    begin
        idpStartDownload; 
        Result := False;
    end
    else
        Result := true;
end;

function DownloadFormNextButtonClick(Page: TWizardPage): Boolean;
begin
    Result := True;
end;

procedure DownloadFormCancelButtonClick(Page: TWizardPage; var Cancel, Confirm: Boolean);
begin
    if MsgBox(ExpandConstant('{cm:IDP_ExitSetupMessage}'), mbConfirmation, MB_YESNO) = IDYES then
    begin
        Status.Caption := ExpandConstant('{cm:IDP_CancellingDownload}');
        WizardForm.Repaint;
        idpStopDownload;
        Cancel  := true;
        Confirm := false;
    end
    else
        Cancel := false;
end;

function CreateDownloadForm(PreviousPageId: Integer): Integer;
var Page: TWizardPage;
begin
    Page := CreateCustomPage(PreviousPageId, ExpandConstant('{cm:IDP_FormCaption}'), ExpandConstant('{cm:IDP_FormDescription}'));

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

    TotalProgressLabel := TNewStaticText.Create(Page);
    with TotalProgressLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_TotalProgress}');
        Left := ScaleX(0);
        Top := ScaleY(0);
        Width := ScaleX(200);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 1;
    end;

    CurrentFileLabel := TNewStaticText.Create(Page);
    with CurrentFileLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_CurrentFile}');
        Left := ScaleX(0);
        Top := ScaleY(48);
        Width := ScaleX(200);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 2;
    end;

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

    TotalDownloaded := TNewStaticText.Create(Page);
    with TotalDownloaded do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(328);
        Top := ScaleY(0);
        Width := ScaleX(80);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 4;
    end;

    FileDownloaded := TNewStaticText.Create(Page);
    with FileDownloaded do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(328);
        Top := ScaleY(48);
        Width := ScaleX(80);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 5;
    end;

    FileNameLabel := TNewStaticText.Create(Page);
    with FileNameLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_File}');
        Left := ScaleX(0);
        Top := ScaleY(100);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 6;
    end;

    SpeedLabel := TNewStaticText.Create(Page);
    with SpeedLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_Speed}');
        Left := ScaleX(0);
        Top := ScaleY(116);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 7;
    end;

    StatusLabel := TNewStaticText.Create(Page);
    with StatusLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_Status}');
        Left := ScaleX(0);
        Top := ScaleY(132);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 8;
    end;

    ElapsedTimeLabel := TNewStaticText.Create(Page);
    with ElapsedTimeLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_ElapsedTime}');
        Left := ScaleX(0);
        Top := ScaleY(148);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 9;
    end;

    RemainingTimeLabel := TNewStaticText.Create(Page);
    with RemainingTimeLabel do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_RemainingTime}');
        Left := ScaleX(0);
        Top := ScaleY(164);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 10;
    end;

    FileName := TNewStaticText.Create(Page);
    with FileName do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(100);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 11;
    end;

    Speed := TNewStaticText.Create(Page);
    with Speed do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(116);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 12;
    end;

    Status := TNewStaticText.Create(Page);
    with Status do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(132);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 13;
    end;

    ElapsedTime := TNewStaticText.Create(Page);
    with ElapsedTime do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(148);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 14;
    end;

    RemainingTime := TNewStaticText.Create(Page);
    with RemainingTime do
    begin
        Parent := Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(164);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 15;
    end;

    DetailsButton := TButton.Create(Page);
    with DetailsButton do
    begin
        Parent := Page.Surface;
        Caption := ExpandConstant('{cm:IDP_DetailsButton}');
        Left := ScaleX(336);
        Top := ScaleY(184);
        Width := ScaleX(75);
        Height := ScaleY(23);
        TabOrder := 16;
        OnClick := @DetailsButtonClick;
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

procedure ConnectControls;
begin
    idpConnectControl('TotalProgressBar', TotalProgressBar.Handle);
    idpConnectControl('FileProgressBar',  FileProgressBar.Handle);
    idpConnectControl('TotalDownloaded',  TotalDownloaded.Handle);
    idpConnectControl('FileDownloaded',   FileDownloaded.Handle);
    idpConnectControl('FileName',         FileName.Handle);
    idpConnectControl('Speed',            Speed.Handle);
    idpConnectControl('Status',           Status.Handle);
    idpConnectControl('ElapsedTime',      ElapsedTime.Handle);
    idpConnectControl('RemainingTime',    RemainingTime.Handle);
    idpConnectControl('WizardForm',       WizardForm.Handle);
    idpConnectControl('BackButton',       WizardForm.BackButton.Handle);
    idpConnectControl('NextButton',       WizardForm.NextButton.Handle);
end;

procedure InitMessages;
begin
    idpAddMessage('KB/s',                   ExpandConstant('{cm:IDP_KBs}'));
    idpAddMessage('%d of %d KB',            ExpandConstant('{cm:IDP_BytesDownloaded}'));
    idpAddMessage('Initializing...',        ExpandConstant('{cm:IDP_Initializing}'));
    idpAddMessage('Querying file sizes...', ExpandConstant('{cm:IDP_QueryingFileSizes}'));
    idpAddMessage('Starting download...',   ExpandConstant('{cm:IDP_StartingDownload}'));
    idpAddMessage('Connecting...',          ExpandConstant('{cm:IDP_Connecting}'));
    idpAddMessage('Downloading...',         ExpandConstant('{cm:IDP_Downloading}'));
    idpAddMessage('Done',                   ExpandConstant('{cm:IDP_Done}'));
    idpAddMessage('Error',                  ExpandConstant('{cm:IDP_Error}'));
    idpAddMessage('Cannot connect',         ExpandConstant('{cm:IDP_CannotConnect}'));
    idpAddMessage('Unknown',                ExpandConstant('{cm:IDP_Unknown}'));
    idpAddMessage('Action cancelled',       ExpandConstant('{cm:IDP_ActionCancelled}'));
end;

procedure idpDownloadAfter(PageAfterId: Integer);
begin
    CreateDownloadForm(PageAfterId);
    ConnectControls;
    InitMessages;
end;
