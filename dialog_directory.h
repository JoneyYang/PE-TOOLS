#ifndef DIALOG_DIRECTORY_20191212
#define DIALOG_DIRECTORY_20191212
#pragma once


#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogDirectory;

// Dialog消息处理函数
BOOL CALLBACK DialogDirectoryProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 初始化Dialog
void InitDialogDirectory();

#endif // !DIALOG_DIRECTORY_20191212
