#include "dialog_directory.h"

#include "dialog_detail.h"
#include "dialog_section.h"
#include "dialog_pe.h"


BOOL CALLBACK DialogDirectoryProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogDirectory = hwndDlg;
        // 对话框初始化消息
        InitDialogDirectory();
        break;

    case WM_CLOSE:
        // 处理窗口关闭消息
        DestroyWindow(hwndDlg);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_DIRECTORYQUIT:
            SendMessage(hwndDlg, WM_CLOSE, NULL, NULL);
            break;
            
        case IDC_BUTTON_DIRECTORY_SECTIONS:
            // 弹出节表窗
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), hwndDlg, DialogProcSections);
            break;

        case IDC_BUTTON_DIR_EXPORT:
            g_dwShowID = IDC_BUTTON_DIR_EXPORT;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        case IDC_BUTTON_DIR_IMPORT:
            g_dwShowID = IDC_BUTTON_DIR_IMPORT;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        case IDC_BUTTON_DIR_RESOURCE:
            g_dwShowID = IDC_BUTTON_DIR_RESOURCE;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        case IDC_BUTTON_DIR_RELOCAL:
            g_dwShowID = IDC_BUTTON_DIR_RELOCAL;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        case IDC_BUTTON_DIR_BOUNDIMPORT:
            g_dwShowID = IDC_BUTTON_DIR_BOUNDIMPORT;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        case IDC_BUTTON_DIR_IAT:
            g_dwShowID = IDC_BUTTON_DIR_IAT;
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DETAIL), hwndDlg, DialogProcDetail);
            break;

        default:
            result = FALSE;
            break;
        }

        break;

    default:
        result = FALSE;
        break;
    }

    return result;
}


void InitDialogDirectory()
{
    // RVA列所有EDIT控件ID. 按PE结构的DATA_DIRECTORY_ENTRY排序
    DWORD rvaIds[16] =
    {
        IDC_EDIT_DIR_EXPORTRVA,
        IDC_EDIT_DIR_IMPORTRVA,
        IDC_EDIT_DIR_RESOURCERVA,
        IDC_EDIT_DIR_EXCEPTIONRVA,
        IDC_EDIT_DIR_SECURITYRVA,
        IDC_EDIT_DIR_RELOCATIONRVA,
        IDC_EDIT_DIR_DEBUGRVA,
        IDC_EDIT_DIR_COPYRIGHTRVA,
        IDC_EDIT_DIR_GLOBALPTRRVA,
        IDC_EDIT_DIR_TLSRVA,
        IDC_EDIT_DIR_LOADCONFIGRVA,
        IDC_EDIT_DIR_BOUNDIMPORTRVA,
        IDC_EDIT_DIR_IATRVA,
        IDC_EDIT_DIR_DELAYIMPORTRVA,
        IDC_EDIT_DIR_COMRVA,
        IDC_EDIT_DIR_RESERVEDRVA
    };

    // SIZE列所有EDIT控件ID. 按PE结构的DATA_DIRECTORY_ENTRY排序
    DWORD sizeIds[16] =
    {
        IDC_EDIT_DIR_EXPORTSIZE,
        IDC_EDIT_DIR_IMPORTSIZE,
        IDC_EDIT_DIR_RESOURCESIZE,
        IDC_EDIT_DIR_EXCEPTIONSIZE,
        IDC_EDIT_DIR_SECURITYSIZE,
        IDC_EDIT_DIR_RELOCATIONSIZE,
        IDC_EDIT_DIR_DEBUGSIZE,
        IDC_EDIT_DIR_COPYRIGHTSIZA,
        IDC_EDIT_DIR_GLOBALPTRSIZE,
        IDC_EDIT_DIR_TLSSIZE,
        IDC_EDIT_DIR_LOADCONFIGSIZE,
        IDC_EDIT_DIR_BOUNDIMPORTSIZE,
        IDC_EDIT_DIR_IATSIZE,
        IDC_EDIT_DIR_DELAYIMPORTSIZE,
        IDC_EDIT_DIR_COMSIZE,
        IDC_EDIT_DIR_RESERVEDSIZE,
    };

    HWND hwndEditRva;
    HWND hwndEditSize;
    TCHAR buffer[16];

    // 遍历所有目录表
    for (int row = 0; row < 16; row++)
    {
        PIMAGE_DATA_DIRECTORY p_DataDirectory = GetDataDirectory(g_pFileBuffer, row);

        // 填充VirtualAddress控件
        wsprintf(buffer, TEXT("%p"), p_DataDirectory->VirtualAddress);
        hwndEditRva = GetDlgItem(g_hwndDialogDirectory, rvaIds[row]);
        SetWindowText(hwndEditRva, buffer);

        // 填充Size控件
        wsprintf(buffer, TEXT("%p"), p_DataDirectory->Size);
        hwndEditSize = GetDlgItem(g_hwndDialogDirectory, sizeIds[row]);
        SetWindowText(hwndEditSize, buffer);
    }
}



