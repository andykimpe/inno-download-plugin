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

	allowContinue  = false;
	hasRetryButton = true;
	detailedMode   = false;

	_tsetlocale(LC_ALL, _T(""));
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
	setLabelText(controls["Speed"],         formatspeed(speed, msg("KB/s"), msg("MB/s")));
}

void UI::setSpeedInfo(DWORD speed)
{
	setLabelText(controls["RemainingTime"], msg("Unknown"));
	setLabelText(controls["Speed"],         formatspeed(speed, msg("KB/s"), msg("MB/s")));
}

void UI::setSizeTimeInfo(DWORDLONG totalSize, DWORDLONG totalDownloaded, DWORDLONG fileSize, DWORDLONG fileDownloaded, DWORD elapsedTime)
{
	setLabelText(controls["ElapsedTime"], Timer::msecToStr(elapsedTime, _T("%02u:%02u:%02u")));
	
	if(!(totalSize == FILE_SIZE_UNKNOWN))
	{
		setLabelText(controls["TotalDownloaded"], formatsize(msg("%.2f of %.2f"), totalDownloaded, totalSize, msg("KB"), msg("MB"), msg("GB")));
		setLabelText(controls["FileDownloaded"],  formatsize(msg("%.2f of %.2f"), fileDownloaded,  fileSize,  msg("KB"), msg("MB"), msg("GB")));
	}
	else
	{
		setLabelText(controls["TotalDownloaded"], formatsize(totalDownloaded, msg("KB"), msg("MB"), msg("GB")));
		setLabelText(controls["FileDownloaded"],  formatsize(fileDownloaded,  msg("KB"), msg("MB"), msg("GB")));
	}

	//NOTE: RedrawWindow needed because these labels are actually TPanel's
	RedrawWindow(controls["TotalDownloaded"], NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
	RedrawWindow(controls["FileDownloaded"],  NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
}

void UI::setStatus(tstring status)
{
	statusStr = status;
	setLabelText(detailedMode ? controls["Status"] : controls["TotalProgressLabel"], status);
}

void UI::setMarquee(bool marquee, bool total)
{
	if(total)
		setProgressBarMarquee(controls["TotalProgressBar"], marquee);
	
	setProgressBarMarquee(controls["FileProgressBar"],  marquee);
}

void UI::setDetailedMode(bool mode)
{
	detailedMode = mode;

	if(detailedMode)
	{
		setLabelText(controls["Status"], statusStr);
		setLabelText(controls["TotalProgressLabel"], msg("Total progress"));
	}
	else
		setLabelText(controls["TotalProgressLabel"], statusStr);
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