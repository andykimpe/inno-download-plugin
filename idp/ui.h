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
	void addMessage(tstring name, tstring message);
	void setFileName(tstring filename);
	void setProgressInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded);
	void setSpeedInfo(DWORD speed, DWORD remainingTime);
	void setSizeTimeInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded, DWORD elapsedTime);
	void setStatus(tstring status);
	int  messageBox(tstring text, tstring caption, DWORD type);
	void clickNextButton();
	void lockButtons();
	void unlockButtons();

	bool allowContinue;

	map<string, tstring> messages;

protected:
	static void setLabelText(HWND l, tstring text);
	static void setProgressBarPos(HWND pb, int pos);

	map<string, HWND> controls;
};
