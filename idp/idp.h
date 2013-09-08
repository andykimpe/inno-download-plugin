#pragma once

#include <tchar.h>
#include <windows.h>

extern "C"
{
void idpAddFile(_TCHAR *url, _TCHAR *filename);
void idpAddFileSize(_TCHAR *url, _TCHAR *filename, DWORDLONG size);
void idpAddFileSize32(_TCHAR *url, _TCHAR *filename, DWORD size);
void idpClearFiles();
int  idpFilesCount();
bool idpFilesDownloaded();
bool idpGetFileSize(_TCHAR *url, DWORDLONG *size);
bool idpGetFilesSize(DWORDLONG *size);
bool idpGetFileSize32(_TCHAR *url, DWORD *size);
bool idpGetFilesSize32(DWORD *size);
bool idpDownloadFile(_TCHAR *url, _TCHAR *filename);
bool idpDownloadFiles();

void idpConnectControl(_TCHAR *name, HWND handle);
void idpAddMessage(_TCHAR *name, _TCHAR *message);
void idpSetInternalOption(_TCHAR *name, _TCHAR *value);
void idpStartDownload();
void idpStopDownload();
}

void downloadFiles(void *param);