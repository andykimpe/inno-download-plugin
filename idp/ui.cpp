#include "ui.h"
#include "url.h"
#include "timer.h"
#include "trace.h"

//HACK: to allow set parent window for InternetErrorDlg in Url class.
static HWND uiMainWindowHandle = NULL;

HWND uiMainWindow()
{
	return uiMainWindowHandle ? uiMainWindowHandle : GetDesktopWindow();
}

UI::UI()
{
	controls["TotalProgressBar"] = NULL;
	controls["FileProgressBar"]  = NULL;
	controls["TotalDownloaded"]	 = NULL;
	controls["FileDownloaded"]   = NULL;
	controls["FileName"]         = NULL;
	controls["Speed"]            = NULL;
	controls["Status"]           = NULL;
	controls["ElapsedTime"]      = NULL;
	controls["RemainingTime"]    = NULL;
	controls["NextButton"]		 = NULL;
	controls["BackButton"]       = NULL;
	controls["WizardForm"]		 = NULL;

	messages["KB/s"]                   = _T("KB/s");
	messages["%d of %d KB"]            = _T("%d of %d KB");
	messages["%d KB"]                  = _T("%d KB");
	messages["Initializing..."]        = _T("Initializing...");
	messages["Querying file sizes..."] = _T("Querying file sizes...");
	messages["Starting download..."]   = _T("Starting download...");
	messages["Connecting..."]          = _T("Connecting...");
	messages["Downloading..."]         = _T("Downloading...");
	messages["Done"]                   = _T("Done");
	messages["Error"]                  = _T("Error");
	messages["Cannot connect"]         = _T("Cannot connect");
	messages["Action cancelled"]       = _T("Action cancelled");
	messages["Unknown"]                = _T("Unknown");

	allowContinue  = false;
	hasRetryButton = true;
}

UI::~UI()
{
}

void UI::connectControl(tstring name, HWND handle)
{
	controls[toansi(name)] = handle;

	if(name.compare(_T("WizardForm")))
		uiMainWindowHandle = handle;
}

void UI::addMessage(tstring name, tstring message)
{
	if(message.length())
		messages[toansi(name)] = message;
}

void UI::setFileName(tstring filename)
{
	setLabelText(controls["FileName"], filename);
}

tstring UI::msg(string key)
{
	if(messages.count(key))
		return messages[key];
	else
		return tocurenc(key);
}

void UI::setProgressInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded)
{
	if(!(totalSize == FILE_SIZE_UNKNOWN))
	{
		double totalPercents = 100.0 / ((double)totalSize / (double)totalDownloaded);
		setProgressBarPos(controls["TotalProgressBar"], f2i(totalPercents));
	}

	if(!(fileSize == FILE_SIZE_UNKNOWN))
	{
		double filePercents  = 100.0 / ((double)fileSize / (double)fileDownloaded);
		setProgressBarPos(controls["FileProgressBar"], f2i(filePercents));
	}
}

void UI::setSpeedInfo(DWORD speed, DWORD remainingTime)
{
	setLabelText(controls["RemainingTime"], speed ? Timer::msecToStr(remainingTime, _T("%02u:%02u:%02u")) : msg("Unknown"));
	setLabelText(controls["Speed"],         itotstr((int)((double)speed / 1024.0 * 1000.0)) + _T(" ") + msg("KB/s"));
}

void UI::setSpeedInfo(DWORD speed)
{
	setLabelText(controls["RemainingTime"], msg("Unknown"));
	setLabelText(controls["Speed"],         itotstr((int)((double)speed / 1024.0 * 1000.0)) + _T(" ") + msg("KB/s"));
}

void UI::setSizeTimeInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded, DWORD elapsedTime)
{
	setLabelText(controls["ElapsedTime"], Timer::msecToStr(elapsedTime, _T("%02u:%02u:%02u")));
	
	if(!(totalSize == FILE_SIZE_UNKNOWN))
	{
		setLabelText(controls["TotalDownloaded"], tstrprintf(msg("%d of %d KB"), (int)(totalDownloaded / 1024), (int)(totalSize / 1024)));
		setLabelText(controls["FileDownloaded"],  tstrprintf(msg("%d of %d KB"), (int)(fileDownloaded  / 1024), (int)(fileSize  / 1024)));
	}
	else
	{
		setLabelText(controls["TotalDownloaded"], tstrprintf(msg("%d KB"), (int)(totalDownloaded / 1024)));
		setLabelText(controls["FileDownloaded"],  tstrprintf(msg("%d KB"), (int)(fileDownloaded  / 1024)));
	}
}

void UI::setStatus(tstring status)
{
	setLabelText(controls["Status"], status);
}

void UI::setMarquee(bool marquee, bool total)
{
	if(total)
		setProgressBarMarquee(controls["TotalProgressBar"], marquee);
	
	setProgressBarMarquee(controls["FileProgressBar"],  marquee);
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

void UI::setProgressBarMarquee(HWND pb, bool marquee)
{
	if(!pb)
		return;

	LONG style = GetWindowLong(pb, GWL_STYLE);

	if(marquee)
	{
		style |= PBS_MARQUEE;
		SetWindowLong(pb, GWL_STYLE, style);
		SendMessage(pb, PBM_SETMARQUEE, (WPARAM)TRUE, 0);
	}
	else
	{
		style ^= PBS_MARQUEE;
		SendMessage(pb, PBM_SETMARQUEE, (WPARAM)FALSE, 0);
		SetWindowLong(pb, GWL_STYLE, style);
		RedrawWindow(pb, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
	}
}

int UI::messageBox(tstring text, tstring caption, DWORD type)
{
	return MessageBox(controls["WizardForm"], text.c_str(), caption.c_str(), type);
}

void UI::clickNextButton()
{
	if(controls["NextButton"])
	{
		EnableWindow(controls["NextButton"], TRUE);
		SendMessage(controls["WizardForm"], WM_COMMAND, MAKEWPARAM(0, BN_CLICKED), (LPARAM)controls["NextButton"]);
	}
}

void UI::lockButtons()
{ 
	if(controls["BackButton"])
	{
		if(hasRetryButton)
			ShowWindow(controls["BackButton"], SW_HIDE);
		else
			EnableWindow(controls["BackButton"], FALSE);
	}

	if(controls["NextButton"])
		EnableWindow(controls["NextButton"], FALSE);
}

void UI::unlockButtons()
{ 
	if(controls["BackButton"])
	{
		if(hasRetryButton)
			ShowWindow(controls["BackButton"], SW_SHOW);
		else
			EnableWindow(controls["BackButton"], TRUE);
	}

	if(controls["NextButton"])
		EnableWindow(controls["NextButton"], allowContinue);
}