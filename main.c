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

    // ��ʾDialog�Ի���.
    DialogBox(hInstance,                    // ���̾��
        MAKEINTRESOURCE(IDD_DIALOG_MAIN),   // ��ԴID
        NULL,               // ������
        DialogProcMain);    // Dialog������
}
