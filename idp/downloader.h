#pragma once

#include <windows.h>
#include <wininet.h>
#include <map>
#include "tstring.h"
#include "netfile.h"
#include "timer.h"
#include "ui.h"
#include "internetoptions.h"

#define IDP_USER_AGENT          _T("InnoDownloadPlugin/1.2")
#define DOWNLOAD_CANCEL_TIMEOUT 30000
#define READ_BUFFER_SIZE        1024

using namespace std;

class Downloader;

typedef void (*FinishedCallback)(Downloader *d, bool res);

class Downloader
{
public:
	Downloader();
	~Downloader();

	void      addFile(tstring url, tstring filename, DWORDLONG size = FILE_SIZE_UNKNOWN);
	void      addMirror(tstring url, tstring mirror);
	void      setMirrorList(Downloader *d);
	void      clearFiles();
	void      clearMirrors();
	bool	  downloadFiles();
	void      startDownload();
	void      stopDownload();
	DWORDLONG getFileSizes();
	int       filesCount();
	bool      filesDownloaded();
	bool      fileDownloaded(tstring url);
	DWORD	  getLastError();
	tstring	  getLastErrorStr();
	void      setUi(Ui *newUi);
	void      setUserAgent(tstring agent);
	void      setInternetOptions(InternetOptions opt);
	void      setFinishedCallback(FinishedCallback callback);

	bool stopOnError;

protected:
	bool openInternet();
	bool closeInternet();
	bool downloadFile(NetFile *netFile);
	bool checkMirrors(tstring url, bool download/* or get size */);
	void updateProgress(NetFile *file);
	void updateFileName(NetFile *file);
	void updateSpeed(NetFile *file, Timer *timer);
	void updateSizeTime(NetFile *file, Timer *timer);
	void updateStatus(tstring status);
	void setMarquee(bool marquee, bool total = true);
	void storeError();
	void storeError(tstring msg);
	tstring msg(string key);
	
	map<tstring, NetFile *>    files;
	multimap<tstring, tstring> mirrors;
	DWORDLONG				   filesSize;
	DWORDLONG				   downloadedFilesSize;
	HINTERNET				   internet;
	Timer					   sizeTimeTimer;
	DWORD					   errorCode;
	tstring					   errorStr;
	Ui                         *ui;
	InternetOptions            internetOptions;
	tstring                    userAgent;
	bool                       downloadCancelled;
	HANDLE                     downloadThread;
	FinishedCallback           finishedCallback;

	friend void downloadThreadProc(void *param);
};
