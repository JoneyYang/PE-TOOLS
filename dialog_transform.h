#ifndef DIALOG_TRANSFORM_20191212
#define DIALOG_TRANSFORM_20191212
#pragma once

#include <windows.h>
#include "dialog_main.h"

HWND g_hwndDialogTransform;

// Dialog消息处理函数
BOOL CALLBACK DialogProcTransform(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 初始化Dialog
void InitDialogTransform();

// 重叠DOS头和NT头
void DoOverlap();

// 执行合并节操作
void DoMerge();

// 执行整理头部空间操作
void DoTidyHeader();

// 执行动态改变Dos-stub大小
void DoResizeDosStub();

// 执行新增节操作
void DoInsertSection();

// 执行IAT静态注入
void DoIATInject();
#endif // !DIALOG_TRANSFORM_20191212
