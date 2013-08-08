#include <stdio.h>
#include "downloader.h"
#include "trace.h"

Downloader::Downloader()
{
	filesSize			= 0;
	downloadedFilesSize = 0;
	ui					= NULL;
	errorCode			= 0;
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
	{
		storeError();
		return -1;
	}

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
	{
		storeError();
		return false;
	}

	sizeTimeTimer.start(500);

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

	updateFileName(netFile);

	if(!(inetfile = netFile->url.open(internet)))
	{
		storeError();
		return false;
	}

	file = _tfopen(netFile->name.c_str(), _T("wb"));

	Timer progressTimer(100);
	Timer speedTimer(1000);

	TRACE(_T("Downloading %s...\n"), netFile->getShortName().c_str());

	while(true)
	{
		if(!InternetReadFile(inetfile, buffer, 1024, &bytesRead))
		{
			storeError();
			fclose(file);
			netFile->url.close();
			return false;
		}

		if(bytesRead == 0)
			break;

		fwrite(buffer, 1, bytesRead, file);
		netFile->bytesDownloaded += bytesRead;

		if(progressTimer.elapsed())
			updateProgress(netFile);

		if(speedTimer.elapsed())
			updateSpeed(netFile, &speedTimer);

		if(sizeTimeTimer.elapsed())
			updateSizeTime(netFile, &sizeTimeTimer);
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

void Downloader::updateSpeed(NetFile *file, Timer *timer)
{
	if(ui)
	{
		DWORD speed = (DWORD)(file->bytesDownloaded / timer->totalElapsed());
		ui->setSpeedInfo(speed, (DWORD)((filesSize - (downloadedFilesSize + file->bytesDownloaded)) / speed));
	}
}

void Downloader::updateSizeTime(NetFile *file, Timer *timer)
{
	if(ui)
		ui->setSizeTimeInfo(filesSize, downloadedFilesSize + file->bytesDownloaded, file->size, file->bytesDownloaded, timer->totalElapsed());
}

void Downloader::storeError()
{
	errorCode = GetLastError();
}

DWORD Downloader::getLastError()
{
	return errorCode;
}

tstring Downloader::getLastErrorStr()
{
	_TCHAR *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf, 0, NULL);
	tstring res = buf;
	LocalFree(buf);

	return res;
}
