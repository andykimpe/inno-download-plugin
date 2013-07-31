#pragma once

#include <tchar.h>
#include <windows.h>

extern "C"
{
void	  idpAddFile(_TCHAR *url, _TCHAR *filename);
void	  idpAddFileSize(_TCHAR *url, _TCHAR *filename, int size);
void	  idpClearFiles();
DWORDLONG idpGetFileSize(_TCHAR *url);
DWORDLONG idpGetFilesSize();
bool	  idpDownloadFile(_TCHAR *url, _TCHAR *filename);

void	  idpStartDownload();
}

void downloadFiles(void *param);