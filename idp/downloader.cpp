#include <stdio.h>
#include "downloader.h"
#include "trace.h"

Downloader::Downloader()
{
	filesSize			= 0;
	downloadedFilesSize = 0;
	ui					= NULL;
	errorCode			= 0;
	userAgent           = _T("Inno Download Plugin/1.0");
	internet			= NULL;
}

Downloader::~Downloader()
{
	clearFiles();
}

void Downloader::addFile(tstring url, tstring filename, DWORDLONG size)
{
	if(!files.count(url))
		files[url] = new NetFile(url, filename, size);
}

void Downloader::clearFiles()
{
	if(files.empty())
		return;

	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		NetFile *file = i->second;
		delete file;
    }

	files.clear();
	filesSize			= 0;
	downloadedFilesSize = 0;
}

int Downloader::filesCount()
{
	return (int)files.size();
}

bool Downloader::filesDownloaded()
{
	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		NetFile *file = i->second;
		if(!file->downloaded)
			return false;
    }

	return true;
}

bool Downloader::openInternet()
{
	if(!internet)
		if(!(internet = InternetOpen(userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))
			return false;

	return true;
}

bool Downloader::closeInternet()
{
	if(internet)
	{
		bool res = InternetCloseHandle(internet) != NULL;
		internet = NULL;
		return res;
	}
	else
		return true;
}

DWORDLONG Downloader::getFileSizes()
{
	if(files.empty())
		return 0;

	updateStatus(msg("Initializing..."));

	if(!openInternet())
	{
		storeError();
		return -1; //TODO: Exception?
	}

	filesSize = 0;

	updateStatus(msg("Querying file sizes..."));

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(file->size == FILE_SIZE_UNKNOWN)
			file->size = file->url.getSize(internet);

		if(!(file->size == FILE_SIZE_UNKNOWN))
			filesSize += file->size;
    }

	closeInternet();
	return filesSize;
}

bool Downloader::downloadFiles()
{
	if(files.empty())
		return true;

	getFileSizes();

	if(!openInternet())
	{
		storeError();
		return false;
	}

	sizeTimeTimer.start(500);

	updateStatus(msg("Starting download..."));

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(!file->downloaded)
			if(!downloadFile(file))
			{
				InternetCloseHandle(internet);
				return false;
			}
			else
				downloadedFilesSize += file->bytesDownloaded;
    }

	closeInternet();
	return true;
}

bool Downloader::downloadFile(NetFile *netFile)
{
	HINTERNET inetfile;
	BYTE	  buffer[1024];
	DWORD	  bytesRead;
	FILE	 *file;

	updateFileName(netFile);
	updateStatus(msg("Connecting..."));

	if(!(inetfile = netFile->url.open(internet)))
	{
		updateStatus(msg("Cannot connect"));
		storeError();
		return false;
	}

	file = _tfopen(netFile->name.c_str(), _T("wb"));

	Timer progressTimer(100);
	Timer speedTimer(1000);

	updateStatus(msg("Downloading..."));
	netFile->bytesDownloaded = 0; //NOTE: remove, if download resume will be implemented

	while(true)
	{
		if(!InternetReadFile(inetfile, buffer, 1024, &bytesRead))
		{
			updateStatus(msg("Error"));
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

	updateProgress(netFile);
	updateSpeed(netFile, &speedTimer);
	updateSizeTime(netFile, &sizeTimeTimer);
	updateStatus(msg("Done"));

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

void Downloader::updateStatus(tstring status)
{
	if(ui)
		ui->setStatus(status);
}

tstring Downloader::msg(string key)
{
	if(ui)
		return ui->messages[key];
	else
		return _T("");
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
	_TCHAR buf[1024];
	memset(buf, 0, 1024);

	if((errorCode >= 12000) && (errorCode <= 12174))
		FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, GetModuleHandle(_T("wininet.dll")), errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 1024, NULL);
	else
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 1024, NULL);
	
	tstring res = buf;
	return res;
}
