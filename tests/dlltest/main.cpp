#include <stdio.h>
#include <conio.h>
#include "../../idp/idp.h"

int _tmain(int argc, _TCHAR* argv[])
{
	idpAddFile(_T("https://websitewithssl.com/test1.rar"), _T("test1.rar"));
	idpAddFile(_T("https://websitewithssl.com/test2.rar"), _T("test2.rar"));
	idpAddFile(_T("https://websitewithssl.com/test3.rar"), _T("test3.rar"));
	
	_tprintf(_T("Size of files: %d bytes\n"), (DWORD)idpGetFilesSize());

	idpStartDownload();

	_tprintf(_T("Download started\n"));
	_gettch();
	
	return 0;
}