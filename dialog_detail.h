#ifndef DIALOG_DETAIL_20191212
#define DIALOG_DETAIL_20191212
#pragma once

#include <windows.h>

#include "dialog_main.h"

HWND g_hwndDialogDetail;
DWORD g_dwShowID;

// Dialog��Ϣ������
BOOL CALLBACK DialogProcDetail(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ��ӡ������
void InitEditExport();
// ��ӡ�����
void InitEditImport();
// ��ӡ�����(PE32+)
void InitEditImport64();

// ��ӡ��Դ��
void InitEditResouce();
// ��ӡ�󶨵����
void InitEditBoundImport();
// ��ӡIAT��
void InitEditIat();
// ��ӡ�ض�λ��
void InitEditRelocal();







#endif // !DIALOG_DETAIL_20191212
