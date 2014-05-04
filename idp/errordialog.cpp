#include "errordialog.h"
#include "trace.h"
#include "resource.h"
#include "ui.h"

static ErrorDialog *errorDialogPtr = NULL;

ErrorDialog::ErrorDialog(Ui *parent)
{
	setUi(parent);
	errorDialogPtr = this;
}

ErrorDialog::~ErrorDialog()
{
}

void ErrorDialog::setUi(Ui *parent)
{
	ui = parent;
}

void ErrorDialog::setFileList(map<tstring, NetFile *> fileList)
{
	files = fileList;
}

int ErrorDialog::exec()
{
	return (int)DialogBox(ui->dllHandle, MAKEINTRESOURCE(IDD_ERRORDIALOG), uiMainWindow(), (DLGPROC)ErrorDialogProc);
}

BOOL CALLBACK ErrorDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
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
