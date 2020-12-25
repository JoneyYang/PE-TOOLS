#include "dialog_about.h"


HBITMAP hBitmap = NULL;

void InitDialogAbout()
{

    hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
    HWND hStatic = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 10, 10, 150, 150, g_hwndDialogAbout, (HMENU)10000, g_hInstance, NULL);
    SendMessage(hStatic, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

}

BOOL CALLBACK DialogProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogAbout = hwndDlg;
        InitDialogAbout();
        break;

    case WM_CLOSE:
        if (hBitmap != NULL) DeleteObject((HBITMAP)hBitmap);
        EndDialog(hwndDlg, 0);
        break;

    default:
        result = FALSE;
        break;
    }

    return result;
}
