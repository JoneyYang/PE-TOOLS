#ifndef DIALOG_TRANSFORM_20191212
#define DIALOG_TRANSFORM_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogTransform;

// Dialog��Ϣ������
BOOL CALLBACK DialogProcTransform(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ��ʼ��Dialog
void InitDialogTransform();

// �ص�DOSͷ��NTͷ
void DoOverlap();

// ִ�кϲ��ڲ���
void DoMerge();

// ִ������ͷ���ռ����
void DoTidyHeader();

// ִ�ж�̬�ı�Dos-stub��С
void DoResizeDosStub();

// ִ�������ڲ���
void DoInsertSection();

// ִ��IAT��̬ע��
void DoIATInject();
#endif // !DIALOG_TRANSFORM_20191212
