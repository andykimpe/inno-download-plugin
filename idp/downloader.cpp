#include <stdio.h>
#include "downloader.h"
#include "file.h"
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

void Downloader::setUI(UI *newUI)
{
	ui = newUI;
}

void Downloader::setUserAgent(tstring agent)
{
	userAgent = agent;
}

void Downloader::setSecurityOptions(SecurityOptions opt)
{
	securityOptions = opt;

	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		NetFile *file = i->second;
		file->url.securityOptions = opt;
	}
}

void Downloader::addFile(tstring url, tstring filename, DWORDLONG size)
{
	if(!files.count(url))
	{
		files[url] = new NetFile(url, filename, size);
		files[url]->url.securityOptions = securityOptions;
	}
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
		{
			try
			{
				file->size = file->url.getSize(internet);
			}
			catch(exception &e)
			{
				updateStatus(msg(e.what()));
				storeError(msg(e.what()));
				closeInternet();
				return OPERATION_STOPPED;
			}
		}

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

	setMarquee(true);

	if(getFileSizes() == OPERATION_STOPPED)
	{
		setMarquee(false);
		return false;
	}

	if(!openInternet())
	{
		storeError();
		setMarquee(false);
		return false;
	}

	sizeTimeTimer.start(500);

	updateStatus(msg("Starting download..."));
	setMarquee(false);

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(!file->downloaded)
			if(!downloadFile(file))
			{
				closeInternet();
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
	File	  file;

	updateFileName(netFile);
	updateStatus(msg("Connecting..."));
	setMarquee(true, false);

	try
	{
		inetfile = netFile->url.open(internet);
	}
	catch(exception &e)
	{
		setMarquee(false, false);
		updateStatus(msg(e.what()));
		storeError(msg(e.what()));
		return false;
	}

	if(!inetfile)
	{
		setMarquee(false, false);
		updateStatus(msg("Cannot connect"));
		storeError();
		return false;
	}

	file.open(netFile->name);

	Timer progressTimer(100);
	Timer speedTimer(1000);

	updateStatus(msg("Downloading..."));
	setMarquee(false, false);
	netFile->bytesDownloaded = 0; //NOTE: remove, if download resume will be implemented

	while(true)
	{
		if(!InternetReadFile(inetfile, buffer, 1024, &bytesRead))
		{
			setMarquee(false, false);
			updateStatus(msg("Error"));
			storeError();
			file.close();
			netFile->url.close();
			return false;
		}

		if(bytesRead == 0)
			break;

		file.write(buffer, bytesRead);
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

	file.close();
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

void Downloader::setMarquee(bool marquee, bool total)
{
	if(ui)
		ui->setMarquee(marquee, total);
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
	errorStr  = formatwinerror(errorCode);
}

void Downloader::storeError(tstring msg)
{
	errorCode = 0;
	errorStr  = msg;
}

DWORD Downloader::getLastError()
{
	return errorCode;
}

tstring Downloader::getLastErrorStr()
{
	return errorStr;
}
