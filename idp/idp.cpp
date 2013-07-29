#include <process.h>
#include "idp.h"
#include "downloader.h"

Downloader downloader;
uintptr_t  downloadThread;

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

bool idpDownloadFile(_TCHAR *url, _TCHAR *filename)
{
	Downloader d;
	d.addFile(url, filename);
	return d.downloadFiles();
}

void idpStartDownload()
{
	downloadThread = _beginthread(downloadFiles, 0, NULL);
}

void downloadFiles(void *param)
{
	downloader.downloadFiles();
}