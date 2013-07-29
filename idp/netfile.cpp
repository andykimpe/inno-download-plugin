#include "netfile.h"

NetFile::NetFile(tstring fileurl, tstring filename, int filesize): url(fileurl)
{
	name       = filename;
	size       = filesize;
	downloaded = false;
}

NetFile::~NetFile()
{
}
