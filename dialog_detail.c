#include "dialog_detail.h"
#include <tchar.h>


TCHAR* resource_types[] = {
    TEXT("0"),            // 00h
    TEXT("CURSOR"),       // 01h
    TEXT("BITMAP"),       // 02h
    TEXT("ICON"),         // 03h
    TEXT("MENU"),         // 04h
    TEXT("DIALOG"),       // 05h
    TEXT("STRING"),       // 06h
    TEXT("FONTDIR"),      // 07h
    TEXT("FONT"),         // 08h
    TEXT("ACCELERATOR"),  // 09h
    TEXT("RCDATA"),       // 0Ah
    TEXT("MESSAGETABLE"), // 0Bh
    TEXT("GROUP_CURSOR"), // 0Ch
    TEXT(""),             // 0Dh
    TEXT("GROUP_ICON"),   // 0Eh
    TEXT(""),             // 0Fh
    TEXT("VERSION INFOMATION")       // 10h
};


BOOL CALLBACK DialogProcDetail(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = TRUE;
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hwndDialogDetail = hwndDlg;
        SendMessage(hwndDlg, g_dwShowID, NULL, NULL);

        // EDIT��굽��λ(Ĭ����ȫѡ״̬)
        PostMessage(GetDlgItem(hwndDlg, IDC_EDIT_DETAIL), EM_SETSEL, 0, 0);
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        break;

    case IDC_BUTTON_DIR_EXPORT:
        InitEditExport();
        break;

    case IDC_BUTTON_DIR_IMPORT:
        if (CheckPE64(g_pFileBuffer))
            InitEditImport64();
        else
            InitEditImport();
        break;

    case IDC_BUTTON_DIR_RESOURCE:
        InitEditResouce();
        break;

    case IDC_BUTTON_DIR_RELOCAL:
        InitEditRelocal();
        break;

    case IDC_BUTTON_DIR_BOUNDIMPORT:
        InitEditBoundImport();
        break;

    case IDC_BUTTON_DIR_IAT:
        InitEditIat();
        break;

    default:
        result = FALSE;
        break;
    }

    return result;
}


void InitEditExport()
{
    PIMAGE_EXPORT_DIRECTORY pExportDirectory = GetExportDirectory(g_pFileBuffer);
    if (pExportDirectory == NULL)
    {
        return;
    }

    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PSTR aName = (char*)(Rva2Foa(g_pFileBuffer, pExportDirectory->Name) + (DWORD)g_pFileBuffer);


    wsprintf(str += _tcslen(str), TEXT("============================= EXPORT_TABLE ============================= \r\n"));

    // ��ӡ�ֶ�
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("Characteristics"), pExportDirectory->Characteristics);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("TimeDateStamp"), pExportDirectory->TimeDateStamp);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%04x\r\n"), TEXT("MajorVersion"), pExportDirectory->MajorVersion);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%04x\r\n"), TEXT("MinorVersion"), pExportDirectory->MinorVersion);
#ifdef _UNICODE
    WCHAR wBuf[256] = { 0 };
    MultiByteToWideChar(CP_ACP, 0, aName, -1, wBuf, 256);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x  ->  \"%s\"\r\n"), TEXT("Name"), pExportDirectory->Name, wBuf);
#else
    // ANSI�汾ֱ��ʹ��
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x  ->  \"%s\"\r\n"), TEXT("Name"), pExportDirectory->Name, aName);
#endif // _UNICODE

    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("Base"), pExportDirectory->Base);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("NumberOfFunctions"), pExportDirectory->NumberOfFunctions);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("NumberOfNames"), pExportDirectory->NumberOfNames);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("AddressOfFunctions"), pExportDirectory->AddressOfFunctions);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("AddressOfNames"), pExportDirectory->AddressOfNames);
    wsprintf(str += _tcslen(str), TEXT("%-30s0x%08x\r\n"), TEXT("AddressOfNameOrdinals"), pExportDirectory->AddressOfNameOrdinals);

    // ��ӡ ��������/���/������ַ
    DWORD dw_NumberOfFunction = pExportDirectory->NumberOfFunctions;
    DWORD dw_NumberOfNames = pExportDirectory->NumberOfNames;
    DWORD dw_Base = pExportDirectory->Base;
    PWORD p_OrdinalTable = (PWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, pExportDirectory->AddressOfNameOrdinals));
    PDWORD p_NameTable = (PDWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, pExportDirectory->AddressOfNames));
    PDWORD p_FunctionTable = (PDWORD)((DWORD)g_pFileBuffer + Rva2Foa(g_pFileBuffer, pExportDirectory->AddressOfFunctions));

    wsprintf(str += _tcslen(str), TEXT("\r\n"));
    wsprintf(str += _tcslen(str), TEXT("%-8s%-10s%-15s%-10s\r\n"), TEXT(""), TEXT("Ordinal"), TEXT("Address(RVA)"), TEXT("Name"));
    PCHAR p_Name = NULL;
    for (DWORD i = 0; i < dw_NumberOfFunction; i++)
    {
        PCHAR buffer[128];
        wsprintfA(buffer, "%s", "-");
        p_Name = buffer;

        DWORD dw_Ordinal = i + dw_Base;
        DWORD dw_Address = *(p_FunctionTable + i);

        // ��ȡNameIndex
        for (DWORD j = 0; j < dw_NumberOfNames; j++)
        {
            if (*(p_OrdinalTable + j) == i)
            {
                DWORD dw_NameRva = *(p_NameTable + j);
                DWORD dw_NameFoa = Rva2Foa(g_pFileBuffer, dw_NameRva);
                wsprintfA(buffer, "\"%s\"", (PCHAR)((DWORD)g_pFileBuffer + dw_NameFoa));
                break;
            }
        }

        wsprintf(str += _tcslen(str), TEXT("%-8s"), TEXT(""));
#ifdef _UNICODE
        // Name��ANSI�ַ��� UNICODE��Ŀ,��Ҫת�ɿ��ַ�
        wchar_t wBuffer[128] = { 0 };
        MultiByteToWideChar(CP_ACP, 0, p_Name, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%-10.4x%-15.8x%s"), dw_Ordinal, dw_Address, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%-10.4x%-15.8x%s"), dw_Ordinal, dw_Address, p_Name);
#endif // _UNICODE

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    wsprintf(str += _tcslen(str), TEXT("\r\n"));


    // �޸�Edit�ؼ��ı�
    SendDlgItemMessage(g_hwndDialogDetail, IDC_EDIT_DETAIL, WM_SETTEXT, 0, buffer);

    free(buffer);
}

void InitEditImport64()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    DWORD  numberOfDescriptors = GetImportDescriptorNumbers(g_pFileBuffer);

    for (DWORD importDescriptorIndex = 0; importDescriptorIndex < numberOfDescriptors; importDescriptorIndex++)
    {
        PIMAGE_IMPORT_DESCRIPTOR p_ImportDescriptor = GetImportDescriptor(g_pFileBuffer, importDescriptorIndex);

        DWORD nameRva = p_ImportDescriptor->Name;
        DWORD nameFoa = Rva2Foa(g_pFileBuffer, nameRva);
        PCHAR p_DllName = (PCHAR)((DWORD)g_pFileBuffer + nameFoa);

        wsprintf(str += _tcslen(str), TEXT("============================= IMPORT_DESCRIPTOR[%d] ============================= \r\n"), importDescriptorIndex);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("OriginalFirstThunk"), p_ImportDescriptor->OriginalFirstThunk);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("TimeDateStamp"), p_ImportDescriptor->TimeDateStamp);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("ForwarderChain"), p_ImportDescriptor->ForwarderChain);
#ifdef _UNICODE
        MultiByteToWideChar(CP_ACP, 0, p_DllName, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x      \"%-s\"\r\n"), TEXT("Name"), p_ImportDescriptor->Name, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x      \"%-s\"\r\n"), TEXT("Name"), p_ImportDescriptor->Name, p_DllName);
#endif // _UNICODE
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("FirstThunk"), p_ImportDescriptor->FirstThunk);

        DWORD originalFirstThunk = p_ImportDescriptor->OriginalFirstThunk;
        // �ж����λ(D15)�Ƿ����0.  ����0��ʾ��ֵ��RVA
        // ����INT��FOA
        DWORD intFoa = Rva2Foa(g_pFileBuffer, originalFirstThunk);

        PIMAGE_THUNK_DATA64 p_Int = (PIMAGE_THUNK_DATA64)((DWORD)g_pFileBuffer + intFoa);
        // ��ӡINT��
        wsprintf(str += _tcslen(str), TEXT("%-30s%-30s%-15s%s\r\n"), TEXT("INT"), TEXT("[NO]AddressOfData"), TEXT("Hint/Ordinal"), TEXT("Name"));

        for (DWORD i = 0; p_Int->u1.AddressOfData != 0; p_Int++, i++)  // INT�������ʽ����һ��ȫ��ṹ��β. ���ȡ����ֵ��0���ʾ�ѵ���.
        {
            // �����ʽ�Ű�
            wsprintf(str += _tcslen(str), TEXT("%30s"), TEXT(" "));

            // ���INT��ַ
            wsprintf(str += _tcslen(str), TEXT("[%04d]%016x%8s"), i, p_Int->u1.AddressOfData, TEXT(""));

            if (p_Int->u1.AddressOfData >> 63 == 0)
            {
                // �� PIMAGE_THUNK_DATA��D31Ϊ0��ʾ����ָ��Hint/Name��RVA.

                // ����Hint/Name�ṹ��ַ
                DWORD importHintNameRva = p_Int->u1.AddressOfData;
                DWORD importHintNameFoa = Rva2Foa(g_pFileBuffer, importHintNameRva);
                PIMAGE_IMPORT_BY_NAME p_ImportHintName = (PIMAGE_IMPORT_BY_NAME)((DWORD)g_pFileBuffer + importHintNameFoa);

#ifdef _UNICODE
                MultiByteToWideChar(CP_ACP, 0, p_ImportHintName->Name, -1, wBuffer, 128);
                wsprintf(str += _tcslen(str), TEXT("%04x%11s\"%s\""), p_ImportHintName->Hint, TEXT(""), wBuffer);
#else
                wsprintf(str += _tcslen(str), TEXT("%04x%11s\"%s\""), p_ImportHintName->Hint, p_ImportHintName->Name);
#endif

            }
            else
            {
                // �� PIMAGE_THUNK_DATA��D31Ϊ1��ʾ����31�Ǻ������
                ULONGLONG dw_Ordinal = p_Int->u1.Ordinal & 0x7FFFFFFFFFFFFFFF;
                wsprintf(str += _tcslen(str), TEXT("%04x%11s%s"), dw_Ordinal, TEXT(""), TEXT("-"));
            }

            // ����
            wsprintf(str += _tcslen(str), TEXT("\r\n"));
        }
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        DWORD firstThunk = p_ImportDescriptor->FirstThunk;
        DWORD iatFoa = Rva2Foa(g_pFileBuffer, firstThunk);

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    wsprintf(str += _tcslen(str), TEXT("\r\n"));



    SendDlgItemMessage(g_hwndDialogDetail, IDC_EDIT_DETAIL, WM_SETTEXT, 0, (PVOID)buffer);
    free(buffer);
}


void InitEditImport()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    DWORD  numberOfDescriptors = GetImportDescriptorNumbers(g_pFileBuffer);

    for (DWORD importDescriptorIndex = 0; importDescriptorIndex < numberOfDescriptors; importDescriptorIndex++)
    {
        PIMAGE_IMPORT_DESCRIPTOR p_ImportDescriptor = GetImportDescriptor(g_pFileBuffer, importDescriptorIndex);

        DWORD nameRva = p_ImportDescriptor->Name;
        DWORD nameFoa = Rva2Foa(g_pFileBuffer, nameRva);
        PCHAR p_DllName = (PCHAR)((DWORD)g_pFileBuffer + nameFoa);

        wsprintf(str += _tcslen(str), TEXT("============================= IMPORT_DESCRIPTOR[%d] ============================= \r\n"), importDescriptorIndex);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("OriginalFirstThunk"), p_ImportDescriptor->OriginalFirstThunk);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("TimeDateStamp"), p_ImportDescriptor->TimeDateStamp);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("ForwarderChain"), p_ImportDescriptor->ForwarderChain);
#ifdef _UNICODE
        MultiByteToWideChar(CP_ACP, 0, p_DllName, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x      \"%-s\"\r\n"), TEXT("Name"), p_ImportDescriptor->Name, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x      \"%-s\"\r\n"), TEXT("Name"), p_ImportDescriptor->Name, p_DllName);
#endif // _UNICODE
        wsprintf(str += _tcslen(str), TEXT("%-30s%08x\r\n"), TEXT("FirstThunk"), p_ImportDescriptor->FirstThunk);

        DWORD originalFirstThunk = p_ImportDescriptor->OriginalFirstThunk;
        // �ж����λ(D15)�Ƿ����0.  ����0��ʾ��ֵ��RVA
        // ����INT��FOA
        DWORD intFoa = Rva2Foa(g_pFileBuffer, originalFirstThunk);

        PIMAGE_THUNK_DATA32 p_Int = (PIMAGE_THUNK_DATA32)((DWORD)g_pFileBuffer + intFoa);
        // ��ӡINT��
        wsprintf(str += _tcslen(str), TEXT("%-30s%-22s%-15s%s\r\n"), TEXT("INT"), TEXT("[NO]AddressOfData"), TEXT("Hint/Ordinal"), TEXT("Name"));

        for (DWORD i = 0; p_Int->u1.AddressOfData != 0; p_Int++, i++)  // INT�������ʽ����һ��ȫ��ṹ��β. ���ȡ����ֵ��0���ʾ�ѵ���.
        {
            // �����ʽ�Ű�
            wsprintf(str += _tcslen(str), TEXT("%30s"), TEXT(" "));

            // ���INT��ַ
            wsprintf(str += _tcslen(str), TEXT("[%04d]%08x%8s"), i, p_Int->u1.AddressOfData, TEXT(""));

            if (p_Int->u1.AddressOfData >> 31 == 0)
            {
                // �� PIMAGE_THUNK_DATA��D31Ϊ0��ʾ����ָ��Hint/Name��RVA.

                // ����Hint/Name�ṹ��ַ
                DWORD importHintNameRva = p_Int->u1.AddressOfData;
                DWORD importHintNameFoa = Rva2Foa(g_pFileBuffer, importHintNameRva);
                PIMAGE_IMPORT_BY_NAME p_ImportHintName = (PIMAGE_IMPORT_BY_NAME)((DWORD)g_pFileBuffer + importHintNameFoa);

#ifdef _UNICODE
                MultiByteToWideChar(CP_ACP, 0, p_ImportHintName->Name, -1, wBuffer, 128);
                wsprintf(str += _tcslen(str), TEXT("%04x%11s\"%s\""), p_ImportHintName->Hint, TEXT(""), wBuffer);
#else
                wsprintf(str += _tcslen(str), TEXT("%04x%11s\"%s\""), p_ImportHintName->Hint, p_ImportHintName->Name);
#endif

            }
            else
            {
                // �� PIMAGE_THUNK_DATA��D31Ϊ1��ʾ����31�Ǻ������
                DWORD dw_Ordinal = p_Int->u1.Ordinal & 0x7FFFFFFF;
                wsprintf(str += _tcslen(str), TEXT("%04x%11s%s"), dw_Ordinal, TEXT(""), TEXT("-"));
            }

            // ����
            wsprintf(str += _tcslen(str), TEXT("\r\n"));
        }
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        DWORD firstThunk = p_ImportDescriptor->FirstThunk;
        DWORD iatFoa = Rva2Foa(g_pFileBuffer, firstThunk);

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }

    wsprintf(str += _tcslen(str), TEXT("\r\n"));



    SendDlgItemMessage(g_hwndDialogDetail, IDC_EDIT_DETAIL, WM_SETTEXT, 0, (PVOID)buffer);
    free(buffer);
}


/* �ݹ������ԴĿ¼ */
VOID  PrintfResrouceDirectoryRe(DWORD dw_Base, PIMAGE_RESOURCE_DIRECTORY p_ResourceDirectory, DWORD dw_Depth, PTSTR str)
{
    // Entry����
    DWORD dw_NumberEntries = p_ResourceDirectory->NumberOfIdEntries + p_ResourceDirectory->NumberOfNamedEntries;
    for (DWORD i = 0; i < dw_NumberEntries; i++)
    {
        if (dw_Depth == 0)
        {
            wsprintf(str += _tcslen(str), TEXT("\r\n|-"));
        }
        if (dw_Depth == 1)
        {
            wsprintf(str += _tcslen(str), TEXT("\r\n|----"));
        }

        PIMAGE_RESOURCE_DIRECTORY_ENTRY p_ResourceDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)p_ResourceDirectory + sizeof(IMAGE_RESOURCE_DIRECTORY)+i*sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
        if ((p_ResourceDirEntry->Name & 0x80000000) == 0x80000000)
        {
            // Name�ֶ����λΪ1 : ��ʾ�ֶε�λ��Ϊָ��ʹ��.
            DWORD dw_Offset = p_ResourceDirEntry->Name & 0x7FFFFFFF;
            PIMAGE_RESOURCE_DIR_STRING_U p_DirString = (PIMAGE_RESOURCE_DIR_STRING_U)(dw_Base + dw_Offset);
            DWORD length = p_DirString->Length;
            wchar_t* str = p_DirString->NameString;
            wsprintf(str += _tcslen(str), TEXT("%-10.*s"), length, str);
        }
        else
        {
            // Name�ֶ����λΪ0 : ��ʾ�ֶ���ΪIDʹ��
            if (dw_Depth > 1)
            {
                wsprintf(str += _tcslen(str), TEXT("ID:%x  "), p_ResourceDirEntry->Name);
            }
            else
            {
                if (dw_Depth == 0 && p_ResourceDirEntry->Name < sizeof(resource_types) / sizeof(CHAR*))
                {
                    wsprintf(str += _tcslen(str), TEXT("%-10s"), resource_types[p_ResourceDirEntry->Name]);
                }
                else
                {
                    wsprintf(str += _tcslen(str), TEXT("%-10x"), p_ResourceDirEntry->Name);
                }
            }
        }

        if ((p_ResourceDirEntry->OffsetToData & 0x80000000) == 0x80000000)
        {
            // OffsetToData�ֶ����λΪ1 :��ʾ��λ����ָ����һ��Ŀ¼��
            DWORD offset = p_ResourceDirEntry->OffsetToData & 0x7FFFFFFF;
            // ������һ��Ŀ¼�ṹָ��
            PIMAGE_RESOURCE_DIRECTORY p_NextResourceDir = (PIMAGE_RESOURCE_DIRECTORY)(dw_Base + offset);

            // �ݹ������һ��
            PrintfResrouceDirectoryRe(dw_Base, p_NextResourceDir, dw_Depth + 1, str);
        }
        else
        {
            // OffsetToData�ֶ����λΪ0 : ��ʾֵ��ָ��IMAGE_RESOURCE_DATA_ENTRY�Ľṹ��ָ��
            DWORD dw_Offset = p_ResourceDirEntry->OffsetToData;
            PIMAGE_RESOURCE_DATA_ENTRY p_DataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(dw_Base + dw_Offset);
            wsprintf(str += _tcslen(str), TEXT("RVA:%x  Size:%x"), p_DataEntry->OffsetToData, p_DataEntry->Size);
        }
    }
}


void InitEditResouce()
{
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    // ��ӡ�����ֶ�
    wsprintf(str += _tcslen(str), TEXT("============================= RESOURCE_DIRECTORY ============================= \r\n"));

    PIMAGE_RESOURCE_DIRECTORY p_ResourceDirectory = GetResourceDirectory(g_pFileBuffer);
    if (p_ResourceDirectory == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("��Դ��Ϊ��!\r\n"));
        return;
    }

    // ��Դ�����ַ
    DWORD dw_ResourceBase = (DWORD)p_ResourceDirectory;

    // �ݹ����;
    PrintfResrouceDirectoryRe(dw_ResourceBase, p_ResourceDirectory, 0, str);
    wsprintf(str += _tcslen(str), TEXT("\r\n"));
    wsprintf(str += _tcslen(str), TEXT("\r\n"));

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_DETAIL);
    SetWindowText(hwndEdit, buffer);
    free(buffer);
}

void InitEditBoundImport()
{
    wchar_t wBuffer[128] = { 0 };
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_BOUND_IMPORT_DESCRIPTOR p_BoundImport = GetBoundImportDescriptor(g_pFileBuffer);
    if (p_BoundImport == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("�󶨵����Ϊ��!\r\n"));
        return;
    }

    // �󶨵�������ַ. dll�������������ַ��ƫ��. 
    DWORD bountImportBase = (DWORD)p_BoundImport;

    // �սṹ��. ������Ϊ����������ж�. 
    IMAGE_BOUND_IMPORT_DESCRIPTOR nullBoundImport = { 0 };

    for (int i = 0; (memcmp(p_BoundImport, &nullBoundImport, sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) != 0); i++)
    {
        PCHAR name = (PCHAR)(+bountImportBase + p_BoundImport->OffsetModuleName);

        wsprintf(str += _tcslen(str), TEXT("============================= BOUND_IMPORT[%d] ============================= \r\n"), i);
        wsprintf(str += _tcslen(str), TEXT("%-20s%08x\r\n"), TEXT("TimeDateStamp"), p_BoundImport->TimeDateStamp);
        wsprintf(str += _tcslen(str), TEXT("%-20s%08x\r\n"),TEXT("Number"), p_BoundImport->NumberOfModuleForwarderRefs);

#ifdef _UNICODE
        MultiByteToWideChar(CP_ACP, 0, name, -1, wBuffer, 128);
        wsprintf(str += _tcslen(str), TEXT("%-20s0%4x     \"%s\""),TEXT("OffsetModuleName"), p_BoundImport->OffsetModuleName, wBuffer);
#else
        wsprintf(str += _tcslen(str), TEXT("%-20s%04x     \"%s\""),TEXT("OffsetModuleName"), p_BoundImport->OffsetModuleName, name);
#endif // _UNICODE


        PIMAGE_BOUND_FORWARDER_REF p_BoundRef = (PIMAGE_BOUND_FORWARDER_REF)(p_BoundImport + 1);
        for (DWORD i = 0; i < p_BoundImport->NumberOfModuleForwarderRefs; i++)
        {
            name = (PCHAR)(+bountImportBase + p_BoundRef->OffsetModuleName);
            wsprintf(str += _tcslen(str), TEXT("\r\n"));
            wsprintf(str += _tcslen(str), TEXT("%8s"), TEXT(""));
#ifdef _UNICODE
            MultiByteToWideChar(CP_ACP, 0, name, -1, wBuffer, 128);
            wsprintf(str += _tcslen(str), TEXT("TimeDateStamp:%08x\tName:%04x -> \"%s\"    Reserved:%04x"), p_BoundRef->TimeDateStamp, p_BoundRef->OffsetModuleName, wBuffer, p_BoundRef->Reserved);
#else
            wsprintf(str += _tcslen(str), TEXT("TimeDateStamp:%08x\tName:%04x -> \"%s\"    Reserved:%04x"), p_BoundRef->TimeDateStamp, p_BoundRef->OffsetModuleName, name, p_BoundRef->Reserved);
#endif // _UNICODE
            p_BoundRef++;
        }

        p_BoundImport += p_BoundImport->NumberOfModuleForwarderRefs;
        p_BoundImport++;

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));
    }




    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_DETAIL);
    SetWindowText(hwndEdit, buffer);
    free(buffer);
}

void InitEditIat()
{
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_DATA_DIRECTORY pDataDirectory = GetDataDirectory(g_pFileBuffer, IMAGE_DIRECTORY_ENTRY_IAT);
    if (pDataDirectory->VirtualAddress == 0 || pDataDirectory->Size == 0)
    {
        wsprintf(str += _tcslen(str), TEXT("û��IAT��\r\n"));
        return;
    }

    DWORD iatRva = pDataDirectory->VirtualAddress;
    DWORD iatFoa = Rva2Foa(g_pFileBuffer, iatRva);
    PDWORD pIat = (PDWORD)((DWORD)g_pFileBuffer + iatFoa);

    DWORD length = (pDataDirectory->Size) / sizeof(DWORD);
    for (size_t index = 0; index < length; index++)
    {
        wsprintf(str += _tcslen(str), TEXT("[%04d] %08x\r\n"), index, pIat[index]);
    }

    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_DETAIL);
    SetWindowText(hwndEdit, buffer);
    free(buffer);
}

void InitEditRelocal()
{
    PVOID buffer = malloc(1024 * 1024 * 1);
    memset(buffer, 0, 1024 * 1024 * 1);
    PTSTR str = (PTSTR)buffer;

    PIMAGE_BASE_RELOCATION p_Relocation = GetRelocation(g_pFileBuffer);
    if (p_Relocation == NULL)
    {
        wsprintf(str += _tcslen(str), TEXT("û���ض�λ��\r\n"));
        return;
    }

    for (DWORD i = 1;; i++)
    {
        // �ض�λ����ȫ�����ݱ�ʾ��β.  
        if (*(PDWORD)p_Relocation == 0)
        {
            break;
        }

        wsprintf(str += _tcslen(str), TEXT("============================= RELOCATION[%d] ============================= \r\n"), i);
        wsprintf(str += _tcslen(str), TEXT("%-16s%08x%6s\r\n"), TEXT("VirtualAddress"), p_Relocation->VirtualAddress, TEXT(""));
        wsprintf(str += _tcslen(str), TEXT("%-16s%08x%6s\r\n"), TEXT("SizeOfBlock"), p_Relocation->SizeOfBlock, TEXT(""));

        wsprintf(str += _tcslen(str), TEXT("%-16s\r\n"), TEXT("Offset(Type)"));

        // �ض�λ��-����
        DWORD dw_Items = (p_Relocation->SizeOfBlock - 8) / sizeof(WORD);
        for (DWORD i = 0; i < dw_Items; i++)
        {
            if (i % 4 == 0)
            {
                // ÿһ���Ű�
                wsprintf(str += _tcslen(str), TEXT("%16s"), TEXT(" "));
            }


            PWORD pw_TypeOffset = (PWORD)((DWORD)p_Relocation + 8 + i*sizeof(WORD));
            WORD w_Type = *pw_TypeOffset >> 12;
            WORD w_Offset = *pw_TypeOffset & 0x0FFF;
            wsprintf(str += _tcslen(str), TEXT("%04x(%04x)%8s"), w_Offset, w_Type, TEXT(""));


            if (i != 0 && (i + 1) % 4 == 0)
            {
                // ����
                wsprintf(str += _tcslen(str), TEXT("\r\n"));
            }

        }

        wsprintf(str += _tcslen(str), TEXT("\r\n"));
        wsprintf(str += _tcslen(str), TEXT("\r\n"));

        p_Relocation = (PIMAGE_BASE_RELOCATION)((DWORD)p_Relocation + p_Relocation->SizeOfBlock);
    };



    HWND hwndEdit = GetDlgItem(g_hwndDialogDetail, IDC_EDIT_DETAIL);
    SetWindowText(hwndEdit, buffer);
    free(buffer);
}

