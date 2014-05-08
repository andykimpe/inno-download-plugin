#include "errordialog.h"
#include "trace.h"
#include "resource.h"
#include "ui.h"

static ErrorDialog *errDlgPtr = NULL;

ErrorDialog::ErrorDialog(Ui *parent)
{
	setUi(parent);
	errDlgPtr = this;
}

ErrorDialog::~ErrorDialog()
{
}

void ErrorDialog::setUi(Ui *parent)
{
	ui = parent;
}

void ErrorDialog::setErrorMsg(tstring msg)
{
	errorMsg = msg;
}

void ErrorDialog::setFileList(map<tstring, NetFile *> fileList)
{
	files = fileList;
}

void ErrorDialog::setComponents(set<tstring> componentList)
{
	components = componentList;
}

int ErrorDialog::exec()
{
	MessageBeep(MB_ICONWARNING);
	return (int)DialogBox(ui->dllHandle, MAKEINTRESOURCE(IDD_ERRORDIALOG), uiMainWindow(), (DLGPROC)ErrorDialogProc);
}

void ErrorDialog::localize()
{
	SetWindowText(handle,    ui->msg("Download failed").c_str());
	setItemText(IDRETRY,     ui->msg("Retry"));
	setItemText(IDIGNORE,    ui->msg("Ignore"));
	setItemText(IDABORT,     ui->msg("Cancel"));
	setItemText(IDC_ERRTEXT, ui->msg("Download failed") + _T(": ") + errorMsg);
	setItemText(IDC_FILESND, ui->msg("The following files was not downloaded:"));
}

void ErrorDialog::setItemText(int id, tstring text)
{
	SetWindowText(GetDlgItem(handle, id), text.c_str());
}

void ErrorDialog::fillFileList()
{
	for(map<tstring, NetFile *>::iterator i = files.begin(); i != files.end(); i++)
    {
		NetFile *file = i->second;

		if(!file->selected(components))
			continue;

		if(!file->downloaded)
			SendMessage(listBox, LB_ADDSTRING, 0, (ui->errorDlgMode == DLG_FILELIST) ?
						(LPARAM)file->getShortName().c_str() : (LPARAM)file->url.urlString.c_str());
	}
}

BOOL CALLBACK ErrorDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, IDC_ERRICON), STM_SETICON, (WPARAM)LoadIcon(NULL, IDI_WARNING), 0);
		ShowWindow(GetDlgItem(hDlg, IDIGNORE), errDlgPtr->ui->allowContinue ? SW_SHOW : SW_HIDE);
		errDlgPtr->handle = hDlg;
		errDlgPtr->listBox = GetDlgItem(hDlg, IDC_FILELIST);
		errDlgPtr->localize();
		errDlgPtr->fillFileList();
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDABORT:
		case IDRETRY:
		case IDIGNORE:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
