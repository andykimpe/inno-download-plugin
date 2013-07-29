#pragma once

#include "tstring.h"
#include "url.h"

using namespace std;

class NetFile
{
public:
	NetFile(tstring url, tstring filename, int filesize = -1);
	~NetFile();

	Url     url;
	tstring name;
	int     size;
	bool    downloaded;
};
