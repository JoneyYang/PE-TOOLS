#ifndef DIALOG_SECTIONS_20191212
#define DIALOG_SECTIONS_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogSections;

// Dialog��Ϣ������
BOOL CALLBACK DialogProcSections(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ��ʼ��Dialog
void InitSectionsDialog();

#endif // !DIALOG_SECTIONS_20191212
