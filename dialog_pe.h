#ifndef DIALOG_PE_20191212
#define DIALOG_PE_20191212
#pragma once


#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogPe;

// Dialog消息处理函数
BOOL CALLBACK DialogProcPe(HWND hwndPeDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 初始化Dialog[PE]
void InitPeDialog();
// 设置WindowsText
void SetWinTextInPe(DWORD dwEditId, PCTSTR buf);

// 保存PE文件
void DoSavePeFile();

#endif // !DIALOG_PE_20191212

