#ifndef DIALOG_ABOUT_20191212
#define DIALOG_ABOUT_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogAbout;

BOOL CALLBACK DialogProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif // !DIALOG_ABOUT_20191212
