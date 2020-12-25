#include <tchar.h>

#include "pe_tool.h"
#include "dialog_main.h"
#include "dialog_pe.h"
#include "dialog_about.h"
#include "dialog_shell.h"

DWORD g_dwRow = 0;

// MAIN Dialog 消息处理函数
BOOL CALLBACK DialogProcMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;
    switch (uMsg) {
        case WM_INITDIALOG:
            InitListViewProcess(hwndDlg); // 初始化Process ListCtrl
            InitListViewModules(hwndDlg); // 初始化Modules ListCtrl
            g_hwndMain = hwndDlg;
            break;

        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            break;

        case WM_COMMAND:
            // 子控件消息处理
            switch (LOWORD(wParam)) {
                case IDC_BUTTON_EXIT: // 退出按钮
                    EndDialog(hwndDlg, 0);
                    break;

                case IDC_BUTTON_ABOUT: // 关于按钮
                    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), g_hwndMain, DialogProcAbout);
                    break;

                case IDC_BUTTON_PE: // PE查看器按钮
                    // PE对话框
                    ShowDialogPe();
                    break;

                case IDC_BUTTON_ADD_SHELL:
                    ShowDialogShell();
                    break;


                default:
                    result = FALSE;
                    break;
            }

        case WM_NOTIFY: {
            NMHDR* pNmhdr = (NMHDR*)lParam;
            if (wParam == IDC_LIST_PROCESS && pNmhdr->code == LVN_ITEMCHANGED) {
                InitListContentModule();
            } else if (wParam == IDC_LIST_MODULES && pNmhdr->code == NM_DBLCLK) {
                CopyModulePath();
            }
            break;
        }

        default:
            result = FALSE;
            break;
    }

    return result;
}


void InitListViewProcess(HWND hwndDlg)
{
    LV_COLUMN lv = {0};

    // 获取句柄
    HWND hwndList = GetDlgItem(hwndDlg, IDC_LIST_PROCESS);
    g_hwndListProcess = hwndList;

    // 设置整行选中
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // 第一列
    lv.pszText = TEXT("进程名");
    lv.cx = 180;
    lv.iSubItem = 0;
    ListView_InsertColumn(hwndList, 0, &lv);


    // 第二列
    lv.pszText = TEXT("PID");
    lv.cx = 50;
    lv.iSubItem = 1;
    ListView_InsertColumn(hwndList, 1, &lv);

    // 第三列
    lv.pszText = TEXT("镜像地址");
    lv.cx = 80;
    lv.iSubItem = 2;
    ListView_InsertColumn(hwndList, 2, &lv);

    // 第四列
    lv.pszText = TEXT("镜像大小");
    lv.cx = 80;
    lv.iSubItem = 3;
    ListView_InsertColumn(hwndList, 3, &lv);

    // 遍历进程列表
    InitListContentProcess(hwndList);
}

void InitListViewModules(HWND hwndDlg)
{
    LV_COLUMN lv = {0};
    HWND hwndList;

    // 获取句柄
    hwndList = GetDlgItem(hwndDlg, IDC_LIST_MODULES);
    // 设置整行选中
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // 第一列
    lv.pszText = TEXT("模块名称");
    lv.cx = 100;
    lv.iSubItem = 0;
    ListView_InsertColumn(hwndList, 0, &lv);

    // 第二列
    lv.pszText = TEXT("模块位置");
    lv.cx = 300;
    lv.iSubItem = 1;
    ListView_InsertColumn(hwndList, 1, &lv);

    g_hwndListModules = hwndList;
}


void InitListContentProcess(hwndList)
{
    // 1. 取得所有进程
    // 2. 获取进程信息(进程名称, 主模块ImageBase, 主模块ImageSize, 主模块EOP)
    // 3. 填充列表数据


    HANDLE lpSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (lpSnapshot == INVALID_HANDLE_VALUE) {
        MessageBox(g_hwndMain, TEXT("创建快照失败"), TEXT("ERROR"), MB_OK);
        return;
    }

    PROCESSENTRY32 p32;
    p32.dwSize = sizeof(p32);
    BOOL pr = Process32First(lpSnapshot, &p32);
    // 遍历所有进程

    for (int row = 0; pr; pr = Process32Next(lpSnapshot, &p32)) {
        // 进程ID
        DWORD dwPid = p32.th32ProcessID;

        // 尝试OpenProcess.  无法打开的进程直接过滤掉. 
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
        if (!hProcess)
            continue;
        else
            CloseHandle(hProcess);

        // 进程名
        LPTSTR name = p32.szExeFile;
        // 主模块信息
        MODULEINFO mi = {0};
        if (GetMainModuleInfo(dwPid, &mi)) // 过滤无法访问的进程
        {
            LV_ITEM vitem = {0};
            vitem.mask = LVIF_TEXT;
            vitem.iItem = row;

            // 第一列(进程名)
            vitem.pszText = name;
            vitem.iSubItem = 0;
            ListView_InsertItem(hwndList, &vitem);

            // 第二列(PID)
            TCHAR buffer[16];
            wsprintf(buffer, TEXT("%d"), dwPid);
            vitem.pszText = buffer;
            vitem.iSubItem = 1;
            ListView_SetItem(hwndList, &vitem);

            // 第三列(主模块基地址)
            wsprintf(buffer, TEXT("%p"), mi.lpBaseOfDll);
            vitem.pszText = buffer;
            vitem.iSubItem = 2;
            ListView_SetItem(hwndList, &vitem);

            // 第四列(镜像大小)
            wsprintf(buffer, TEXT("%p"), mi.SizeOfImage);
            vitem.pszText = buffer;
            vitem.iSubItem = 3;
            ListView_SetItem(hwndList, &vitem);
            row++;
        }
    }

    CloseHandle(lpSnapshot);
}


void InitListContentModule()
{
    // 1. 获取到进程的ID
    // 2. 遍历模块
    // 3. 填充数据. 数据获取失败. 清空列表

    DWORD dwRowId;
    TCHAR szPid[0x20] = {0};

    dwRowId = SendMessage(g_hwndListProcess, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (dwRowId == -1) {
        return;
    }
    // 清空所有数据行
    ListView_DeleteAllItems(g_hwndListModules);


    // 获取PID
    LV_ITEM lv = {0};
    lv.iSubItem = 1;      // 指定列
    lv.pszText = szPid;   // 缓冲区
    lv.cchTextMax = 0x20; // 缓冲区大小
    SendMessage(g_hwndListProcess, LVM_GETITEMTEXT, dwRowId, (DWORD)&lv);
    DWORD dwPid = StrToInt(szPid); // 字符串转数字


    // 遍历模块
    DWORD dwNumOfModules = 0;
    BOOL bResult = FALSE;
    HMODULE hModules[1024];
    HANDLE hProcess;

    bResult = GetAllModules(dwPid, hModules, 1024, &dwNumOfModules);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (bResult == FALSE || hProcess == NULL || dwNumOfModules == 0) {
        return;
    }


    // 填充所有行数据
    for (int row = 0; row < dwNumOfModules; row++) {
        // 获取模块数据: 模块名称,模块地址
        TCHAR strBaseName[1024] = {0};
        TCHAR strFileName[1024] = {0};
        GetModuleBaseName(hProcess, hModules[row], strBaseName, 1024);   // 获取模块名
        GetModuleFileNameEx(hProcess, hModules[row], strFileName, 1024); // 获取模块文件路径

        LV_ITEM vitem = {0};
        vitem.mask = LVIF_TEXT;
        vitem.iItem = row;

        // 第一列(模块名称)
        vitem.pszText = strBaseName;
        vitem.iSubItem = 0;
        ListView_InsertItem(g_hwndListModules, &vitem);

        // 第二列(模块位置)
        vitem.pszText = strFileName;
        vitem.iSubItem = 1;
        ListView_SetItem(g_hwndListModules, &vitem);
        MODULEINFO mi;
        if (GetModuleInformation(hProcess, hModules[row], &mi, sizeof(MODULEINFO))) {
            vitem.pszText = strFileName;
            vitem.iSubItem = 1;
            ListView_SetItem(g_hwndListModules, &vitem);
        }
    }

    CloseHandle(hProcess);
}


void ShowDialogPe()
{
    // 1. 选择文件
    // 2. 加载PE

    // 弹出文件选择器
    TCHAR strFeFileExt[128] = TEXT("PE File(*.exe,*.dll,*.sys)\0*.exe;*.dll*;.sys\0")
        TEXT("All File(*.*)\0*.*\0\0");
    TCHAR strFileName[MAX_PATH] = {0};
    OPENFILENAMEW st = {0};
    st.lStructSize = sizeof(OPENFILENAME);
    st.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    st.hwndOwner = g_hwndMain;
    st.lpstrFilter = strFeFileExt;
    st.lpstrFile = strFileName;
    st.nMaxFile = MAX_PATH;

    if (GetOpenFileName(&st) == FALSE) {
        SetStaticMessage(TEXT("未选择文件!"));
        return;
    }

    // 加载PE文件
    PVOID pFileBuffer = NULL;
    if (ReadPeFile(strFileName, &pFileBuffer, &g_dwFileSize) == FALSE) {
        SetStaticMessage(TEXT("加载PE文件失败!"));
        return;
    }

    // 检查PE文件
    if (!CheckPeFile(pFileBuffer)) {
        SetStaticMessage(TEXT("文件不是有效的PE结构文件!"));
        free(pFileBuffer);
        return;
    }

    g_pFileBuffer = pFileBuffer;


    TCHAR buf[1024];
    wsprintf(buf, TEXT("已打开文件 %s"), strFileName);
    SetStaticMessage(buf);

    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_PE), g_hwndMain, DialogProcPe);

    if (g_pFileBuffer != NULL) {
        free(g_pFileBuffer); // PeDialog关闭后需要释放内存. 
    }

    g_pFileBuffer = NULL;
    g_dwFileSize = 0;
}


void ShowDialogShell()
{
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG_SHELL), g_hwndMain, DialogProcShell);
}


BOOL GetAllModules(DWORD dwPid, HMODULE* pModuel, int nSize, DWORD* dwNumOfModules)
{
    HANDLE hProcess;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (hProcess == NULL) {
        return FALSE;
    }

    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, pModuel, nSize, &cbNeeded) == FALSE) {
        CloseHandle(hProcess);
        return FALSE;
    }

    *dwNumOfModules = cbNeeded / sizeof(DWORD);
    return TRUE;
}


BOOL GetAllProcessId(DWORD* pPids, DWORD cb, PDWORD lpNumberOfPids)
{
    DWORD cbNeeded;
    if (!EnumProcesses(pPids, cb, &cbNeeded)) {
        return FALSE;
    }

    *lpNumberOfPids = cbNeeded / sizeof(DWORD);
    return TRUE;
}

BOOL GetMainModuleInfo(DWORD dwPid, MODULEINFO* mi)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if (hProcess == NULL) {
        return FALSE;
    }

    HMODULE hModules[1];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded) == FALSE) {
        CloseHandle(hProcess);
        return FALSE;
    }

    MODULEINFO info;
    DWORD cb;
    if (GetModuleInformation(hProcess, hModules[0], &info, &cb) == FALSE) {
        CloseHandle(hProcess);
        return FALSE;
    }

    *mi = info;
    CloseHandle(hProcess);
    return TRUE;
}


void SetEditText(DWORD dwEditId, PCTSTR buf)
{
    HWND hEdit = GetDlgItem(g_hwndDialogPe, MAKEINTRESOURCE(dwEditId));
    SetWindowText(hEdit, buf);
}


void SetStaticMessage(LPCTSTR str)
{
    HWND hwndStatic = GetDlgItem(g_hwndMain, IDC_STATIC1);
    SetWindowText(hwndStatic, str);
}

void CopyModulePath()
{
    DWORD dwRowId;
    TCHAR strPath[4096] = {0};

    dwRowId = SendMessage(g_hwndListModules, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
    if (dwRowId == -1) {
        return;
    }


    // 获取路径
    LV_ITEM lv = {0};
    lv.iSubItem = 1;
    lv.pszText = strPath;
    lv.cchTextMax = 4096;
    SendMessage(g_hwndListModules, LVM_GETITEMTEXT, dwRowId, (DWORD)&lv);

    TCHAR buf[1024];
    wsprintf(buf, TEXT("已复制 %s"), strPath);
    SetStaticMessage(buf);

    CopyTextToClipboard(strPath, sizeof(TCHAR));
}

BOOL CopyTextToClipboard(LPCTSTR strText)
{
    // 打开剪贴板   
    if (!OpenClipboard(NULL) || !EmptyClipboard()) {
        SetStaticMessage(TEXT("打开或清空剪切板出错"));
        return;
    }

    HGLOBAL hMen;

    // 分配全局内存    

    hMen = GlobalAlloc(GMEM_MOVEABLE, ((_tcslen(strText) + 1) * sizeof(TCHAR)));

    if (!hMen) {
        SetStaticMessage(TEXT("分配全局内存出错!"));
        // 关闭剪切板    
        CloseClipboard();
        return FALSE;
    }

    // 把数据拷贝考全局内存中   
    // 锁住内存区    
    LPTSTR lpStr = (LPTSTR)GlobalLock(hMen);

    // 内存复制   
    memcpy(lpStr, strText, ((_tcslen(strText)) * sizeof(TCHAR)));
    // 字符结束符    

    lpStr[_tcslen(strText)] = (TCHAR)0;
    // 释放锁    
    GlobalUnlock(hMen);

    // 把内存中的数据放到剪切板上   
    SetClipboardData(CF_UNICODETEXT, hMen);
    CloseClipboard();

    return;
}
