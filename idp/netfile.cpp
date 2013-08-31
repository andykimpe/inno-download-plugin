#include "netfile.h"

NetFile::NetFile(tstring fileurl, tstring filename, DWORDLONG filesize): url(fileurl)
{
	name			= filename;
	size			= filesize;
	bytesDownloaded = 0;
	downloaded		= false;
	handle			= NULL;
}

NetFile::~NetFile()
{
}

bool NetFile::open(HINTERNET internet)
{
	bytesDownloaded = 0; //NOTE: remove, if download resume will be implemented
	return (handle = url.open(internet)) != NULL;
}

void NetFile::close()
{
	url.close();
}

bool NetFile::read(void *buffer, DWORD size, DWORD *bytesRead)
{
	BOOL res = InternetReadFile(handle, buffer, size, bytesRead);
	bytesDownloaded += *bytesRead;
	return res != FALSE;
}

tstring NetFile::getShortName()
{
	size_t off = name.rfind(_T('\\'));

	if(off == tstring::npos)
		off = 0;
	else
		off++;

	size_t len = name.length() - off;
	return name.substr(off, len);
}
