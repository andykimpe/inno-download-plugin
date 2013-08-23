#pragma once

#include <tchar.h>
#include <windows.h>

extern "C"
{
void	  idpAddFile(_TCHAR *url, _TCHAR *filename);
void	  idpAddFileSize(_TCHAR *url, _TCHAR *filename, DWORDLONG size);
void	  idpAddFileSize32(_TCHAR *url, _TCHAR *filename, DWORD size);
void	  idpClearFiles();
int		  idpFilesCount();
bool	  idpFilesDownloaded();
DWORDLONG idpGetFileSize(_TCHAR *url);
DWORDLONG idpGetFilesSize();
DWORD     idpGetFileSize32(_TCHAR *url);
DWORD     idpGetFilesSize32();
bool	  idpDownloadFile(_TCHAR *url, _TCHAR *filename);
bool	  idpDownloadFiles();

void	  idpConnectControl(_TCHAR *name, HWND handle);
void      idpAddMessage(_TCHAR *name, _TCHAR *message);
void      idpSetInternalOption(_TCHAR *name, _TCHAR *value);
void	  idpStartDownload();
}

void downloadFiles(void *param);