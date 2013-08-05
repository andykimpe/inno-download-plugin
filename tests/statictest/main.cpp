#include <stdio.h>
#include <conio.h>
#include "../../idp/downloader.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Downloader downloader;

	downloader.addFile(_T("http://127.0.0.1/test1.rar"), _T("test1.rar"));
	downloader.addFile(_T("http://127.0.0.1/test2.rar"), _T("test2.rar"));
	downloader.addFile(_T("http://127.0.0.1/test3.rar"), _T("test3.rar"));
	//downloader.addFile(_T("ftp://ftp.delorie.com/pub/djgpp/current/unzip32.exe"), _T("unzip32.exe"));
	
	_tprintf(_T("Total size: %d\n"), (DWORD)downloader.getFileSizes());
	_tprintf(_T("Download %s\n"), downloader.downloadFiles() ? _T("OK") : _T("FAILED"));
	_gettch();
	
	return 0;
}