#include "dialog_pe.h"

#include "Resource.h"

#include "dialog_directory.h"
#include "dialog_section.h"
#include "dialog_transform.h"


BOOL CALLBACK DialogProcPe(HWND hwndPeDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogPe = hwndPeDlg;
        InitPeDialog();
        break;

    case WM_CLOSE:
        // 处理窗口关闭消息
        EndDialog(hwndPeDlg, 0);
        break;

    case WM_COMMAND:
        // 子控件消息处理
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_PEQUIT:
            SendMessage(hwndPeDlg, WM_CLOSE, NULL, NULL);
            break;

        case IDC_BUTTON_DIRECTORY:
            if (IsWindow(g_hwndDialogDirectory) == FALSE)
            {
                HWND hwnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY), g_hwndDialogPe, DialogDirectoryProc);

                RECT rect;
                GetWindowRect(g_hwndDialogPe, &rect);
                SetWindowPos(hwnd, HWND_TOP, rect.right, rect.top, 0, 0, SWP_NOSIZE);
            }
            else
            {
                SendMessage(g_hwndDialogDirectory, WM_INITDIALOG, NULL, NULL);
            }
            break;
        case IDC_BUTTON_SECTIONS:
            if (IsWindow(g_hwndDialogSections) == FALSE)
            {
                HWND hwnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), g_hwndDialogPe, DialogProcSections);

                RECT rectPe;
                GetWindowRect(g_hwndDialogPe, &rectPe);
                //RECT rectSub;
                //GetWindowRect(hwnd, &rectSub);
                //LONG y = rectPe.bottom + (rectSub.bottom - rectSub.top);

                SetWindowPos(hwnd, HWND_TOP, rectPe.left, rectPe.bottom, 0, 0, SWP_NOSIZE);
            }
            else
            {
                SendMessage(g_hwndDialogSections, WM_INITDIALOG, NULL, NULL);
            }
            break;

        case IDC_BUTTON_TRANSFORM:
            if (IsWindow(g_hwndDialogTransform) == FALSE)
            {
                HWND hwnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_TRANSFORM), g_hwndDialogPe, DialogProcTransform);

                RECT rectTransform;
                GetWindowRect(hwnd, &rectTransform);
                LONG width = rectTransform.right - rectTransform.left;

                RECT rectPe;
                GetWindowRect(g_hwndDialogPe, &rectPe);

                SetWindowPos(hwnd, HWND_TOP, rectPe.left - width, rectPe.top, 0, 0, SWP_NOSIZE);
            }
            else
            {
                SendMessage(g_hwndDialogTransform, WM_INITDIALOG, NULL, NULL);
            }
            break;

        case IDC_BUTTON_SAVE:
            // 保存PE文件
            DoSavePeFile();
            break;

        default:
            result = FALSE;
            break;
        }


    default:
        result = FALSE;
    }


    return result;
}

void InitPeDialog()
{

    PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(g_pFileBuffer);

    if (!CheckPE64(g_pFileBuffer))
    {
        PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = GetOptionalHeader(g_pFileBuffer);

        TCHAR buf[32];
        wsprintf(buf, TEXT("%p"), pOptionalHeader->AddressOfEntryPoint);
        SetWinTextInPe(IDC_EDIT_EOP, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->ImageBase);
        SetWinTextInPe(IDC_EDIT_IMAGEBASE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfImage);
        SetWinTextInPe(IDC_EDIT_IMAGESIZE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfCode);
        SetWinTextInPe(IDC_EDIT_BASEOFCODE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfData);
        SetWinTextInPe(IDC_EDIT_BASEOFDATA, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SectionAlignment);
        SetWinTextInPe(IDC_EDIT_SECTIONALIGNMENT, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->FileAlignment);
        SetWinTextInPe(IDC_EDIT_FILEALIGNMENT, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Magic);
        SetWinTextInPe(IDC_EDIT_MAGIC, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Subsystem);
        SetWinTextInPe(IDC_EDIT_SUBSYSTEM, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->NumberOfSections);
        SetWinTextInPe(IDC_EDIT_NUMBEROFSECTIONS, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->TimeDateStamp);
        SetWinTextInPe(IDC_EDIT_TIMEDATESTAMP, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfHeaders);
        SetWinTextInPe(IDC_EDIT_SIZEOFHEADERS, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->Characteristics);
        SetWinTextInPe(IDC_EDIT_CHARACTERISTICS, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->CheckSum);
        SetWinTextInPe(IDC_EDIT_CHECKSUM, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->SizeOfOptionalHeader);
        SetWinTextInPe(IDC_EDIT_SIZEOFOPTIONHEADER, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->NumberOfRvaAndSizes);
        SetWinTextInPe(IDC_EDIT_NUMOFRVASIZE, buf);
    }
    else if (pFileHeader->SizeOfOptionalHeader == 0x00f0)
    {
        PIMAGE_OPTIONAL_HEADER64 pOptionalHeader = GetOptionalHeader64(g_pFileBuffer);

        TCHAR buf[32];
        wsprintf(buf, TEXT("%p"), pOptionalHeader->AddressOfEntryPoint);
        SetWinTextInPe(IDC_EDIT_EOP, buf);
        wsprintf(buf, TEXT("%I64X\0"), pOptionalHeader->ImageBase);
        SetWinTextInPe(IDC_EDIT_IMAGEBASE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfImage);
        SetWinTextInPe(IDC_EDIT_IMAGESIZE, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->BaseOfCode);
        SetWinTextInPe(IDC_EDIT_BASEOFCODE, buf);
        wsprintf(buf, TEXT("%p"), 0);
        SetWinTextInPe(IDC_EDIT_BASEOFDATA, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SectionAlignment);
        SetWinTextInPe(IDC_EDIT_SECTIONALIGNMENT, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->FileAlignment);
        SetWinTextInPe(IDC_EDIT_FILEALIGNMENT, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Magic);
        SetWinTextInPe(IDC_EDIT_MAGIC, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->Subsystem);
        SetWinTextInPe(IDC_EDIT_SUBSYSTEM, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->NumberOfSections);
        SetWinTextInPe(IDC_EDIT_NUMBEROFSECTIONS, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->TimeDateStamp);
        SetWinTextInPe(IDC_EDIT_TIMEDATESTAMP, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->SizeOfHeaders);
        SetWinTextInPe(IDC_EDIT_SIZEOFHEADERS, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->Characteristics);
        SetWinTextInPe(IDC_EDIT_CHARACTERISTICS, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->CheckSum);
        SetWinTextInPe(IDC_EDIT_CHECKSUM, buf);
        wsprintf(buf, TEXT("%p"), pFileHeader->SizeOfOptionalHeader);
        SetWinTextInPe(IDC_EDIT_SIZEOFOPTIONHEADER, buf);
        wsprintf(buf, TEXT("%p"), pOptionalHeader->NumberOfRvaAndSizes);
        SetWinTextInPe(IDC_EDIT_NUMOFRVASIZE, buf);
    }
}


void SetWinTextInPe(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogPe, dwEditId);
    SetWindowText(hEdit, buf);
}

void DoSavePeFile()
{
    TCHAR strFileName[MAX_PATH] = TEXT("new.exe");

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrFilter = TEXT("All File(*.*)\0*.*\0\0");
    ofn.hwndOwner = g_hwndDialogPe;
    ofn.lpstrFile = strFileName;
    ofn.nMaxFile = MAX_PATH;
    
    if (GetSaveFileName(&ofn) == FALSE)
    {
        SetStaticMessage(TEXT("未选择文件!"));
        return;
    }

    if (SavePeFile(strFileName, g_pFileBuffer, g_dwFileSize) == FALSE)
    {
        SetStaticMessage(TEXT("保存文件失败!"));
        MessageBox(g_hwndDialogPe, TEXT("保存文件失败"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
        return;
    }

    SetStaticMessage(TEXT("保存文件成功!"));
    MessageBox(g_hwndDialogPe, TEXT("保存文件成功"), TEXT("SUCCESS"), MB_OK | MB_ICONINFORMATION);

}   

