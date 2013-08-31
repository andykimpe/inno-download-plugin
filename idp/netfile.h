#pragma once

#include "tstring.h"
#include "url.h"

using namespace std;

class NetFile
{
public:
	NetFile(tstring url, tstring filename, DWORDLONG filesize = FILE_SIZE_UNKNOWN);
	~NetFile();

	bool	open(HINTERNET internet);
	void	close();
	bool	read(void *buffer, DWORD size, DWORD *bytesRead);
	tstring getShortName();

	Url       url;
	tstring   name;
	DWORDLONG size;
	DWORDLONG bytesDownloaded;
	bool      downloaded;
	HINTERNET handle;
};
