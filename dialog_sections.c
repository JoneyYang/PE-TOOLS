#include "dialog_section.h"

BOOL CALLBACK DialogProcSections(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogSections = hwndDlg;
        // 对话框初始化消息
        InitSectionsDialog();
        break;

    case WM_CLOSE:
        // 处理窗口关闭消息
        DestroyWindow(hwndDlg);
        break;
    default:
        result = FALSE;
        break;
    }

    return result;

}


void InitSectionsDialog()
{
    // 1. 初始化所有列
    // 2. 填充所有节数据


    LV_COLUMN lv = { 0 };
    HWND hwndList;

    // 获取句柄
    hwndList = GetDlgItem(g_hwndDialogSections, IDC_LIST_SECTIONS);
    ListView_DeleteAllItems(hwndList);
    for (size_t i = 0; i < 6; i++)
    {
        ListView_DeleteColumn(hwndList, 0);
    }

    // 设置整行选中
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCFMT_CENTER;

    // 第一列
    lv.pszText = TEXT("节名");
    lv.cx = 60;
    lv.iSubItem = 0;
    ListView_InsertColumn(hwndList, 0, &lv);

    // 第二列
    lv.pszText = TEXT("文件偏移");
    lv.cx = 80;
    lv.iSubItem = 1;
    ListView_InsertColumn(hwndList, 1, &lv);

    // 第三列
    lv.pszText = TEXT("文件大小");
    lv.cx = 80;
    lv.iSubItem = 2;
    ListView_InsertColumn(hwndList, 2, &lv);

    // 第四列
    lv.pszText = TEXT("内存偏移");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hwndList, 3, &lv);

    // 第五列
    lv.pszText = TEXT("内存大小");
    lv.cx = 80;
    lv.iSubItem = 4;
    ListView_InsertColumn(hwndList, 4, &lv);

    // 第六列
    lv.pszText = TEXT("节表属性");
    lv.cx = 80;
    lv.iSubItem = 5;
    ListView_InsertColumn(hwndList, 5, &lv);


    // 获取所有节表数据
    for (size_t row = 0; row < GetSectionNumbers(g_pFileBuffer); row++)
    {
        PIMAGE_SECTION_HEADER pSectionHeader = GetSectionHeader(g_pFileBuffer, row);
        TCHAR tBuffer[16];

        LV_ITEM vitem = { 0 };
        vitem.mask = LVIF_TEXT | LVCFMT_LEFT;
        vitem.iItem = row;

        // 第一列(节名)
        char cBuffer[16] = { 0 };
        sprintf_s(cBuffer, 16, ("%.*s"), IMAGE_SIZEOF_SHORT_NAME, pSectionHeader->Name); // Name不是以'\x00'结尾的, 格式化字符串需要限定长度

#ifdef _UNICODE
        // UNICODE版本需要将 多字节转成宽字节
        ZeroMemory(tBuffer, sizeof(TCHAR)* 16);
        MultiByteToWideChar(CP_ACP, 0, cBuffer, -1, tBuffer, IMAGE_SIZEOF_SHORT_NAME);
        vitem.pszText = tBuffer;
#else
        // ANSI版本可以直接使用窄字符
        vitem.pszText = cBuffer;
#endif // _UNICODE
        vitem.iSubItem = 0;
        ListView_InsertItem(hwndList, &vitem);

        // 第二列(文件偏移)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->PointerToRawData);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 1;
        ListView_SetItem(hwndList, &vitem);


        // 第三列(文件大小)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->SizeOfRawData);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 2;
        ListView_SetItem(hwndList, &vitem);

        // 第四列(内存偏移)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->VirtualAddress);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 3;
        ListView_SetItem(hwndList, &vitem);

        // 第五列(内存大小)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Misc.VirtualSize);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 4;
        ListView_SetItem(hwndList, &vitem);

        // 第六列(节表属性)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Characteristics);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 5;
        ListView_SetItem(hwndList, &vitem);
    }
}

