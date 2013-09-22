#include <process.h>
#include "downloader.h"
#include "file.h"
#include "trace.h"

Downloader::Downloader()
{
	filesSize			= 0;
	downloadedFilesSize = 0;
	ui					= NULL;
	errorCode			= 0;
	userAgent           = IDP_USER_AGENT;
	internet			= NULL;
	downloadThread      = NULL;
	downloadCancelled   = false;
	finishedCallback    = NULL;
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

void Downloader::setFinishedCallback(FinishedCallback callback)
{
	finishedCallback = callback;
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

void downloadThreadProc(void *param)
{
	Downloader *d = (Downloader *)param;
	bool res = d->downloadFiles();

	if((!d->downloadCancelled) && d->finishedCallback)
		d->finishedCallback(d, res);
}

void Downloader::startDownload()
{
	downloadThread = (HANDLE)_beginthread(&downloadThreadProc, 0, (void *)this);
}

void Downloader::stopDownload()
{
	UI *uitmp = ui;
	ui = NULL;
	downloadCancelled = true;
	WaitForSingleObject(downloadThread, DOWNLOAD_CANCEL_TIMEOUT);
	downloadCancelled = false;
	ui = uitmp;
}

DWORDLONG Downloader::getFileSizes()
{
	if(files.empty())
		return 0;

	updateStatus(msg("Initializing..."));

	if(!openInternet())
	{
		storeError();
		return FILE_SIZE_UNKNOWN;
	}

	filesSize = 0;

	updateStatus(msg("Getting file information..."));
	bool sizeUnknown = false;

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(downloadCancelled)
			break;

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
		else
			sizeUnknown = true;
    }

	closeInternet();

	if(sizeUnknown)
		filesSize = FILE_SIZE_UNKNOWN;

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

	if(!(filesSize == FILE_SIZE_UNKNOWN))
		setMarquee(false);

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(downloadCancelled)
			break;

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
	BYTE  buffer[READ_BUFFER_SIZE];
	DWORD bytesRead;
	File  file;

	updateFileName(netFile);
	updateStatus(msg("Connecting..."));
	setMarquee(true, false);

	try
	{
		netFile->open(internet);
	}
	catch(exception &e)
	{
		setMarquee(false, netFile->size == FILE_SIZE_UNKNOWN);
		updateStatus(msg(e.what()));
		storeError(msg(e.what()));
		return false;
	}

	if(!netFile->handle)
	{
		setMarquee(false, netFile->size == FILE_SIZE_UNKNOWN);
		updateStatus(msg("Cannot connect"));
		storeError();
		return false;
	}

	file.open(netFile->name);

	Timer progressTimer(100);
	Timer speedTimer(1000);

	updateStatus(msg("Downloading..."));

	if(!(netFile->size == FILE_SIZE_UNKNOWN))
		setMarquee(false, false);

	while(true)
	{
		if(downloadCancelled)
		{
			file.close();
			netFile->close();
			return true;
		}

		if(!netFile->read(buffer, READ_BUFFER_SIZE, &bytesRead))
		{
			setMarquee(false, netFile->size == FILE_SIZE_UNKNOWN);
			updateStatus(msg("Download failed"));
			storeError();
			file.close();
			netFile->close();
			return false;
		}

		if(bytesRead == 0)
			break;

		file.write(buffer, bytesRead);

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
	updateStatus(msg("Download complete"));

	file.close();
	netFile->close();
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
		double speed = (double)file->bytesDownloaded / (double)timer->totalElapsed();
		double rtime = (double)(filesSize - (downloadedFilesSize + file->bytesDownloaded)) / speed;
		
		if(!(filesSize == FILE_SIZE_UNKNOWN))
			ui->setSpeedInfo(f2i(speed), f2i(rtime));
		else
			ui->setSpeedInfo(f2i(speed));
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
	tstring res;

	if(ui)
		res = ui->msg(key);
	else
		return tocurenc(key);

	int errcode = _ttoi(res.c_str());

	if(errcode > 0)
		return tstrprintf(msg("Error %d"), errcode);

	return res;
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
