#include <stdio.h>
#include <conio.h>
#include "../../idp/downloader.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Downloader downloader;

	downloader.addFile(_T("https://websitewithssl.com/test1.rar"), _T("test1.rar"));
	downloader.addFile(_T("https://websitewithssl.com/test2.rar"), _T("test2.rar"));
	downloader.addFile(_T("https://websitewithssl.com/test3.rar"), _T("test3.rar"));
	
	_tprintf(_T("Total size: %d\n"), (DWORD)downloader.getFileSizes());
	bool result = downloader.downloadFiles();
	_tprintf(_T("Download %s\n"), result ? _T("OK") : _T("FAILED"));
	if(!result)
		_tprintf(_T("Error code: %u, error description: %s\n"), downloader.getLastError(), downloader.getLastErrorStr().c_str());
	_gettch();
	
	return 0;
}