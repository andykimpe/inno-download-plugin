#pragma once

#include <windows.h>
#include <wininet.h>
#include <list>
#include "tstring.h"
#include "netfile.h"

using namespace std;

class Downloader
{
public:
	Downloader();
	~Downloader();

	void addFile(tstring url, tstring filename, int size = -1);
	void clearFiles();
	bool downloadFiles();

protected:
	bool downloadFile(NetFile *netFile);
	
	list<NetFile *> fileList;
	HINTERNET internet;
};
