#pragma once

#include <windows.h>
#include <commctrl.h>
#include <map>
#include "tstring.h"

using namespace std;

class UI
{
public:
	UI();
	~UI();

	void connectControl(tstring name, HWND handle);
	void setFileName(tstring filename);
	void setProgressInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded);
	void setSpeedInfo(DWORD speed, DWORD remainingTime);
	void setSizeTimeInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded, DWORD elapsedTime);
	int  messageBox(tstring text, tstring caption, DWORD type);

	bool detailed;

protected:
	static void setLabelText(HWND l, tstring text);
	static void setProgressBarPos(HWND pb, int pos);

	map<string, HWND> controls;
};
