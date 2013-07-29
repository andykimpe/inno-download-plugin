#include <stdio.h>
#include "../../idp/downloader.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Downloader downloader;

	downloader.addFile(_T("http://127.0.0.1/test1.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\statictest\\test1.rar"));
	downloader.addFile(_T("http://127.0.0.1/test2.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\statictest\\test2.rar"));
	downloader.addFile(_T("http://127.0.0.1/test3.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\statictest\\test3.rar"));
	
	_tprintf(_T("Download %s\n"), downloader.downloadFiles() ? _T("OK") : _T("FAILED"));
	_gettch();
	
	return 0;
}