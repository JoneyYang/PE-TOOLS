#include "dialog_section.h"

BOOL CALLBACK DialogProcSections(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogSections = hwndDlg;
        // �Ի����ʼ����Ϣ
        InitSectionsDialog();
        break;

    case WM_CLOSE:
        // �����ڹر���Ϣ
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
    // 1. ��ʼ��������
    // 2. ������н�����


    LV_COLUMN lv = { 0 };
    HWND hwndList;

    // ��ȡ���
    hwndList = GetDlgItem(g_hwndDialogSections, IDC_LIST_SECTIONS);
    ListView_DeleteAllItems(hwndList);
    for (size_t i = 0; i < 6; i++)
    {
        ListView_DeleteColumn(hwndList, 0);
    }

    // ��������ѡ��
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCFMT_CENTER;

    // ��һ��
    lv.pszText = TEXT("����");
    lv.cx = 60;
    lv.iSubItem = 0;
    ListView_InsertColumn(hwndList, 0, &lv);

    // �ڶ���
    lv.pszText = TEXT("�ļ�ƫ��");
    lv.cx = 80;
    lv.iSubItem = 1;
    ListView_InsertColumn(hwndList, 1, &lv);

    // ������
    lv.pszText = TEXT("�ļ���С");
    lv.cx = 80;
    lv.iSubItem = 2;
    ListView_InsertColumn(hwndList, 2, &lv);

    // ������
    lv.pszText = TEXT("�ڴ�ƫ��");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hwndList, 3, &lv);

    // ������
    lv.pszText = TEXT("�ڴ��С");
    lv.cx = 80;
    lv.iSubItem = 4;
    ListView_InsertColumn(hwndList, 4, &lv);

    // ������
    lv.pszText = TEXT("�ڱ�����");
    lv.cx = 80;
    lv.iSubItem = 5;
    ListView_InsertColumn(hwndList, 5, &lv);


    // ��ȡ���нڱ�����
    for (size_t row = 0; row < GetSectionNumbers(g_pFileBuffer); row++)
    {
        PIMAGE_SECTION_HEADER pSectionHeader = GetSectionHeader(g_pFileBuffer, row);
        TCHAR tBuffer[16];

        LV_ITEM vitem = { 0 };
        vitem.mask = LVIF_TEXT | LVCFMT_LEFT;
        vitem.iItem = row;

        // ��һ��(����)
        char cBuffer[16] = { 0 };
        sprintf_s(cBuffer, 16, ("%.*s"), IMAGE_SIZEOF_SHORT_NAME, pSectionHeader->Name); // Name������'\x00'��β��, ��ʽ���ַ�����Ҫ�޶�����

#ifdef _UNICODE
        // UNICODE�汾��Ҫ�� ���ֽ�ת�ɿ��ֽ�
        ZeroMemory(tBuffer, sizeof(TCHAR)* 16);
        MultiByteToWideChar(CP_ACP, 0, cBuffer, -1, tBuffer, IMAGE_SIZEOF_SHORT_NAME);
        vitem.pszText = tBuffer;
#else
        // ANSI�汾����ֱ��ʹ��խ�ַ�
        vitem.pszText = cBuffer;
#endif // _UNICODE
        vitem.iSubItem = 0;
        ListView_InsertItem(hwndList, &vitem);

        // �ڶ���(�ļ�ƫ��)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->PointerToRawData);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 1;
        ListView_SetItem(hwndList, &vitem);


        // ������(�ļ���С)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->SizeOfRawData);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 2;
        ListView_SetItem(hwndList, &vitem);

        // ������(�ڴ�ƫ��)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->VirtualAddress);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 3;
        ListView_SetItem(hwndList, &vitem);

        // ������(�ڴ��С)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Misc.VirtualSize);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 4;
        ListView_SetItem(hwndList, &vitem);

        // ������(�ڱ�����)
        wsprintf(tBuffer, TEXT("%p"), pSectionHeader->Characteristics);
        vitem.pszText = tBuffer;
        vitem.iSubItem = 5;
        ListView_SetItem(hwndList, &vitem);
    }
}

