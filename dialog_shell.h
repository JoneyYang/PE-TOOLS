#ifndef DIALOG_SHELL_20191212
#define DIALOG_SEHLL_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogShell;

BOOL CALLBACK DialogProcShell(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL SelectFile(PCTSTR strFilter, PTSTR FileName);

// ���ɼ��ܵ�Src�ļ�
void GenerateSrcEncry();
// ���ɼӿǵ��ļ�
void GenerateShellFull();

#endif // !DIALOG_SHELL_20191212
