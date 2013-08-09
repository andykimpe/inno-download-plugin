#include <process.h>
#include "idp.h"
#include "downloader.h"

Downloader downloader;
uintptr_t  downloadThread;
UI		   ui;

void idpAddFile(_TCHAR *url, _TCHAR *filename)
{
	downloader.addFile(url, filename);
}

void idpAddFileSize(_TCHAR *url, _TCHAR *filename, int filesize)
{
	downloader.addFile(url, filename, filesize);
}

void idpClearFiles()
{
	downloader.clearFiles();
}

DWORDLONG idpGetFileSize(_TCHAR *url)
{
	Downloader d;
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
	d.addFile(url, filename);
	return d.downloadFiles();
}

bool idpDownloadFiles()
{
	downloader.setUI(NULL);
	return downloader.downloadFiles();
}

void idpConnectControl(_TCHAR *name, HWND handle)
{
	ui.connectControl(name, handle);
}

void idpStartDownload()
{
	downloadThread = _beginthread(downloadFiles, 0, NULL);
}

void downloadFiles(void *param)
{
	ui.lockButtons();
	downloader.setUI(&ui);

	if(downloader.downloadFiles())
		ui.clickNextButton(); // go to next page
	else
	{
		ui.unlockButtons(); // allow user to click Retry or Next
		ui.messageBox(downloader.getLastErrorStr(), _T("Error"), MB_OK | MB_ICONERROR);
	}
}