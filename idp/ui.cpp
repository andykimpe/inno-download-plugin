#include "ui.h"
#include "trace.h"

string toansi(tstring s)
{
#ifdef UNICODE
	int bufsize = (int)s.length();
	char *buffer = new char[bufsize];
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, buffer, bufsize, NULL, NULL);
	string res = buffer;
	delete[] buffer;
	return res;
#else
	return s;
#endif
}

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
	string n = toansi(name);

	if(controls.count(n))
		controls[n] = handle;
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

void UI::setLabelText(HWND l, tstring text)
{
	if(l)
		PostMessage(l, WM_SETTEXT, 0, (LPARAM)text.c_str());
}

void UI::setProgressBarPos(HWND pb, int pos)
{
	if(pb)
		PostMessage(pb, PBM_SETPOS, (int)((65535.0 / 100.0) * pos), 0);
}
