#include "dialog_shell.h"
#include "encry.h"


BOOL CALLBACK DialogProcShell(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogShell = hwndDlg;
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;

    case WM_COMMAND:
        // �ӿؼ���Ϣ����
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SRCFILE:
        {
                                   TCHAR strFileName[MAX_PATH] = { 0 };
                                   if (SelectFile(TEXT("Execute File(*.exe)\0*.exe;\0All File(*.*)\0*.*\0\0"), strFileName))
                                   {
                                       SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_SRCFILE), strFileName);
                                   }
                                   break;
        }
        case IDC_BUTTON_SHELLFILE:
        {
                                     TCHAR strFileName[MAX_PATH] = { 0 };
                                     if (SelectFile(TEXT("Execute File(*.exe)\0*.exe;\0All File(*.*)\0*.*\0\0"), strFileName))
                                     {
                                         SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_SHELLFILE), strFileName);
                                     }
                                     break;
        }
        case IDC_BUTTON_SRCCRYPT:
            GenerateSrcEncry();
            break;

        case IDC_BUTTON_SHELLEXE:
            GenerateShellFull();
            break;

        default:
            result = FALSE;
            break;
        }

    default:
        result = FALSE;
        break;
    }

    return result;

}

BOOL SelectFile(PCTSTR strFilter, PTSTR FileName)
{
    // �����ļ�ѡ����
    OPENFILENAMEW st = { 0 };
    st.lStructSize = sizeof(OPENFILENAME);
    st.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    st.hwndOwner = g_hwndMain;
    st.lpstrFilter = strFilter;
    st.lpstrFile = FileName;
    st.nMaxFile = MAX_PATH;

    if (GetOpenFileName(&st) == FALSE)
    {
        SetStaticMessage(TEXT("δѡ���ļ�!"));
        return FALSE;
    }
    return TRUE;
}

void GenerateSrcEncry()
{
    // 1. ��ȡ�ļ�
    // 2. �����ļ�
    // 3. ���ļ�д�뵽����

    // ��ȡ����·��
    TCHAR strSaveFileName[MAX_PATH] = TEXT("Src-Encry");

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFilter = TEXT("All File(*.*)\0*.*\0\0");
    ofn.hwndOwner = g_hwndDialogShell;
    ofn.lpstrFile = strSaveFileName;
    ofn.nMaxFile = MAX_PATH;

    if (GetSaveFileName(&ofn) == FALSE)
    {
        SetStaticMessage(TEXT("ѡ���ļ�ʧ��!"));
        MessageBox(g_hwndDialogShell, TEXT("�����ļ�ʧ��"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }



    // ��ȡԴ�ļ���ַ
    TCHAR strSrcFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SRCFILE), strSrcFileName, MAX_PATH);

    // ��ȡPE�ļ�
    PVOID pFileBuffer;
    DWORD dwFileSize;
    if (ReadPeFile(strSrcFileName, &pFileBuffer, &dwFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("��ȡSrc�ļ�ʧ��!"), TEXT("ERROR"), MB_OK);
        return;
    }

    // �����ļ�
    if (EncryptData(pFileBuffer, dwFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("�����ļ�ʧ��"), TEXT("ERROR"), MB_OK);
        return;
    }

    // �����ܵ�����д�뱾��. 
    FILE *pFile;
    _tfopen_s(&pFile, strSaveFileName, TEXT("wb"));
    fwrite(pFileBuffer, dwFileSize, 1, pFile);

    // �ͷ���Դ
    fclose(pFile);
    free(pFileBuffer);

    MessageBox(g_hwndDialogShell, TEXT("���ɼ������ݳɹ�"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}

void GenerateShellFull()
{

    // 1. ��ȡ�ļ�
    // 2. �����ļ�
    // 3. ��Shell�ļ������һ����
    // 4. �����ܵ�������Ӹ��Ƶ��½�. 


    // ��ȡԴ�ļ���ַ
    TCHAR strSrcFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SRCFILE), strSrcFileName, MAX_PATH);

    // ��ȡSrc�ļ�
    PVOID pSrcFileBuffer;
    DWORD dwSrcFileSize;
    if (ReadPeFile(strSrcFileName, &pSrcFileBuffer, &dwSrcFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("��ȡSrc�ļ�ʧ��!"), TEXT("ERROR"), MB_OK);
        return;
    }

    // ��ȡShell�ļ���ַ
    TCHAR strShellFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SHELLFILE), strShellFileName, MAX_PATH);

    // ��ȡShell�ļ�
    PVOID pShellFileBuffer;
    DWORD dwShellFileSize;
    if (ReadPeFile(strShellFileName, &pShellFileBuffer, &dwShellFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("��ȡShell�ļ�ʧ��!"), TEXT("ERROR"), MB_OK);
        return;
    }


    // �����ļ�
    if (EncryptData(pSrcFileBuffer, dwSrcFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("�����ļ�ʧ��"), TEXT("ERROR"), MB_OK);
        return;
    }


    // ������
    PVOID pNewFileBuffer = NULL;
    PVOID pNewSection = NULL;
    DWORD dwNewFileSize = 0;
    DWORD dwNewSectionSize = 0;
    // �ɶ� 
    DWORD characteristics = IMAGE_SCN_MEM_READ;

    if (InsertSection(pShellFileBuffer,
        dwShellFileSize,
        dwSrcFileSize,
        characteristics,
        ".encry",
        &pNewFileBuffer,
        &pNewSection,
        &dwNewFileSize,
        &dwNewSectionSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("������ʧ��"), TEXT("ERROR"), MB_OK);
        return;
    }

    // ���½ڵ����ݳ�ʼ���ɼ���״̬
    EncryptData(pNewSection,dwNewSectionSize);

    // �����ݸ��Ƶ����һ����
    memcpy(pNewSection, pSrcFileBuffer,dwSrcFileSize);

    // ��ȡ����·��
    TCHAR strSaveFileName[MAX_PATH] = TEXT("Src-Shelled.exe");

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFilter = TEXT("All File(*.*)\0*.*\0\0");
    ofn.hwndOwner = g_hwndDialogShell;
    ofn.lpstrFile = strSaveFileName;
    ofn.nMaxFile = MAX_PATH;

    if (GetSaveFileName(&ofn) == FALSE)
    {
        SetStaticMessage(TEXT("ѡ���ļ�ʧ��!"));
        MessageBox(g_hwndDialogShell, TEXT("�����ļ�ʧ��"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    // �����ܵ�����д�뱾��. 
    FILE *pFile;
    _tfopen_s(&pFile, strSaveFileName, TEXT("wb+"));
    fwrite(pNewFileBuffer,1, dwNewFileSize, pFile);

    // �ͷ���Դ
    fclose(pFile);
    free(pShellFileBuffer);
    free(pSrcFileBuffer);

    MessageBox(g_hwndDialogShell, TEXT("����Shell�ɹ�"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}
