#ifndef DIALOG_DETAIL_20191212
#define DIALOG_DETAIL_20191212
#pragma once

#include <windows.h>

#include "dialog_main.h"

HWND g_hwndDialogDetail;
DWORD g_dwShowID;

// Dialog消息处理函数
BOOL CALLBACK DialogProcDetail(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 打印导出表
void InitEditExport();
// 打印导入表
void InitEditImport();
// 打印导入表(PE32+)
void InitEditImport64();

// 打印资源表
void InitEditResouce();
// 打印绑定导入表
void InitEditBoundImport();
// 打印IAT表
void InitEditIat();
// 打印重定位表
void InitEditRelocal();







#endif // !DIALOG_DETAIL_20191212
