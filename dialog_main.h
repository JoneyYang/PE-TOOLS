#ifndef DIALOG_MAIN_20191212
#define DIALOG_MAIN_20191212
#pragma once

#include <Windows.h> 
#include <commctrl.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <Shlwapi.h>

#include "resource.h"
#include "pe_tool.h"

#pragma comment(lib,"Shlwapi.lib")

#define  WM_DETAIL_SHOWEXPORT WM_USER + 0x01
#define  WM_DETAIL_SHOWIMPORT WM_USER + 0x02
#define  WM_DETAIL_SHOWRESOURCE WM_USER + 0x03
#define  WM_DETAIL_SHOWRELOCAL WM_USER + 0x04
#define  WM_DETAIL_SHOWBOUNDIMPORT WM_USER + 0x05
#define  WM_DETAIL_SHOWIAT WM_USER + 0x06

PVOID g_pFileBuffer;
DWORD g_dwFileSize;
HWND g_hwndMain;
HWND g_hwndListProcess;
HWND g_hwndListModules;
HINSTANCE g_hInstance;


// 对话框处理函数
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT UMsg, WPARAM wParam, LPARAM lParam);

// 初始化[Porcess]列表控件
void InitListViewProcess(HWND hwndDlg);
// 初始化[Modules]列表控件
void InitListViewModules(HWND hwndDlg);

// 遍历进程, 填充[Process]列表控件
void InitListContentProcess(hwndList);
// 遍历模块, 填充[Modules]列表控件
void InitListContentModule();

// 复制模块路径
void CopyModulePath();

// 设置消息
void SetStaticMessage(LPCTSTR str);

// 显示PE对话框
void ShowDialogPe();
// 显示Shell对话框
void ShowDialogShell();


#endif // !DIALOG_MAIN_20191212

