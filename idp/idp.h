#pragma once

#include <tchar.h>
#include <windows.h>

extern "C"
{
void	  idpAddFile(_TCHAR *url, _TCHAR *filename);
void	  idpAddFileSize(_TCHAR *url, _TCHAR *filename, DWORDLONG size);
void	  idpClearFiles();
int		  idpFilesCount();
bool	  idpFilesDownloaded();
DWORDLONG idpGetFileSize(_TCHAR *url);
DWORDLONG idpGetFilesSize();
bool	  idpDownloadFile(_TCHAR *url, _TCHAR *filename);
bool	  idpDownloadFiles();

void	  idpConnectControl(_TCHAR *name, HWND handle);
void	  idpStartDownload();
}

void downloadFiles(void *param);