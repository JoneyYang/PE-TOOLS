#ifndef DIALOG_DIRECTORY_20191212
#define DIALOG_DIRECTORY_20191212
#pragma once


#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogDirectory;

// Dialog��Ϣ������
BOOL CALLBACK DialogDirectoryProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ��ʼ��Dialog
void InitDialogDirectory();

#endif // !DIALOG_DIRECTORY_20191212
