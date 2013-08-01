#include <stdio.h>
#include "downloader.h"

Downloader::Downloader()
{
	filesSize = 0;
}

Downloader::~Downloader()
{
	clearFiles();
}

void Downloader::addFile(tstring url, tstring filename, int size)
{
	fileList.push_back(new NetFile(url, filename, size));
}

void Downloader::clearFiles()
{
	if(fileList.empty())
		return;

	for(list<NetFile *>::iterator pfile = fileList.begin(); pfile != fileList.end(); pfile++)
    {
        NetFile *file = *pfile;
		delete file;
    }

	fileList.clear();
}

DWORDLONG Downloader::getFileSizes()
{
	if(fileList.empty())
		return 0;

	if(!(internet = InternetOpen(_T("Inno Download Plugin/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))
		return -1;

	filesSize = 0;

    for(list<NetFile *>::iterator pfile = fileList.begin(); pfile != fileList.end(); pfile++)
    {
        NetFile *file = *pfile;

		if(file->size == -1)
			file->size = file->url.getSize(internet);

		filesSize += file->size;
    }

	InternetCloseHandle(internet);
	return filesSize;
}

bool Downloader::downloadFiles()
{
	if(fileList.empty())
		return true;

	if(!filesSize)
		getFileSizes();

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