#include <stdio.h>
#include "downloader.h"

Downloader::Downloader()
{
}

Downloader::~Downloader()
{
}

void Downloader::addFile(tstring url, tstring filename, int size)
{
	fileList.push_back(new NetFile(url, filename, size));
}

void Downloader::clearFiles()
{
	for(list<NetFile *>::iterator pfile = fileList.begin(); pfile != fileList.end(); pfile++)
    {
        NetFile *file = *pfile;
		delete file;
    }

	fileList.clear();
}

bool Downloader::downloadFiles()
{
	if(!(internet = InternetOpen(_T("Inno Download Plugin/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))
		return false;

    for(list<NetFile *>::iterator pfile = fileList.begin(); pfile != fileList.end(); pfile++)
    {
        NetFile *file = *pfile;

		if(!file->downloaded)
			if(!downloadFile(file))
			{
				InternetCloseHandle(internet);
				return false;
			}
    }

	InternetCloseHandle(internet);
	return true;
}

bool Downloader::downloadFile(NetFile *netFile)
{
	HINTERNET inetfile;
	BYTE buffer[1024];
	DWORD bytesRead;
	FILE *file;

	if(!(inetfile = netFile->url.open(internet)))
		return false;

	file = _tfopen(netFile->name.c_str(), _T("wb"));
	
	for(int i = 1; 1; i++)
	{
		if(!InternetReadFile(inetfile, buffer, 1024, &bytesRead))
		{
			fclose(file);
			netFile->url.close();
			return false;
		}

		if(bytesRead == 0)
			break;

		fwrite(buffer, 1, bytesRead, file);
	}

	fclose(file);
	netFile->url.close();
	netFile->downloaded = true;

	return true;
}