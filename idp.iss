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

type IDPFormRec = record
        Page              : TWizardPage;
        TotalProgressBar  : TNewProgressBar;
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
    end;

    IDPOptionsRec = record
        DetailedMode   : Boolean;
        NoDetailsButton: Boolean;
        NoRetryButton  : Boolean;
    end;

var IDPForm   : IDPFormRec;
    IDPOptions: IDPOptionsRec;

procedure idpSetOption(name, value: String);
var key: String;
begin
    key := LowerCase(name);

    if      key = 'detailedmode'  then IDPOptions.DetailedMode    := StrToInt(value) > 0
    else if key = 'detailsbutton' then IDPOptions.NoDetailsButton := StrToInt(value) = 0
    else if key = 'retrybutton'   then 
    begin
        IDPOptions.NoRetryButton := StrToInt(value) = 0;
        idpSetInternalOption('RetryButton', value);
    end
    else
        idpSetInternalOption(name, value);
end;

procedure idpShowDetails(show: Boolean);
begin
    IDPForm.FileProgressBar.Visible    := show; 
    IDPForm.CurrentFileLabel.Visible   := show;  
    IDPForm.FileDownloaded.Visible     := show;    
    IDPForm.FileNameLabel.Visible      := show;     
    IDPForm.SpeedLabel.Visible         := show;        
    IDPForm.StatusLabel.Visible        := show;       
    IDPForm.ElapsedTimeLabel.Visible   := show;  
    IDPForm.RemainingTimeLabel.Visible := show;
    IDPForm.FileName.Visible           := show;          
    IDPForm.Speed.Visible              := show;             
    IDPForm.Status.Visible             := show;            
    IDPForm.ElapsedTime.Visible        := show;       
    IDPForm.RemainingTime.Visible      := show;
    
    IDPForm.DetailsVisible := show;
    
    if IDPForm.DetailsVisible then
    begin
        IDPForm.DetailsButton.Caption := ExpandConstant('{cm:IDP_HideButton}');
        IDPForm.DetailsButton.Top := ScaleY(184);
    end
    else
    begin
        IDPForm.DetailsButton.Caption := ExpandConstant('{cm:IDP_DetailsButton}');
        IDPForm.DetailsButton.Top := ScaleY(44);
    end;
end;

procedure idpDetailsButtonClick(Sender: TObject);
begin
    idpShowDetails(not IDPForm.DetailsVisible);
end;

procedure idpFormActivate(Page: TWizardPage);
begin
    if not IDPOptions.NoRetryButton then
        WizardForm.BackButton.Caption := ExpandConstant('{cm:IDP_RetryButton}');
         
    idpShowDetails(IDPOptions.DetailedMode);
    IDPForm.DetailsButton.Visible := not IDPOptions.NoDetailsButton;
    idpStartDownload;
end;

function idpShouldSkipPage(Page: TWizardPage): Boolean;
begin
    Result := (idpFilesCount = 0) or idpFilesDownloaded;
end;

function idpBackButtonClick(Page: TWizardPage): Boolean;
begin
    if not IDPOptions.NoRetryButton then // Retry button clicked
    begin
        idpStartDownload; 
        Result := False;
    end
    else
        Result := true;
end;

function idpNextButtonClick(Page: TWizardPage): Boolean;
begin
    Result := True;
end;

procedure idpCancelButtonClick(Page: TWizardPage; var Cancel, Confirm: Boolean);
begin
    if MsgBox(ExpandConstant('{cm:IDP_ExitSetupMessage}'), mbConfirmation, MB_YESNO) = IDYES then
    begin
        IDPForm.Status.Caption := ExpandConstant('{cm:IDP_CancellingDownload}');
        WizardForm.Repaint;
        idpStopDownload;
        Cancel  := true;
        Confirm := false;
    end
    else
        Cancel := false;
end;

function idpCreateDownloadForm(PreviousPageId: Integer): Integer;
begin
    IDPForm.Page := CreateCustomPage(PreviousPageId, ExpandConstant('{cm:IDP_FormCaption}'), ExpandConstant('{cm:IDP_FormDescription}'));

    IDPForm.TotalProgressBar := TNewProgressBar.Create(IDPForm.Page);
    with IDPForm.TotalProgressBar do
    begin
        Parent := IDPForm.Page.Surface;
        Left := ScaleX(0);
        Top := ScaleY(16);
        Width := ScaleX(410);
        Height := ScaleY(20);
        Min := 0;
        Max := 100;
    end;

    IDPForm.TotalProgressLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.TotalProgressLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_TotalProgress}');
        Left := ScaleX(0);
        Top := ScaleY(0);
        Width := ScaleX(200);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 1;
    end;

    IDPForm.CurrentFileLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.CurrentFileLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_CurrentFile}');
        Left := ScaleX(0);
        Top := ScaleY(48);
        Width := ScaleX(200);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 2;
    end;

    IDPForm.FileProgressBar := TNewProgressBar.Create(IDPForm.Page);
    with IDPForm.FileProgressBar do
    begin
        Parent := IDPForm.Page.Surface;
        Left := ScaleX(0);
        Top := ScaleY(64);
        Width := ScaleX(410);
        Height := ScaleY(20);
        Min := 0;
        Max := 100;
    end;

    IDPForm.TotalDownloaded := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.TotalDownloaded do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(328);
        Top := ScaleY(0);
        Width := ScaleX(80);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 4;
    end;

    IDPForm.FileDownloaded := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.FileDownloaded do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(328);
        Top := ScaleY(48);
        Width := ScaleX(80);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 5;
    end;

    IDPForm.FileNameLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.FileNameLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_File}');
        Left := ScaleX(0);
        Top := ScaleY(100);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 6;
    end;

    IDPForm.SpeedLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.SpeedLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_Speed}');
        Left := ScaleX(0);
        Top := ScaleY(116);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 7;
    end;

    IDPForm.StatusLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.StatusLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_Status}');
        Left := ScaleX(0);
        Top := ScaleY(132);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 8;
    end;

    IDPForm.ElapsedTimeLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.ElapsedTimeLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_ElapsedTime}');
        Left := ScaleX(0);
        Top := ScaleY(148);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 9;
    end;

    IDPForm.RemainingTimeLabel := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.RemainingTimeLabel do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_RemainingTime}');
        Left := ScaleX(0);
        Top := ScaleY(164);
        Width := ScaleX(116);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 10;
    end;

    IDPForm.FileName := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.FileName do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(100);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 11;
    end;

    IDPForm.Speed := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.Speed do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(116);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 12;
    end;

    IDPForm.Status := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.Status do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(132);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 13;
    end;

    IDPForm.ElapsedTime := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.ElapsedTime do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(148);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 14;
    end;

    IDPForm.RemainingTime := TNewStaticText.Create(IDPForm.Page);
    with IDPForm.RemainingTime do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := '';
        Left := ScaleX(120);
        Top := ScaleY(164);
        Width := ScaleX(180);
        Height := ScaleY(14);
        AutoSize := False;
        TabOrder := 15;
    end;

    IDPForm.DetailsButton := TButton.Create(IDPForm.Page);
    with IDPForm.DetailsButton do
    begin
        Parent := IDPForm.Page.Surface;
        Caption := ExpandConstant('{cm:IDP_DetailsButton}');
        Left := ScaleX(336);
        Top := ScaleY(184);
        Width := ScaleX(75);
        Height := ScaleY(23);
        TabOrder := 16;
        OnClick := @idpDetailsButtonClick;
    end;
  
    with IDPForm.Page do
    begin
        OnActivate          := @idpFormActivate;
        OnShouldSkipPage    := @idpShouldSkipPage;
        OnBackButtonClick   := @idpBackButtonClick;
        OnNextButtonClick   := @idpNextButtonClick;
        OnCancelButtonClick := @idpCancelButtonClick;
    end;
  
    Result := IDPForm.Page.ID;
end;

procedure idpConnectControls;
begin
    idpConnectControl('TotalProgressBar', IDPForm.TotalProgressBar.Handle);
    idpConnectControl('FileProgressBar',  IDPForm.FileProgressBar.Handle);
    idpConnectControl('TotalDownloaded',  IDPForm.TotalDownloaded.Handle);
    idpConnectControl('FileDownloaded',   IDPForm.FileDownloaded.Handle);
    idpConnectControl('FileName',         IDPForm.FileName.Handle);
    idpConnectControl('Speed',            IDPForm.Speed.Handle);
    idpConnectControl('Status',           IDPForm.Status.Handle);
    idpConnectControl('ElapsedTime',      IDPForm.ElapsedTime.Handle);
    idpConnectControl('RemainingTime',    IDPForm.RemainingTime.Handle);
    idpConnectControl('WizardForm',       WizardForm.Handle);
    idpConnectControl('BackButton',       WizardForm.BackButton.Handle);
    idpConnectControl('NextButton',       WizardForm.NextButton.Handle);
end;

procedure idpInitMessages;
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
    idpCreateDownloadForm(PageAfterId);
    idpConnectControls;
    idpInitMessages;
end;
