#ifndef DIALOG_SHELL_20191212
#define DIALOG_SEHLL_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogShell;

BOOL CALLBACK DialogProcShell(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL SelectFile(PCTSTR strFilter, PTSTR FileName);

// 生成加密的Src文件
void GenerateSrcEncry();
// 生成加壳的文件
void GenerateShellFull();

#endif // !DIALOG_SHELL_20191212
