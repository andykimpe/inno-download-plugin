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
