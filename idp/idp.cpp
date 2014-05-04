#include "idp.h"

HINSTANCE idpDllHandle = NULL;

Downloader      downloader;
Ui		        ui;
InternetOptions internetOptions;

void idpAddFile(_TCHAR *url, _TCHAR *filename)
{
	downloader.addFile(url, filename);
}

void idpAddFileSize(_TCHAR *url, _TCHAR *filename, DWORDLONG filesize)
{
	downloader.addFile(url, filename, filesize);
}

void idpAddMirror(_TCHAR *url, _TCHAR *mirror)
{
	downloader.addMirror(url, mirror);
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

bool idpFileDownloaded(_TCHAR *url)
{
	return downloader.fileDownloaded(url);
}

bool idpGetFileSize(_TCHAR *url, DWORDLONG *size)
{
	Downloader d;
	d.setInternetOptions(internetOptions);
	d.setMirrorList(&downloader);
	d.addFile(url, _T(""));
	*size = d.getFileSizes();

	return *size != FILE_SIZE_UNKNOWN;
}

bool idpGetFilesSize(DWORDLONG *size)
{
	downloader.setInternetOptions(internetOptions);
	*size = downloader.getFileSizes();
	return *size != FILE_SIZE_UNKNOWN;
}

bool idpDownloadFile(_TCHAR *url, _TCHAR *filename)
{
	Downloader d;
	d.setInternetOptions(internetOptions);
	d.setMirrorList(&downloader);
	d.addFile(url, filename);
	return d.downloadFiles();
}

bool idpDownloadFiles()
{
	downloader.setUi(NULL);
	downloader.setInternetOptions(internetOptions);
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
	downloader.setUi(&ui);
	downloader.setInternetOptions(internetOptions);
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

	if(res || (ui.errorDlgMode == DLG_NONE))
		ui.clickNextButton(); // go to next page
	else if(ui.errorDlgMode == DLG_SIMPLE)
	{
		if(ui.messageBox(ui.msg("Download failed") + _T(": ") + downloader.getLastErrorStr() + _T("\r\n") + (ui.allowContinue ?
		                 ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Next' to continue installing anyway.") :
					     ui.msg("Check your connection and click 'Retry' to try downloading the files again, or click 'Cancel' to terminate setup.")),
						 ui.msg("Download failed"), MB_ICONWARNING | (ui.hasRetryButton ? MB_OK : MB_RETRYCANCEL)) == IDRETRY)
			idpStartDownload();
	}
	else
	{
		ui.dllHandle = idpDllHandle;

		switch(ui.errorDialog(&downloader))
		{
		case IDRETRY : idpStartDownload();   break;
		case IDIGNORE: ui.clickNextButton(); break;
		}
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

DWORD timeoutVal(_TCHAR *value)
{
	string val = toansi(tstrlower(value));

	if(val.compare("infinite") == 0) return TIMEOUT_INFINITE;
	if(val.compare("infinity") == 0) return TIMEOUT_INFINITE;
	if(val.compare("inf")      == 0) return TIMEOUT_INFINITE;

	return _ttoi(value);
}

bool boolVal(_TCHAR *value)
{
	string val = toansi(tstrlower(value));

	if(val.compare("true")  == 0) return true;
	if(val.compare("yes")   == 0) return true;
	if(val.compare("y")     == 0) return true;
	if(val.compare("false") == 0) return false;
	if(val.compare("no")    == 0) return false;
	if(val.compare("n")     == 0) return false;

	return _ttoi(value) > 0;
}

int dlgVal(_TCHAR *value)
{
	string val = toansi(tstrlower(value));

	if(val.compare("none")     == 0) return DLG_NONE;
	if(val.compare("simple")   == 0) return DLG_SIMPLE;
	if(val.compare("filelist") == 0) return DLG_FILELIST;
	if(val.compare("urllist")  == 0) return DLG_URLLIST;

	return boolVal(value) ? DLG_NONE : DLG_SIMPLE;
}

int invCertVal(_TCHAR *value)
{
	string val = toansi(tstrlower(value));

	if(val.compare("showdlg") == 0) return INVC_SHOWDLG;
	if(val.compare("stop")    == 0) return INVC_STOP;
	if(val.compare("ignore")  == 0) return INVC_IGNORE;

	return INVC_SHOWDLG;
}

void idpSetInternalOption(_TCHAR *name, _TCHAR *value)
{
	string key = toansi(tstrlower(name));

	if(key.compare("allowcontinue") == 0)
	{
		ui.allowContinue       = boolVal(value);
		downloader.stopOnError = !ui.allowContinue;
	}
	else if(key.compare("stoponerror")      == 0) downloader.stopOnError         = boolVal(value);
	else if(key.compare("retrybutton")      == 0) ui.hasRetryButton              = boolVal(value);
	else if(key.compare("redrawbackground") == 0) ui.redrawBackground            = boolVal(value);
	else if(key.compare("errordialog")      == 0) ui.errorDlgMode                = dlgVal(value);
	else if(key.compare("useragent")        == 0) internetOptions.userAgent      = value;
	else if(key.compare("referer")          == 0) internetOptions.referer        = value;
	else if(key.compare("invalidcert")      == 0) internetOptions.invalidCert    = invCertVal(value);
	else if(key.compare("connecttimeout")   == 0) internetOptions.connectTimeout = timeoutVal(value);
	else if(key.compare("sendtimeout")      == 0) internetOptions.sendTimeout    = timeoutVal(value);
	else if(key.compare("receivetimeout")   == 0) internetOptions.receiveTimeout = timeoutVal(value);
}

void idpSetDetailedMode(bool mode)
{
	ui.setDetailedMode(mode);
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	idpDllHandle = hinstDLL;
	return TRUE;
}
