#include <process.h>
#include "downloader.h"
#include "file.h"
#include "trace.h"

Downloader::Downloader()
{
	stopOnError         = true;
	filesSize			= 0;
	downloadedFilesSize = 0;
	ui					= NULL;
	errorCode			= 0;
	internet			= NULL;
	downloadThread      = NULL;
	downloadCancelled   = false;
	finishedCallback    = NULL;
}

Downloader::~Downloader()
{
	clearFiles();
	clearMirrors();
}

void Downloader::setUi(Ui *newUi)
{
	ui = newUi;
}

void Downloader::setInternetOptions(InternetOptions opt)
{
	internetOptions = opt;

	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		NetFile *file = i->second;
		file->url.internetOptions = opt;
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
		files[url]->url.internetOptions = internetOptions;
	}
}

void Downloader::addMirror(tstring url, tstring mirror)
{
	mirrors.insert(pair<tstring, tstring>(url, mirror));
}

void Downloader::setMirrorList(Downloader *d)
{
	mirrors = d->mirrors;
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

void Downloader::clearMirrors()
{
	if(!mirrors.empty())
		mirrors.clear();
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

bool Downloader::fileDownloaded(tstring url)
{
	return files[url]->downloaded;
}

bool Downloader::openInternet()
{
	if(!internet)
		if(!(internet = InternetOpen(internetOptions.userAgent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))
			return false;

	if(internetOptions.connectTimeout != TIMEOUT_DEFAULT)
		InternetSetOption(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &internetOptions.connectTimeout, sizeof(DWORD));

	if(internetOptions.sendTimeout    != TIMEOUT_DEFAULT)
		InternetSetOption(internet, INTERNET_OPTION_SEND_TIMEOUT,    &internetOptions.sendTimeout,    sizeof(DWORD));

	if(internetOptions.receiveTimeout != TIMEOUT_DEFAULT)
		InternetSetOption(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &internetOptions.receiveTimeout, sizeof(DWORD));

#ifdef _DEBUG
	DWORD connectTimeout, sendTimeout, receiveTimeout, bufSize = sizeof(DWORD);

	InternetQueryOption(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &connectTimeout, &bufSize);
	InternetQueryOption(internet, INTERNET_OPTION_SEND_TIMEOUT,    &sendTimeout,    &bufSize);
	InternetQueryOption(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &receiveTimeout, &bufSize);

	TRACE(_T("Internet options:"));
	TRACE(_T("    Connect timeout: %d"), connectTimeout);
	TRACE(_T("    Send timeout   : %d"), sendTimeout);
	TRACE(_T("    Receive timeout: %d"), receiveTimeout);
#endif

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
	Ui *uitmp = ui;
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
	bool sizeUnknown = false;

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		updateStatus(msg("Getting file information..."));

        NetFile *file = i->second;

		if(downloadCancelled)
			break;

		if(file->size == FILE_SIZE_UNKNOWN)
		{
			try
			{
				try
				{
					updateFileName(file);
					file->size = file->url.getSize(internet);
				}
				catch(HTTPError &e)
				{
					updateStatus(msg(e.what()));
					//TODO: if allowContinue==0 & error code == file not found - stop.
				}
				
				if(file->size == FILE_SIZE_UNKNOWN)
					checkMirrors(i->first, false);
			}
			catch(InvalidCertError &e)
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

	if(sizeUnknown && !filesSize)
		filesSize = FILE_SIZE_UNKNOWN; //TODO: if only part of files has unknown size - ???

#ifdef _DEBUG
	TRACE(_T("getFileSizes result:"));

	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;
		TRACE(_T("    %s: %s"), file->getShortName().c_str(), (file->size == FILE_SIZE_UNKNOWN) ? _T("Unknown") : itotstr((DWORD)file->size).c_str()); 
	}
#endif

	return filesSize;
}

bool Downloader::downloadFiles()
{
	if(files.empty())
		return true;

	setMarquee(true);

	if(getFileSizes() == OPERATION_STOPPED)
	{
		TRACE(_T("OPERATION_STOPPED"));
		setMarquee(false);
		return false;
	}

	TRACE(_T("filesSize: %d"), (DWORD)filesSize);

	if(!openInternet())
	{
		storeError();
		setMarquee(false);
		return false;
	}

	sizeTimeTimer.start(500);
	updateStatus(msg("Starting download..."));
	TRACE(_T("Starting file download cycle..."));

	if(!(filesSize == FILE_SIZE_UNKNOWN))
		setMarquee(false);

    for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
        NetFile *file = i->second;

		if(downloadCancelled)
			break;

		if(!file->downloaded)
		{
			// If mirror was used in getFileSizes() function, check mirror first:
			if(file->mirrorUsed.length())
			{
				NetFile newFile(file->mirrorUsed, file->name, file->size);

				if(downloadFile(&newFile))
				{
					downloadedFilesSize += file->bytesDownloaded;
					continue;
				}
			}

			if(!downloadFile(file))
			{
				TRACE(_T("File was not downloaded."));

				if(checkMirrors(i->first, true))
					downloadedFilesSize += file->bytesDownloaded;
				else
				{
					if(stopOnError)
					{
						closeInternet();
						return false;
					}
					else
					{
						TRACE(_T("Ignoring file %s"), file->name.c_str());
					}
				}
			}
			else
				downloadedFilesSize += file->bytesDownloaded;
		}
    }

	closeInternet();
	return filesDownloaded();
}

bool Downloader::checkMirrors(tstring url, bool download/* or get size */)
{
	TRACE(_T("Checking mirrors for %s (%s)..."), url.c_str(), download ? _T("download") : _T("get size"));
	pair<multimap<tstring, tstring>::iterator, multimap<tstring, tstring>::iterator> fileMirrors = mirrors.equal_range(url);
	
	for(multimap<tstring, tstring>::iterator i = fileMirrors.first; i != fileMirrors.second; ++i)
	{
		tstring mirror = i->second;
		TRACE(_T("Checking mirror %s:"), mirror.c_str());
		NetFile f(mirror, files[url]->name, files[url]->size);

		if(download)
		{
			if(downloadFile(&f))
			{
				files[url]->downloaded = true;
				return true;
			}
		}
		else // get size
		{
			try
			{
				DWORDLONG size = f.url.getSize(internet);

				if(size != FILE_SIZE_UNKNOWN)
				{
					files[url]->size = size;
					files[url]->mirrorUsed = mirror;
					return true;
				}
			}
			catch(HTTPError &e)
			{
				updateStatus(msg(e.what()));
			}
		}
	}

	return false;
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
		setMarquee(false, stopOnError ? (netFile->size == FILE_SIZE_UNKNOWN) : false);
		updateStatus(msg(e.what()));
		storeError(msg(e.what()));
		return false;
	}

	if(!netFile->handle)
	{
		setMarquee(false, stopOnError ? (netFile->size == FILE_SIZE_UNKNOWN) : false);
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
		double speed = (double)file->bytesDownloaded / ((double)timer->totalElapsed() / 1000.0);
		double rtime = (double)(filesSize - (downloadedFilesSize + file->bytesDownloaded)) / speed * 1000.0;
		
		if((filesSize == FILE_SIZE_UNKNOWN) || ((downloadedFilesSize + file->bytesDownloaded) > filesSize))
			ui->setSpeedInfo(f2i(speed));
		else
			
			ui->setSpeedInfo(f2i(speed), f2i(rtime));
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
		return tstrprintf(msg("HTTP error %d"), errcode);

	return res;
}

void Downloader::storeError()
{
	errorCode = GetLastError();
	errorStr  = formatwinerror(errorCode);
}

void Downloader::storeError(tstring msg, DWORD errcode)
{
	errorCode = errcode;
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
