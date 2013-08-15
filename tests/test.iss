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
Name: en; MessagesFile: "compiler:Default.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"

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
en.DownloadFormCaption=Downloading additional files
en.DownloadFormDescription=Please wait, while setup downloading additional files...
en.DownloadFormTotalProgressLabel=Total progress
en.DownloadFormCurrentFileLabel=Current file
en.DownloadFormFileNameLabel=File:
en.DownloadFormSpeedLabel=Speed:
en.DownloadFormStatusLabel=Status:
en.DownloadFormElapsedTimeLabel=Elapsed time:
en.DownloadFormRemainingTimeLabel=Remaining time:
en.DownloadFormDetailsButton=Details
en.DownloadFormHideButton=Hide
en.DownloadFormRetryButton=Retry
en.KBs=KB/s                
en.BytesDownloaded=%d of %d KB         
en.Initializing=Initializing...     
en.QueryingFileSizes=Querying file sizes...
en.StartingDownload=Starting download...
en.Connecting=Connecting...       
en.Downloading=Downloading...      
en.Done=Done                
en.Error=Error               
en.CannotConnect=Cannot connect     

fr.DownloadFormCaption=Téléchargement des fichiers additionnels
fr.DownloadFormDescription=Veuillez patienter durant le téléchargement des fichiers additionnels...
fr.DownloadFormTotalProgressLabel=Progression générale
fr.DownloadFormCurrentFileLabel=Fichier en cours
fr.DownloadFormFileNameLabel=Fichier:
fr.DownloadFormSpeedLabel=Vitesse:
fr.DownloadFormStatusLabel=Status:
fr.DownloadFormElapsedTimeLabel=Temps écoulé:
fr.DownloadFormRemainingTimeLabel=Temps restant:
en.DownloadFormDetailsButton=Details
fr.DownloadFormHideButton=Cacher
fr.DownloadFormRetryButton=Retry
fr.KBs=KB/s                
fr.BytesDownloaded=%d of %d KB         
fr.Initializing=Initializing...     
fr.QueryingFileSizes=Querying file sizes...
fr.StartingDownload=Starting download...
fr.Connecting=Connecting...       
fr.Downloading=Downloading...      
fr.Done=Done                
fr.Error=Error               
fr.CannotConnect=Cannot connect      

ru.DownloadFormCaption=Скачивание дополнительных файлов
ru.DownloadFormDescription=Пожалуйста подождите, пока инсталлятор скачает дополнительные файлы...
ru.DownloadFormTotalProgressLabel=Общий прогресс
ru.DownloadFormCurrentFileLabel=Текущий файл
ru.DownloadFormFileNameLabel=Файл:
ru.DownloadFormSpeedLabel=Скорость:
ru.DownloadFormStatusLabel=Состояние:
ru.DownloadFormElapsedTimeLabel=Прошло времени:
ru.DownloadFormRemainingTimeLabel=Осталось времени:
en.DownloadFormDetailsButton=Подробно
ru.DownloadFormHideButton=Скрыть
ru.DownloadFormRetryButton=Повтор
ru.KBs=КБ/с                
ru.BytesDownloaded=%d из %d КБ         
ru.Initializing=Инициализация...     
ru.QueryingFileSizes=Определение размеров файлов...
ru.StartingDownload=Начало загрузки...
ru.Connecting=Соединение...       
ru.Downloading=Загрузка...      
ru.Done=Готово                
ru.Error=Ошибка               
ru.CannotConnect=Невозможно соединиться      

[Code]
procedure idpAddFile(url: String; filename: String);                  external 'idpAddFile@files:idp.dll cdecl';
procedure idpAddFileSize(url: String; filename: String; size: Int64); external 'idpAddFileSize@files:idp.dll cdecl';
procedure idpClearFiles;                                              external 'idpClearFiles@files:idp.dll cdecl';
function  idpFilesCount: Integer;                                     external 'idpFilesCount@files:idp.dll cdecl';
function  idpFilesDownloaded: Boolean;                                external 'idpFilesDownloaded@files:idp.dll cdecl';
function  idpGetFileSize(url: String): Int64;                         external 'idpGetFileSize@files:idp.dll cdecl';
function  idpGetFilesSize: Int64;                                     external 'idpGetFilesSize@files:idp.dll cdecl';
procedure idpDownloadFile(url: String; filename: String);             external 'idpDownloadFile@files:idp.dll cdecl';
function  idpDownloadFiles: Boolean;                                  external 'idpDownloadFiles@files:idp.dll cdecl';
procedure idpStartDownload;                                           external 'idpStartDownload@files:idp.dll cdecl';
procedure idpConnectControl(name: String; Handle: HWND);              external 'idpConnectControl@files:idp.dll cdecl';
procedure idpAddMessage(name, message: String);                       external 'idpAddMessage@files:idp.dll cdecl';

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
        DetailsButton.Caption := ExpandConstant('{cm:DownloadFormHideButton}');
        DetailsButton.Top := ScaleY(184);
    end
    else
    begin
        DetailsButton.Caption := ExpandConstant('{cm:DownloadFormDetailsButton}');
        DetailsButton.Top := ScaleY(44);
    end;
end;

procedure DetailsButtonClick(Sender: TObject);
begin
    ShowDetails(not DetailsVisible);
end;

procedure DownloadFormActivate(Page: TWizardPage);
begin
    WizardForm.BackButton.Caption := ExpandConstant('{cm:DownloadFormRetryButton}'); 
    ShowDetails(false);
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
    //TODO: Stopping download
end;

function CreateDownloadForm(PreviousPageId: Integer): Integer;
var Page: TWizardPage;
begin
    Page := CreateCustomPage(PreviousPageId,
        ExpandConstant('{cm:DownloadFormCaption}'),
        ExpandConstant('{cm:DownloadFormDescription}'));

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
        Caption := ExpandConstant('{cm:DownloadFormTotalProgressLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormCurrentFileLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormFileNameLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormSpeedLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormStatusLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormElapsedTimeLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormRemainingTimeLabel}');
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
        Caption := ExpandConstant('{cm:DownloadFormDetailsButton}');
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
    idpAddMessage('KB/s',                   ExpandConstant('{cm:KBs}'));
    idpAddMessage('%d of %d KB',            ExpandConstant('{cm:BytesDownloaded}'));
    idpAddMessage('Initializing...',        ExpandConstant('{cm:Initializing}'));
    idpAddMessage('Querying file sizes...', ExpandConstant('{cm:QueryingFileSizes}'));
    idpAddMessage('Starting download...',   ExpandConstant('{cm:StartingDownload}'));
    idpAddMessage('Connecting...',          ExpandConstant('{cm:Connecting}'));
    idpAddMessage('Downloading...',         ExpandConstant('{cm:Downloading}'));
    idpAddMessage('Done',                   ExpandConstant('{cm:Done}'));
    idpAddMessage('Error',                  ExpandConstant('{cm:Error}'));
    idpAddMessage('Cannot connect',         ExpandConstant('{cm:CannotConnect}'));
end;

procedure idpDownloadAfter(PageAfterId: Integer);
begin
    CreateDownloadForm(PageAfterId);
    ConnectControls;
    InitMessages;
end;

procedure InitializeWizard();
begin
    idpAddFile('http://127.0.0.1/test1.rar', ExpandConstant('{tmp}\test1.rar'));
    idpAddFile('http://127.0.0.1/test2.rar', ExpandConstant('{tmp}\test2.rar'));
    idpAddFile('http://127.0.0.1/test3.rar', ExpandConstant('{tmp}\test3.rar'));

    idpDownloadAfter(wpWelcome);
end;
