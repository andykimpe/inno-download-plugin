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
	downloader.setUI(&ui);
	downloader.downloadFiles();
}