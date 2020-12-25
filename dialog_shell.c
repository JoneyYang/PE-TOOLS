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
        // 子控件消息处理
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
    // 弹出文件选择器
    OPENFILENAMEW st = { 0 };
    st.lStructSize = sizeof(OPENFILENAME);
    st.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    st.hwndOwner = g_hwndMain;
    st.lpstrFilter = strFilter;
    st.lpstrFile = FileName;
    st.nMaxFile = MAX_PATH;

    if (GetOpenFileName(&st) == FALSE)
    {
        SetStaticMessage(TEXT("未选择文件!"));
        return FALSE;
    }
    return TRUE;
}

void GenerateSrcEncry()
{
    // 1. 读取文件
    // 2. 加密文件
    // 3. 将文件写入到本地

    // 获取保存路径
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
        SetStaticMessage(TEXT("选择文件失败!"));
        MessageBox(g_hwndDialogShell, TEXT("保存文件失败"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }



    // 获取源文件地址
    TCHAR strSrcFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SRCFILE), strSrcFileName, MAX_PATH);

    // 读取PE文件
    PVOID pFileBuffer;
    DWORD dwFileSize;
    if (ReadPeFile(strSrcFileName, &pFileBuffer, &dwFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("读取Src文件失败!"), TEXT("ERROR"), MB_OK);
        return;
    }

    // 加密文件
    if (EncryptData(pFileBuffer, dwFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("加密文件失败"), TEXT("ERROR"), MB_OK);
        return;
    }

    // 将加密的数据写入本地. 
    FILE *pFile;
    _tfopen_s(&pFile, strSaveFileName, TEXT("wb"));
    fwrite(pFileBuffer, dwFileSize, 1, pFile);

    // 释放资源
    fclose(pFile);
    free(pFileBuffer);

    MessageBox(g_hwndDialogShell, TEXT("生成加密数据成功"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}

void GenerateShellFull()
{

    // 1. 读取文件
    // 2. 加密文件
    // 3. 在Shell文件中添加一个节
    // 4. 将加密的数据添加复制到新节. 


    // 获取源文件地址
    TCHAR strSrcFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SRCFILE), strSrcFileName, MAX_PATH);

    // 读取Src文件
    PVOID pSrcFileBuffer;
    DWORD dwSrcFileSize;
    if (ReadPeFile(strSrcFileName, &pSrcFileBuffer, &dwSrcFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("读取Src文件失败!"), TEXT("ERROR"), MB_OK);
        return;
    }

    // 获取Shell文件地址
    TCHAR strShellFileName[MAX_PATH];
    GetWindowText(GetDlgItem(g_hwndDialogShell, IDC_EDIT_SHELLFILE), strShellFileName, MAX_PATH);

    // 读取Shell文件
    PVOID pShellFileBuffer;
    DWORD dwShellFileSize;
    if (ReadPeFile(strShellFileName, &pShellFileBuffer, &dwShellFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("读取Shell文件失败!"), TEXT("ERROR"), MB_OK);
        return;
    }


    // 加密文件
    if (EncryptData(pSrcFileBuffer, dwSrcFileSize) == FALSE)
    {
        MessageBox(g_hwndDialogShell, TEXT("加密文件失败"), TEXT("ERROR"), MB_OK);
        return;
    }


    // 新增节
    PVOID pNewFileBuffer = NULL;
    PVOID pNewSection = NULL;
    DWORD dwNewFileSize = 0;
    DWORD dwNewSectionSize = 0;
    // 可读 
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
        MessageBox(g_hwndDialogShell, TEXT("新增节失败"), TEXT("ERROR"), MB_OK);
        return;
    }

    // 对新节的数据初始化成加密状态
    EncryptData(pNewSection,dwNewSectionSize);

    // 将数据复制到最后一个节
    memcpy(pNewSection, pSrcFileBuffer,dwSrcFileSize);

    // 获取保存路径
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
        SetStaticMessage(TEXT("选择文件失败!"));
        MessageBox(g_hwndDialogShell, TEXT("保存文件失败"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    // 将加密的数据写入本地. 
    FILE *pFile;
    _tfopen_s(&pFile, strSaveFileName, TEXT("wb+"));
    fwrite(pNewFileBuffer,1, dwNewFileSize, pFile);

    // 释放资源
    fclose(pFile);
    free(pShellFileBuffer);
    free(pSrcFileBuffer);

    MessageBox(g_hwndDialogShell, TEXT("生成Shell成功"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);
}
