#ifndef DIALOG_PE_20191212
#define DIALOG_PE_20191212
#pragma once


#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogPe;

// Dialog��Ϣ������
BOOL CALLBACK DialogProcPe(HWND hwndPeDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ��ʼ��Dialog[PE]
void InitPeDialog();
// ����WindowsText
void SetWinTextInPe(DWORD dwEditId, PCTSTR buf);

// ����PE�ļ�
void DoSavePeFile();

#endif // !DIALOG_PE_20191212

