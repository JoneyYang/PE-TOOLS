#include "dialog_main.h"
#include "main.h"

int WINAPI _tWinMain(
    HINSTANCE hInstance,      // handle to current instance
    HINSTANCE hPrevInstance,  // handle to previous instance
    LPSTR lpCmdLine,          // command line
    int nCmdShow              // show state
    )
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    g_hInstance = hInstance;

    // 显示Dialog对话框.
    DialogBox(hInstance,                    // 进程句柄
        MAKEINTRESOURCE(IDD_DIALOG_MAIN),   // 资源ID
        NULL,               // 父窗口
        DialogProcMain);    // Dialog处理函数
}
