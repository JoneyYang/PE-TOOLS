#ifndef DIALOG_SECTIONS_20191212
#define DIALOG_SECTIONS_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogSections;

// Dialog消息处理函数
BOOL CALLBACK DialogProcSections(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 初始化Dialog
void InitSectionsDialog();

#endif // !DIALOG_SECTIONS_20191212
