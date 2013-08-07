#include "ui.h"
#include "timer.h"
#include "trace.h"

UI::UI()
{
	controls["progressBarTotal"]   = NULL;
	controls["progressBarFile"]    = NULL;
	controls["sizeLabelTotal"]	   = NULL;
	controls["sizeLabelFile"]      = NULL;
	controls["fileNameLabel"]      = NULL;
	controls["speedLabel"]         = NULL;
	controls["statusLabel"]        = NULL;
	controls["elapsedTimeLabel"]   = NULL;
	controls["remainingTimeLabel"] = NULL;
	controls["nextButton"]		   = NULL;

	detailed = false;
}

UI::~UI()
{
}

void UI::connectControl(tstring name, HWND handle)
{
	controls[toansi(name)] = handle;
}

void UI::setFileName(tstring filename)
{
	setLabelText(controls["fileNameLabel"], filename);
}

void UI::setProgressInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded)
{
	int filePercents  = (int)(100.0 / ((double)fileSize / (double)fileDownloaded));
	int totalPercents = (int)(100.0 / ((double)totalSize / (double)(totalDownloaded)));

	setProgressBarPos(controls["progressBarTotal"], totalPercents);
	setProgressBarPos(controls["progressBarFile"], filePercents);
}

void UI::setSpeedInfo(DWORD speed, DWORD remainingTime)
{
	setLabelText(controls["remainingTimeLabel"], Timer::msecToStr(remainingTime, _T("%02u:%02u:%02u")));
	setLabelText(controls["speedLabel"],         itotstr((int)((double)speed / 1024.0 * 1000.0)));
}

void UI::setLabelText(HWND l, tstring text)
{
	if(l)
		SendMessage(l, WM_SETTEXT, 0, (LPARAM)text.c_str());
}

void UI::setProgressBarPos(HWND pb, int pos)
{
	if(pb)
		PostMessage(pb, PBM_SETPOS, (int)((65535.0 / 100.0) * pos), 0);
}
