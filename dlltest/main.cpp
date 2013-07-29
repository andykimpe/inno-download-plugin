#include <stdio.h>
#include "../idp/idp.h"

int _tmain(int argc, _TCHAR* argv[])
{
	idpAddFile(_T("http://127.0.0.1/test1.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\dlltest\\test1.rar"));
	idpAddFile(_T("http://127.0.0.1/test2.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\dlltest\\test2.rar"));
	idpAddFile(_T("http://127.0.0.1/test3.rar"), _T("E:\\mydev\\InnoDownloadPlugin\\dlltest\\test3.rar"));
	
	idpStartDownload();

	_tprintf(_T("Download started\n"));
	_gettch();
	
	return 0;
}