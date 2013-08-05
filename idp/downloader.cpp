#include <stdio.h>
#include "downloader.h"
#include "trace.h"

Downloader::Downloader()
{
	filesSize			= 0;
	downloadedFilesSize = 0;
	ui					= NULL;
}

Downloader::~Downloader()
{
	clearFiles();
}

void Downloader::setUI(UI *newUI)
{
	ui = newUI;
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
	filesSize			= 0;
	downloadedFilesSize = 0;
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
			else
				downloadedFilesSize += file->bytesDownloaded;
    }

	InternetCloseHandle(internet);
	return true;
}

bool Downloader::downloadFile(NetFile *netFile)
{
	HINTERNET inetfile;
	BYTE	  buffer[1024];
	DWORD	  bytesRead;
	FILE	 *file;
	DWORD	  startTime;
	DWORD	  elapsedTime;

	updateFileName(netFile);

	if(!(inetfile = netFile->url.open(internet)))
		return false;

	file = _tfopen(netFile->name.c_str(), _T("wb"));
	startTime = GetTickCount();

	TRACE(_T("Downloading %s...\n"), netFile->getShortName().c_str());

	while(true)
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
		netFile->bytesDownloaded += bytesRead;

		elapsedTime = GetTickCount();
		if((elapsedTime - startTime) > 100)
		{
			updateProgress(netFile);
			startTime = elapsedTime;
		}
	}

	TRACE(_T("Done\n"));

	fclose(file);
	netFile->url.close();
	netFile->downloaded = true;

	return true;
}

void Downloader::updateProgress(NetFile *file)
{
	if(ui)
		ui->setProgressInfo(filesSize, downloadedFilesSize + file->bytesDownloaded, file->size, file->bytesDownloaded);
}

void Downloader::updateFileName(NetFile *file)
{
	if(ui)
		ui->setFileName(file->getShortName());
}
