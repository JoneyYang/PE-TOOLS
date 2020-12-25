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


// �Ի�������
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT UMsg, WPARAM wParam, LPARAM lParam);

// ��ʼ��[Porcess]�б�ؼ�
void InitListViewProcess(HWND hwndDlg);
// ��ʼ��[Modules]�б�ؼ�
void InitListViewModules(HWND hwndDlg);

// ��������, ���[Process]�б�ؼ�
void InitListContentProcess(hwndList);
// ����ģ��, ���[Modules]�б�ؼ�
void InitListContentModule();

// ����ģ��·��
void CopyModulePath();

// ������Ϣ
void SetStaticMessage(LPCTSTR str);

// ��ʾPE�Ի���
void ShowDialogPe();
// ��ʾShell�Ի���
void ShowDialogShell();


#endif // !DIALOG_MAIN_20191212

