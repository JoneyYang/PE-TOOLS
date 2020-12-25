#include "dialog_transform.h"
#include "dialog_section.h"
#include "dialog_directory.h"
#include "dialog_pe.h"



BOOL CALLBACK DialogProcTransform(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogTransform = hwndDlg;
        // 对话框初始化消息
        InitDialogTransform();
        break;

    case WM_CLOSE:
        // 处理窗口关闭消息
        DestroyWindow(hwndDlg);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_TRANS_OVERLAP:
            // 重叠 DOS头 和 NT头
            DoOverlap();
            InitDialogTransform();
            break;

        case IDC_BUTTON_TRANS_MERGE:
            // 合并所有节
            DoMerge();
            InitDialogTransform();
            break;

        case IDC_BUTTON_TRANS_TIDYHEADER:
            DoTidyHeader();
            InitDialogTransform();
            break;

        case IDC_BUTTON_TRANS_SECTIONS:
            // 弹出节表窗
            CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), hwndDlg, DialogProcSections);
            break;

        case IDC_BUTTON_TRANS_RESIZESTUB:
            DoResizeDosStub();
            InitDialogTransform();
            break;

        case IDC_BUTTON_TRANS_INSERTSECTION:
            DoInsertSection();
            InitDialogTransform();
            break;

        case IDC_BUTTON_TRANS_IATINJECT:
            DoIATInject();
            InitDialogTransform();
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

void InitDialogTransform()
{
    TCHAR buffer[32];

    // 计算DOS-STUB 大小
    DWORD dwStubSize = GetDosStubSize(g_pFileBuffer);
    wsprintf(buffer, TEXT("0x%04X"), dwStubSize);
    SetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_STUBSIZE), buffer);

    // 计算头部可用空间大小
    DWORD dwAvailableSize = GetHeaderAvailableSize(g_pFileBuffer);
    wsprintf(buffer, TEXT("0x%04X"), dwAvailableSize);
    SetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_HEADERAVAILSIZE), buffer);

    // 头部大小
    DWORD dwSizeOfHeader = GetOptionalHeader(g_pFileBuffer)->SizeOfHeaders; // SizeOfHeaders字段 32/64偏移都是一样的(NT+0x54). 所以不用区分
    wsprintf(buffer, TEXT("0x%04X"), dwSizeOfHeader);
    SetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_SIZEOFHEADERS), buffer);

    SendMessage(g_hwndDialogDirectory, WM_INITDIALOG, NULL, NULL);
    SendMessage(g_hwndDialogSections, WM_INITDIALOG, NULL, NULL);
    SendMessage(g_hwndDialogPe, WM_INITDIALOG, NULL, NULL);
}

void DoOverlap()
{
    if (OverlapDosAndNt(g_pFileBuffer) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("操作失败!"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
    }
    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}

void DoMerge()
{
    PVOID pMergedFileBuffer = NULL;
    DWORD dwMergedFileBufferSize = 0;
    if (MergeAllSections(g_pFileBuffer, &pMergedFileBuffer, &dwMergedFileBufferSize) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("操作失败!"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);

    free(g_pFileBuffer);
    g_pFileBuffer = pMergedFileBuffer;
    g_dwFileSize = dwMergedFileBufferSize;
}

void DoTidyHeader()
{
    if (RearrangeHeaderData(g_pFileBuffer) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("操作失败!"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }
    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}

void DoResizeDosStub()
{
    // 读取Edit控件 文本内容
    TCHAR strBuffer[128];
    if (GetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_RESIZESTUB), strBuffer, 128) == 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("未输入大小"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }
    // 字符串转数字
    DWORD dwInputSize = StrToInt(strBuffer);

    DWORD dwRealSize = 0;
    if (ResizeDosStub(g_pFileBuffer, dwInputSize, &dwRealSize) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("失败!"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    // 修改Edit值
    wsprintf(strBuffer, TEXT("%d"), dwRealSize);
    SetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_RESIZESTUB), strBuffer);

    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}

void DoInsertSection()
{
    // 读取Edit控件内容
    TCHAR strSectionName[16];
    TCHAR strSectionSize[16];

    if (GetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_SECTIONNAME), strSectionName, 16) == 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("未输入节名称"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }
    if (GetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_SECTIONSIZE), strSectionSize, 16) == 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("未输入节大小"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    PSTR pcSectionName[8] = { 0 };
#ifdef _UNICODE
    // 把UNICODE转成ANSI
    WideCharToMultiByte(CP_OEMCP, 0, strSectionName, -1, pcSectionName, 8, NULL, NULL);
#else
    strcpy_s(pcSectionName, 10, strSectionName);
#endif // _UNICODE


    DWORD dwSectionsSize = StrToInt(strSectionSize);
    if (dwSectionsSize <= 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("节大小必须大于0"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }


    // 统计节属性
    DWORD dwCharacteristics = 0;
    if (IsDlgButtonChecked(g_hwndDialogTransform, IDC_CHECK_TRANS_READ) == BST_CHECKED)
    {
        dwCharacteristics = dwCharacteristics | IMAGE_SCN_MEM_READ;
    }
    if (IsDlgButtonChecked(g_hwndDialogTransform, IDC_CHECK_TRANS_WRITE) == BST_CHECKED)
    {
        dwCharacteristics = dwCharacteristics | IMAGE_SCN_MEM_WRITE;
    }
    if (IsDlgButtonChecked(g_hwndDialogTransform, IDC_CHECK_TRANS_EXECUTE) == BST_CHECKED)
    {
        dwCharacteristics = dwCharacteristics | IMAGE_SCN_MEM_EXECUTE;
    }
    if (IsDlgButtonChecked(g_hwndDialogTransform, IDC_CHECK_TRANS_CODE) == BST_CHECKED)
    {
        dwCharacteristics = dwCharacteristics | IMAGE_SCN_CNT_CODE;
    }


    DWORD dwAvailableSize = GetHeaderAvailableSize(g_pFileBuffer);
    if (dwAvailableSize < sizeof(IMAGE_SECTION_HEADER))
    {
        MessageBox(g_hwndDialogTransform, TEXT("头部可用空间不足"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    PVOID pNewFileBuffer = NULL;
    DWORD dwNewFileBufferSize = 0;
    if (InsertSection(g_pFileBuffer, g_dwFileSize, dwSectionsSize, dwCharacteristics, pcSectionName,
        &pNewFileBuffer, NULL, &dwNewFileBufferSize, NULL) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("插入新节失败"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);

    free(g_pFileBuffer);
    g_pFileBuffer = pNewFileBuffer;
    g_dwFileSize = dwNewFileBufferSize;

}

void DoIATInject()
{
    // 读取Edit控件内容
    TCHAR strDllName[128];
    TCHAR strFunName[128];


    if (GetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_IATINJECT_DLLNAME), strDllName, 128) == 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("未输入DLL名称"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }
    if (GetWindowText(GetDlgItem(g_hwndDialogTransform, IDC_EDIT_TRANS_IATINJECT_FUNNAME), strFunName, 128) == 0)
    {
        MessageBox(g_hwndDialogTransform, TEXT("未输入函数名称"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    PVOID funNameArr[1] = { strFunName };
    PVOID pNewFileBuffer;
    DWORD dwNewFileBufferSize;
    if (IatInject(g_pFileBuffer, g_dwFileSize, strDllName, funNameArr, 1, &pNewFileBuffer, &dwNewFileBufferSize) == FALSE)
    {
        MessageBox(g_hwndDialogTransform, TEXT("IAT注入失败!"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    MessageBox(g_hwndDialogTransform, TEXT("SUCCESS!"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);


    // 释放原BUFFE
    free(g_pFileBuffer);
    g_pFileBuffer = pNewFileBuffer;
    g_dwFileSize = dwNewFileBufferSize;


    return;
}

