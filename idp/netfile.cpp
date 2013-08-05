#include "netfile.h"

NetFile::NetFile(tstring fileurl, tstring filename, int filesize): url(fileurl)
{
	name			= filename;
	size			= filesize;
	bytesDownloaded = 0;
	downloaded		= false;
}

NetFile::~NetFile()
{
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
