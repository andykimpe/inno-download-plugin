#include "idp.h"

Downloader      downloader;
UI		        ui;
SecurityOptions securityOptions;
tstring         userAgent = IDP_USER_AGENT;

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

bool idpGetFileSize(_TCHAR *url, DWORDLONG *size)
{
	Downloader d;
	d.setUserAgent(userAgent);
	d.setSecurityOptions(securityOptions);
	d.addFile(url, _T(""));
	*size = d.getFileSizes();

	return *size != FILE_SIZE_UNKNOWN;
}

bool idpGetFilesSize(DWORDLONG *size)
{
	*size = downloader.getFileSizes();
	return *size != FILE_SIZE_UNKNOWN;
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
	if(name)
		ui.connectControl(name, handle);
}

void idpAddMessage(_TCHAR *name, _TCHAR *message)
{
	if(name)
		ui.addMessage(name, message ? message : _T(""));
}

void idpStartDownload()
{
	ui.lockButtons();
	downloader.setUI(&ui);
	downloader.setUserAgent(userAgent);
	downloader.setSecurityOptions(securityOptions);
	downloader.setFinishedCallback(&downloadFinished);
	downloader.startDownload();
}

void idpStopDownload()
{
	downloader.stopDownload();
	ui.unlockButtons();
	ui.setStatus(ui.msg("Download cancelled"));
}

void downloadFinished(Downloader *d, bool res)
{
	ui.unlockButtons(); // allow user to click Retry or Next

	if(res)
		ui.clickNextButton(); // go to next page
	else
	{	
		if(ui.hasRetryButton)
			ui.messageBox(ui.msg("Download failed") + _T(": ") + downloader.getLastErrorStr() + _T("\r\n") + (ui.allowContinue ?
			              ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Next' to continue installing anyway.") :
						  ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Cancel' to terminate setup.")),
						  ui.msg("Download failed"), MB_OK | MB_ICONWARNING);
		else
			if(ui.messageBox(ui.msg("Download failed") + _T(": ") + downloader.getLastErrorStr() + _T("\r\n") + (ui.allowContinue ?
			                 ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Next' to continue installing anyway.") :
						     ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Cancel' to terminate setup.")),
						     ui.msg("Download failed"), MB_OK | MB_ICONWARNING | MB_RETRYCANCEL) == IDRETRY)
				idpStartDownload();
	}
}

// ANSI Inno Setup don't support 64-bit integers.

void idpAddFileSize32(_TCHAR *url, _TCHAR *filename, DWORD filesize)
{
	idpAddFileSize(url, filename, filesize);
}

bool idpGetFileSize32(_TCHAR *url, DWORD *size)
{
	DWORDLONG size64;
	bool r = idpGetFileSize(url, &size64);
	*size = (DWORD)size64;
	return r;
}

bool idpGetFilesSize32(DWORD *size)
{
	DWORDLONG size64;
	bool r = idpGetFilesSize(&size64);
	*size = (DWORD)size64;
	return r;
}

void idpSetInternalOption(_TCHAR *name, _TCHAR *value)
{
	string key = toansi(_tcslwr(name));

	if     (key.compare("allowcontinue") == 0) ui.allowContinue  = (_ttoi(value) > 0);
	else if(key.compare("retrybutton")   == 0) ui.hasRetryButton = (_ttoi(value) > 0);
	else if(key.compare("useragent")     == 0) userAgent = value;
	else if(key.compare("invalidcert")   == 0)
	{
		string val = toansi(_tcslwr(value));

		if     (val.compare("showdlg") == 0) securityOptions.invalidCert = INVC_SHOWDLG;
		else if(val.compare("stop")    == 0) securityOptions.invalidCert = INVC_STOP;
		else if(val.compare("ignore")  == 0) securityOptions.invalidCert = INVC_IGNORE;
	}
}

void idpSetDetailedMode(bool mode)
{
	ui.setDetailedMode(mode);
}
