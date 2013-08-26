#include <process.h>
#include "idp.h"
#include "downloader.h"

Downloader      downloader;
uintptr_t       downloadThread;
UI		        ui;
SecurityOptions securityOptions;
tstring         userAgent = _T("Inno Download Plugin/1.0");

void idpAddFile(_TCHAR *url, _TCHAR *filename)
{
	downloader.addFile(url, filename);
}

void idpAddFileSize(_TCHAR *url, _TCHAR *filename, DWORDLONG filesize)
{
	downloader.addFile(url, filename, filesize);
}

void idpClearFiles()
{
	downloader.clearFiles();
}

int	idpFilesCount()
{
	return downloader.filesCount();
}

bool idpFilesDownloaded()
{
	return downloader.filesDownloaded();
}

DWORDLONG idpGetFileSize(_TCHAR *url)
{
	Downloader d;
	d.setUserAgent(userAgent);
	d.setSecurityOptions(securityOptions);
	d.addFile(url, _T(""));
	return d.getFileSizes();
}

DWORDLONG idpGetFilesSize()
{
	return downloader.getFileSizes();
}

bool idpDownloadFile(_TCHAR *url, _TCHAR *filename)
{
	Downloader d;
	d.setUserAgent(userAgent);
	d.setSecurityOptions(securityOptions);
	d.addFile(url, filename);
	return d.downloadFiles();
}

bool idpDownloadFiles()
{
	downloader.setUI(NULL);
	downloader.setUserAgent(userAgent);
	downloader.setSecurityOptions(securityOptions);
	return downloader.downloadFiles();
}

void idpConnectControl(_TCHAR *name, HWND handle)
{
	ui.connectControl(name, handle);
}

void idpAddMessage(_TCHAR *name, _TCHAR *message)
{
	ui.addMessage(name, message);
}

void idpStartDownload()
{
	downloadThread = _beginthread(downloadFiles, 0, NULL);
}

void downloadFiles(void *param)
{
	ui.lockButtons();
	downloader.setUI(&ui);
	downloader.setUserAgent(userAgent);
	downloader.setSecurityOptions(securityOptions);

	if(downloader.downloadFiles())
	{
		ui.unlockButtons();
		ui.clickNextButton(); // go to next page
	}
	else
	{
		ui.unlockButtons(); // allow user to click Retry or Next
		ui.messageBox(downloader.getLastErrorStr(), ui.messages["Error"], MB_OK | MB_ICONWARNING);
	}
}

// ANSI Inno Setup don't support 64-bit integers.

void idpAddFileSize32(_TCHAR *url, _TCHAR *filename, DWORD filesize)
{
	idpAddFileSize(url, filename, filesize);
}

DWORD idpGetFileSize32(_TCHAR *url)
{
	return (DWORD)idpGetFileSize(url);
}

DWORD idpGetFilesSize32()
{
	return (DWORD)idpGetFilesSize();
}

void idpSetInternalOption(_TCHAR *name, _TCHAR *value)
{
	string key = toansi(_tcslwr(name));

	if     (key.compare("allowcontinue")     == 0) ui.allowContinue = (_ttoi(value) > 0);
	else if(key.compare("useragent")         == 0) userAgent = value;
	else if(key.compare("invalidcertaction") == 0)
	{
		string val = toansi(_tcslwr(value));

		if     (val.compare("showdlg") == 0) securityOptions.invalidCertAction = INVC_SHOWDLG;
		else if(val.compare("stop")    == 0) securityOptions.invalidCertAction = INVC_STOP;
		else if(val.compare("ignore")  == 0) securityOptions.invalidCertAction = INVC_IGNORE;
	}
}
