#pragma once

#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <commctrl.h>
#include <map>
#include <float.h>
#include "tstring.h"

using namespace std;

#define DLG_NONE   0
#define DLG_SIMPLE 1
#define DLG_LIST   2

class Ui
{
public:
	Ui();
	~Ui();

	void connectControl(tstring name, HWND handle);
	void addMessage(tstring name, tstring message);
	void setFileName(tstring filename);
	void setProgressInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded);
	void setSpeedInfo(DWORD speed, DWORD remainingTime);
	void setSpeedInfo(DWORD speed);
	void setSizeTimeInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded, DWORD elapsedTime);
	void setStatus(tstring status);
	int  messageBox(tstring text, tstring caption, DWORD type);
	void clickNextButton();
	void lockButtons();
	void unlockButtons();
	void setMarquee(bool marquee, bool total = true);
	void setDetailedMode(bool mode);

	tstring msg(string key);

	bool allowContinue;
	bool hasRetryButton;
	bool redrawBackground;
	bool errorDlgMode;

protected:
	void rightAlignLabel(HWND label, tstring text);
	void setLabelText(HWND l, tstring text);
	void setProgressBarPos(HWND pb, int pos);
	void setProgressBarMarquee(HWND pb, bool marquee);

	map<string, HWND>    controls;
	map<string, tstring> messages;
	bool                 detailedMode;
	tstring              statusStr;
};

HWND uiMainWindow();

#define f2i(x) (_isnan(x) ? 0 : (int)(x))
