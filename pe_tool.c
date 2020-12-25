#define _CRT_SECURE_NO_WARNINGS

#define OFFSET(Type, member) (DWORD)&( ((Type*)0)->member) )

#include "pe_tool.h"

/* ��ȡDOSͷָ�� */
PIMAGE_DOS_HEADER GetDosHeader(const PVOID pFileBuffer)
{
    // �ļ����׵�ַ ����DOSͷ��ƫ��
    return (PIMAGE_DOS_HEADER)pFileBuffer;
}

/* ��ȡ��׼PEͷ */
PIMAGE_FILE_HEADER GetFileHeader(const PVOID pFileBuffer)
{
    // ��ȡDOSͷ��Ϣ
    PIMAGE_DOS_HEADER p_DosHeader = GetDosHeader(pFileBuffer);
    LONG e_lfanew = p_DosHeader->e_lfanew;

    // ƫ�� : e_lfanew + 4���ֽڵ�PEͷ��ʶ  
    DWORD offset = e_lfanew + 0x4;

    return (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + offset);
}

/* ��ȡ��չPEͷ(��ѡPEͷ) */
PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID pFileBuffer)
{
    // ��ȡ��׼PEͷָ��
    PIMAGE_FILE_HEADER p_FileHeader = GetFileHeader(pFileBuffer);

    // ��ѡPEͷ�����ű�׼PEͷ, ���ƫ�ƾ��� sizeof(IMAGE_FILE_HEADER)
    DWORD offsetByFileHeader = sizeof(IMAGE_FILE_HEADER);

    return (PIMAGE_OPTIONAL_HEADER32)((DWORD)p_FileHeader + offsetByFileHeader);
}

PIMAGE_OPTIONAL_HEADER64 GetOptionalHeader64(const PVOID pFileBuffer)
{
    return (PIMAGE_OPTIONAL_HEADER64)GetOptionalHeader(pFileBuffer);
}

/* ��ȡ�ڱ����� */
DWORD GetSectionNumbers(const PVOID pFileBuffer)
{
    // ͨ����׼PEͷ���Ի�ȡ���ڱ�����
    return GetFileHeader(pFileBuffer)->NumberOfSections;
}


/* ��ȡ�ڱ� */
PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID pFileBuffer, DWORD index)
{
    // ��ȡ��ѡPEͷ
    PIMAGE_OPTIONAL_HEADER p_OptionalHeader = GetOptionalHeader(pFileBuffer);

    // ͨ����׼PEͷ��ȡ��ѡPEͷ��С
    DWORD offsetByOptionalHeader = (GetFileHeader(pFileBuffer)->SizeOfOptionalHeader);

    // ����ƫ��
    DWORD offsetByIndex = index * sizeof(IMAGE_SECTION_HEADER);

    return  (PIMAGE_SECTION_HEADER)((DWORD)p_OptionalHeader + offsetByOptionalHeader + offsetByIndex);
}


/* ��ȡ����Ŀ¼ */
PIMAGE_DATA_DIRECTORY GetDataDirectory(const PVOID pFileBuffer, DWORD index)
{

    if (CheckPE64(pFileBuffer))
    {
        PIMAGE_OPTIONAL_HEADER64 p_OptionalHeader = GetOptionalHeader64(pFileBuffer);
        PIMAGE_DATA_DIRECTORY p_DataDirectory = p_OptionalHeader->DataDirectory;
        return p_DataDirectory + index;
    }
    else
    {
        PIMAGE_OPTIONAL_HEADER32 p_OptionalHeader = GetOptionalHeader(pFileBuffer);
        PIMAGE_DATA_DIRECTORY p_DataDirectory = p_OptionalHeader->DataDirectory;
        return p_DataDirectory + index;
    }

}

/* ��ȡ����� */
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID pFileBuffer, DWORD index)
{
    // ��ȡ���������Ŀ¼
    PIMAGE_DATA_DIRECTORY p_DateDirectoryEntryImport = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_IMPORT);

    if (p_DateDirectoryEntryImport->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD importDescriptorRva = p_DateDirectoryEntryImport->VirtualAddress;
    DWORD importDescriptorFoa = Rva2Foa(pFileBuffer, importDescriptorRva);

    return (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + importDescriptorFoa) + index;
}

/* ��ȡ�󶨵���� */
PIMAGE_BOUND_IMPORT_DESCRIPTOR GetBoundImportDescriptor(const PVOID pFileBuffer)
{
    PIMAGE_DATA_DIRECTORY p_DateDirectoryBoundImport = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);

    if (p_DateDirectoryBoundImport->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD boundImportRva = p_DateDirectoryBoundImport->VirtualAddress;
    DWORD boundImportFoa = Rva2Foa(pFileBuffer, boundImportRva);

    return (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + boundImportFoa);
}

/* ��ȡ������ */
PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID pFileBuffer)
{
    // ��ȡ����������Ŀ¼
    PIMAGE_DATA_DIRECTORY p_ExportDataDirectory = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_EXPORT);

    if (p_ExportDataDirectory->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD exportDirectoryRva = p_ExportDataDirectory->VirtualAddress;
    DWORD exportDirectoryFoa = Rva2Foa(pFileBuffer, exportDirectoryRva);

    return (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + exportDirectoryFoa);
}


/* ��ȡ��Դ��Ŀ¼*/
PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory(const PVOID pFileBuffer)
{
    PIMAGE_DATA_DIRECTORY p_ResourceDataDirectory = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_RESOURCE);
    if (p_ResourceDataDirectory->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD dw_ResourceRva = p_ResourceDataDirectory->VirtualAddress;
    DWORD dw_ResourceFoa = Rva2Foa(pFileBuffer, dw_ResourceRva);

    return (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pFileBuffer + dw_ResourceFoa);

}

/* ��ȡͷ������λ��(�ڱ�β����ʼ)*/
PVOID GetHeaderAvailble(const PVOID pFileBuffer)
{
    PIMAGE_SECTION_HEADER pLastSectionHeader = GetSectionHeader(pFileBuffer, GetSectionNumbers(pFileBuffer) - 1);
    return (PVOID)((DWORD)pLastSectionHeader + sizeof(IMAGE_SECTION_HEADER));
}

/* ��ȡ�ض�λ�� */
PIMAGE_BASE_RELOCATION GetRelocation(const PVOID pFileBuffer)
{
    PIMAGE_DATA_DIRECTORY p_BaserelocDataDirectory = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (p_BaserelocDataDirectory->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD dw_RelocationRva = p_BaserelocDataDirectory->VirtualAddress;
    DWORD dw_RelocationFoa = Rva2Foa(pFileBuffer, dw_RelocationRva);

    return (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + dw_RelocationFoa);
}


/* RVRתFOA */
DWORD Rva2Foa(const PVOID pFileBuffer, DWORD rva)
{
    int numberOfSections = GetSectionNumbers(pFileBuffer);
    PIMAGE_SECTION_HEADER firstSection = GetSectionHeader(pFileBuffer, 0);

    // �õ�һ���ڱ�ĵ�ַ�ж�RVA�Ƿ����ڽ�����.
    if (rva < firstSection->VirtualAddress)
    {
        // ��ǰ�������RVA��FOA����ͬ��. ֱ�ӷ���. 
        return rva;
    }

    // �ж�RVA�����ĸ�����
    PIMAGE_SECTION_HEADER targetSection = NULL;
    for (int i = numberOfSections - 1; i >= 0; i--)
    {
        targetSection = GetSectionHeader(pFileBuffer, i);

        if (rva >= targetSection->VirtualAddress)
        {
            break;
        }
    }

    // ����ƫ��
    DWORD offset = targetSection->VirtualAddress - targetSection->PointerToRawData;  // ��������ڵ�ƫ��, ֻҪ��ͬһ������, �ļ���������ImageBuffer�����ݵ�ƫ�ƶ���һ����. 
    DWORD foa = rva - offset;
    return foa;
}

DWORD Foa2Rva(const PVOID pFileBuffer, DWORD foa)
{
    DWORD numberOfSections = GetSectionNumbers(pFileBuffer);
    PIMAGE_SECTION_HEADER firstSection = GetSectionHeader(pFileBuffer, 0);

    // �õ�һ���ڱ�ĵ�ַ�ж�RVA�Ƿ����ڽ�����.
    if (foa < firstSection->PointerToRawData)
    {
        // ��ǰ�������RVA��FOA����ͬ��. ֱ�ӷ���. 
        return foa;
    }

    // �ж�RVA�����ĸ�����
    PIMAGE_SECTION_HEADER targetSection = NULL;
    for (size_t i = numberOfSections - 1; i >= 0; i--)
    {
        targetSection = GetSectionHeader(pFileBuffer, i);

        if (foa >= targetSection->PointerToRawData)
        {
            break;
        }
    }

    // ����ƫ��
    DWORD offset = targetSection->VirtualAddress - targetSection->PointerToRawData;  // ��������ڵ�ƫ��, ֻҪ��ͬһ������, �ļ���������ImageBuffer�����ݵ�ƫ�ƶ���һ����. 
    DWORD rva = foa + offset;
    return rva;


}

/*
��������Ĵ�С
unalignedSize Ϊ����Ĵ�С
aligenment �������. ��С����ֵ����
*/
DWORD AlignedSize(DWORD unalignedSize, DWORD aligenment)
{
    if ((unalignedSize % aligenment) == 0)
    {
        return unalignedSize;
    }
    else
    {
        return ((unalignedSize / aligenment) + 1) * aligenment;
    }
}

/* ��ȡ��������� */
DWORD GetImportDescriptorNumbers(const PVOID pFileBuffer)
{
    DWORD countOfImportDescriptor = 0;

    PIMAGE_IMPORT_DESCRIPTOR p_ImportDescriptor = GetImportDescriptor(pFileBuffer, 0);
    if (p_ImportDescriptor == NULL)
    {
        return 0;
    }


    IMAGE_IMPORT_DESCRIPTOR emptyIID = { 0 };
    while (TRUE)
    {
        if (memcmp(p_ImportDescriptor, &emptyIID, sizeof(IMAGE_IMPORT_DESCRIPTOR)) == 0)
        {
            return countOfImportDescriptor;
        }
        else
        {
            countOfImportDescriptor++;
        }

        p_ImportDescriptor++;
    }
}


BOOL CheckPeFile(const PVOID pFileBuffer)
{
    if (pFileBuffer == NULL)
    {
        return FALSE;
    }

    PIMAGE_DOS_HEADER p_DosHeader = GetDosHeader(pFileBuffer);
    if (p_DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return FALSE;
    }

    PIMAGE_NT_HEADERS p_NtHeaders = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + p_DosHeader->e_lfanew);
    if (p_NtHeaders->Signature != IMAGE_NT_SIGNATURE)
    {
        return FALSE;
    }
    return TRUE;
}


/* �鿴�Ƿ�ΪPE32+ (64λPE)*/
BOOL CheckPE64(const PVOID pFileBuffer)
{
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(pFileBuffer);
    return pOptionalHeader->Magic == 0x020B;        // ����ѡͷ�Ľṹ�� IMAGE_OPTIONAL_HEADER64ʱ Magicֵ�� 0x020B
}

/* ����DOS-STUB��С*/
DWORD GetDosStubSize(const PVOID pFileBuffer)
{
    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBuffer);
    DWORD dwLfanew = pDosHeader->e_lfanew;

    if (dwLfanew <= sizeof(IMAGE_DOS_HEADER))
    {
        return 0;
    }

    return dwLfanew - sizeof(IMAGE_DOS_HEADER);
}

/* ����ͷ�����������ռ��С(һ�������ݻ����ͷ��, ��Ҫ�ж�)*/
DWORD GetHeaderAvailableSize(const PVOID pFileBuffer)
{
    // ��ȡ�ǰ���ݵ�offset
    DWORD dwHeadDataOffset = GetOptionalHeader(pFileBuffer)->SizeOfHeaders;
    for (size_t i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
    {
        PIMAGE_DATA_DIRECTORY pDataDirectory = GetDataDirectory(pFileBuffer, i);
        if (pDataDirectory->VirtualAddress == 0)
        {
            continue;
        }

        if (pDataDirectory->VirtualAddress < dwHeadDataOffset)
        {
            dwHeadDataOffset = pDataDirectory->VirtualAddress;
        }
    }

    // ��ȡ�ڱ�β����offset
    DWORD dwAvailableOffset = (DWORD)(GetHeaderAvailble(pFileBuffer)) - (DWORD)pFileBuffer;

    return dwHeadDataOffset - dwAvailableOffset;
}

/*
��ȡ�ļ����ڴ�
IN lp_FilePath �ļ�·��
OUT pFileBuffer FileBuffer
OUT bufferSize �ļ���С(FileBuffer)��С
*/
BOOL ReadPeFile(LPTSTR lp_FilePath, OUT PVOID* pFileBuffer, OUT DWORD* bufferSize)
{
    PVOID p_newBuffer = NULL;
    DWORD fileSize = 0;
    FILE* pFile = NULL;


    if (lp_FilePath == NULL)
    {
        printf("�ļ�·��Ϊ��\n");
        return FALSE;
    }

    pFile = _tfopen(lp_FilePath, TEXT("rb"));
    if (!pFile)
    {
        printf("���ļ�ʧ��!");
        return FALSE;
    }

    // ������ƫ�Ƶ��ļ���β
    fseek(pFile, 0, SEEK_END);
    // ��ȡ��ǰ����ƫ��λ��(�ļ���βλ��=�ļ���С)
    fileSize = ftell(pFile);
    // ������ƫ�Ƶ��ļ���ͷ
    fseek(pFile, 0, SEEK_SET);

    // �����ڴ�ռ�
    p_newBuffer = malloc(fileSize);
    if (!p_newBuffer)
    {
        printf("����ռ�ʧ��!");
        fclose(pFile);
        return FALSE;
    }

    // ��ȡ�ļ�����, ����0��ʾ�ɹ�
    size_t n = fread(p_newBuffer, fileSize, 1, pFile);
    if (!n)
    {
        printf("��ȡ����ʧ��!");
        free(p_newBuffer);
        fclose(pFile);
        return FALSE;
    }
    fclose(pFile);

    *pFileBuffer = p_newBuffer;
    *bufferSize = fileSize;

    return TRUE;
}



BOOL SavePeFile(LPTSTR strFileName, PVOID pFileBuffer, DWORD dwFileSize)
{
    BOOL result = TRUE;

    // ���ļ�
    HANDLE hFILE = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFILE == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // д�ļ�
    if (WriteFile(hFILE, pFileBuffer, dwFileSize, NULL, NULL) == FALSE)
    {
        result = FALSE;
    }

    CloseHandle(hFILE);
    return result;
}

/*
ImageBuffer ����� ImageBuffer
*/
BOOL FileBufferToImageBuffer(PVOID const pFileBuffer, OUT PVOID* p_ImageBufferOut, DWORD* p_ImageBufferSize)
{
    if (CheckPeFile(pFileBuffer) == FALSE)
    {
        printf("����PE�ļ�\n");
        return FALSE;
    }

    PIMAGE_OPTIONAL_HEADER p_OptionalHeader = GetOptionalHeader(pFileBuffer);
    DWORD imageBufferSize = AlignedSize(p_OptionalHeader->SizeOfImage, p_OptionalHeader->SectionAlignment);
    DWORD numberOfSections = GetSectionNumbers(pFileBuffer);

    // �����ڴ�ռ�
    PVOID pImageBuffer = malloc(imageBufferSize);
    ZeroMemory(pImageBuffer, imageBufferSize); 

    // ���������뵽�ڴ�ռ�
    PVOID p_ImageBufferCursor = pImageBuffer; // �α�ָ��.  
    PVOID pFileBufferCursor = pFileBuffer; // �α�ָ��.  

    // ������ͷ�����ڴ�
    memcpy_s(p_ImageBufferCursor, p_OptionalHeader->SizeOfHeaders, pFileBufferCursor, p_OptionalHeader->SizeOfHeaders);

    // �������н�
    for (DWORD i = 0; i < numberOfSections; i++)
    {
        PIMAGE_SECTION_HEADER p_SectionHeader = GetSectionHeader(pFileBuffer, i);
        p_ImageBufferCursor = (PVOID)((DWORD)pImageBuffer + p_SectionHeader->VirtualAddress);
        pFileBufferCursor = (PVOID)((DWORD)pFileBuffer + p_SectionHeader->PointerToRawData);
        memcpy(p_ImageBufferCursor, pFileBufferCursor, p_SectionHeader->SizeOfRawData);
    }

    pFileBufferCursor = NULL;
    p_ImageBufferCursor = NULL;

    *p_ImageBufferOut = pImageBuffer;
    if (imageBufferSize != NULL) *p_ImageBufferSize = imageBufferSize;

    return TRUE;
}



BOOL InsertSection(PVOID pFileBuffer, DWORD fileBufferSize, DWORD addSize, DWORD characteristics, PSTR name,
    OUT PVOID* out_pNewFileBuffer, OUT PVOID* out_pNewSection, OUT PDWORD out_newFileBufferSize, OUT PDWORD out_newSectionSize)
{
    PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(pFileBuffer);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(pFileBuffer);
    PIMAGE_SECTION_HEADER pFirstSectionHeader = GetSectionHeader(pFileBuffer, 0);
    PIMAGE_SECTION_HEADER pLastSectionHeader = GetSectionHeader(pFileBuffer, pFileHeader->NumberOfSections - 1);

    DWORD dwSectionAlignmen = pOptionalHeader->SectionAlignment;
    DWORD dwFileAlignment = pOptionalHeader->FileAlignment;

    // �½ڱ�λ��
    PVOID pNewSectionHeader = (PVOID)((DWORD)pLastSectionHeader + sizeof(IMAGE_SECTION_HEADER));


    // ͨ�����ַ�ʽ����ͷ��. �ڳ�һ��ռ��Žڱ�
    while (GetHeaderAvailableSize(pFileBuffer) < sizeof(IMAGE_SECTION_HEADER))
    {
        // �������д����ͷ��������(����:�󶨵����)
        RearrangeHeaderData(pFileBuffer);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        // ��dos-stubѹ��
        ResizeDosStub(pFileBuffer, 0, NULL);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        // �ص�DOS/NTͷ
        OverlapDosAndNt(pFileBuffer);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        /* �ϲ��ڶ�ԭ�ļ��Ķ�̫��, ���ﲻʹ��*/
        return FALSE;
    }


    // �½�Rva��ַ
    DWORD dwNewSectionRva = AlignedSize(pLastSectionHeader->Misc.VirtualSize, dwSectionAlignmen) + pLastSectionHeader->VirtualAddress;
    // �½�Foa��ַ
    DWORD dwNewSectionFoa = AlignedSize(pLastSectionHeader->SizeOfRawData, dwFileAlignment) + pLastSectionHeader->PointerToRawData;
    // �½ڴ�С(�ڴ��С���ļ���Сһ��)
    DWORD dwNewSectionSize = AlignedSize(addSize, dwSectionAlignmen);

    // ������ImageSize, ��Ҫ�����ڴ��������
    DWORD dwNewImageSize = dwNewSectionRva + dwNewSectionSize;
    // ������FileBuffer Size
    DWORD dwNewFileBufferSize = dwNewSectionFoa + dwNewSectionSize;


    IMAGE_SECTION_HEADER sectionHeader = { 0 };
    strcpy_s(sectionHeader.Name, IMAGE_SIZEOF_SHORT_NAME, name);
    sectionHeader.Misc.VirtualSize = dwNewSectionSize;
    sectionHeader.SizeOfRawData = dwNewSectionSize;
    sectionHeader.VirtualAddress = dwNewSectionRva;
    sectionHeader.PointerToRawData = dwNewSectionFoa;
    sectionHeader.Characteristics = characteristics;
    sectionHeader.NumberOfLinenumbers = pLastSectionHeader->NumberOfLinenumbers;
    sectionHeader.PointerToRelocations = 0;
    sectionHeader.PointerToLinenumbers = 0;
    sectionHeader.NumberOfRelocations = 0;

    // ����ڱ�
    memcpy_s(pNewSectionHeader, sizeof(IMAGE_SECTION_HEADER), &sectionHeader, sizeof(IMAGE_SECTION_HEADER));

    // �޹��ڱ���
    pFileHeader->NumberOfSections += 1;    // ��������1
    // �޸�ImageSize
    pOptionalHeader->SizeOfImage = dwNewImageSize;        // SizeOfImage 32/64ƫ��һ������Ҫ�ֱ���. 

    // �����ڴ�ռ�
    PVOID pNewFileBuffer = malloc(dwNewFileBufferSize);
    if (pNewFileBuffer == NULL)
    {
        return FALSE;
    }
    ZeroMemory(pNewFileBuffer, dwNewFileBufferSize);    // ��ʼ������

    // ��������
    memcpy_s(pNewFileBuffer, dwNewFileBufferSize, pFileBuffer, fileBufferSize);

    if (out_pNewFileBuffer != NULL) *out_pNewFileBuffer = pNewFileBuffer;
    if (out_pNewSection != NULL) *out_pNewSection = (PVOID)((DWORD)pNewFileBuffer + dwNewSectionFoa);
    if (out_newFileBufferSize != NULL) *out_newFileBufferSize = dwNewFileBufferSize;
    if (out_newSectionSize != NULL) *out_newSectionSize = dwNewSectionSize;

    return TRUE;
}

/*
�ϲ����н�
*/
BOOL MergeAllSections(PVOID pFileBuffer, PVOID* pMergedFileBuffer, DWORD* pdwMergedFileSize)
{
    /*
        1. �����нںϲ���һ������.  ���нڰ����ڴ����ķ�ʽ����.
        2. �����ļ��Ĵ�С�ܱ��
        3. ���������Կճ��ܶ�ڱ�ռ����ʹ��.


        ����
        1. ��FileBuffer���쵽ImageBuffer
        2. �������нڱ� �ϲ��ڱ������.
        3. ����ϲ��ڵĴ�С
        */

    if (pFileBuffer == NULL)
    {
        return FALSE;
    }

    // ��FileBuffer ���쵽 ImageBuffer
    PVOID pImageBuffer = NULL;
    DWORD dwImageSize = 0;
    if (FileBufferToImageBuffer(pFileBuffer, &pImageBuffer, &dwImageSize) == FALSE)
    {
        return FALSE;
    }

    // �������нڱ�, �ϲ��ڱ�����.
    DWORD characteristics = 0;
    for (DWORD i = 0; i < GetSectionNumbers(pFileBuffer); i++)
    {
        PIMAGE_SECTION_HEADER p_SectionHeader = GetSectionHeader(pFileBuffer, i);
        characteristics = characteristics | p_SectionHeader->Characteristics;
    }

    // ����'�ϲ���'�Ĵ�С
    PIMAGE_SECTION_HEADER pImageBufFirstSection = GetSectionHeader(pImageBuffer, 0);
    DWORD mergedVirtualSize = dwImageSize - pImageBufFirstSection->VirtualAddress;
    DWORD mergedSizeOfRawData = dwImageSize - pImageBufFirstSection->VirtualAddress;


    // �޸ĵ�һ�Žڱ�(ImageBuuffer)������
    pImageBufFirstSection->PointerToRawData = pImageBufFirstSection->VirtualAddress;
    pImageBufFirstSection->Characteristics = characteristics;
    pImageBufFirstSection->SizeOfRawData = mergedSizeOfRawData;
    pImageBufFirstSection->Misc.VirtualSize = mergedVirtualSize;


    // �޸Ľڱ�����Ϊ1
    GetFileHeader(pImageBuffer)->NumberOfSections = 1;

    // ��������ڱ�
    memset(
        (PVOID)((DWORD)pImageBufFirstSection + sizeof(IMAGE_SECTION_HEADER)),   // �ڶ���ƫ��
        0,
        (GetSectionNumbers(pFileBuffer) - 1) * sizeof(IMAGE_SECTION_HEADER)
    );

    *pMergedFileBuffer = pImageBuffer;
    *pdwMergedFileSize = dwImageSize;
}

BOOL OverlapDosAndNt(PVOID pFileBufer)
{
    /*
        1. ��NTͷλ���ƶ���ĳƫ�ƴ�.  ��֤OptionalHeader.BaseOfCode �ص��� DosHeader.e_lfanew����.
        2. NTͷ���ƶ���Ӱ�쵽�ڱ�ƫ��, ������Ҫһ���ƶ�.
        3. 0x01~0x02λ�����ڱ���MZͷ���ܶ�, 0x03~e_lfanew ֮��Ŀռ�. ���������������.

        ����
        1. ͳ�����ֽ��� NTͷ(PE���+FILE-HEADER + OPTIONAL-HEADER) + �ڱ�(�ڱ�����*�ڱ��С)
        2. �����ݸ��Ƶ�ƫ�ƴ�. ע��:ֱ�ӴӺ���ǰ���ƿ��ܻ�����ص�����������.
        3. �޸�DOS.e_lfanew ָ����ȷ��λ��
        4. �������ÿ�
        */

    PBYTE pbFileBuffer = pFileBufer;

    // ������Ҫ���Ƶ����ݴ�С
    DWORD dwDataSize = 0;
    PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(pFileBufer);
    DWORD dwSizeOfOptionalHeader = pFileHeader->SizeOfOptionalHeader;
    DWORD dwNumberOfSections = pFileHeader->NumberOfSections;
    dwDataSize += 4;    // PE���
    dwDataSize += sizeof(IMAGE_FILE_HEADER);    // File_Header
    dwDataSize += dwSizeOfOptionalHeader;       // ��ѡͷ��С
    dwDataSize += (dwNumberOfSections * sizeof(IMAGE_SECTION_HEADER));  // ���нڱ��С

    // ��ȡ����
    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBufer);
    PVOID pOrignalData = (PVOID)((DWORD)pFileBufer + pDosHeader->e_lfanew);
    PVOID pData = malloc(dwDataSize);
    memcpy_s(pData, dwDataSize, pOrignalData, dwDataSize);  // ��������

    // ��û���õ��ڴ��ÿ�
    ZeroMemory(pOrignalData, dwDataSize);   // Դ���ݵ��ڴ��ַ(NTͷ+�ڱ�)
    ZeroMemory(pbFileBuffer + 2, sizeof(IMAGE_DOS_HEADER) - 2);    // ����'MZ'���, �������Dosͷ.
    pFileHeader = NULL;     // ԭNTͷ�����, ���ܼ���ʹ��.

    // e_lfanew��ƫ�� - NTͷ.BaseOfCodeƫ�� = PEͷ��ƫ��
    DWORD dwLfanew = (DWORD) & ((IMAGE_DOS_HEADER*)0)->e_lfanew - (DWORD) & ((IMAGE_NT_HEADERS*)0)->OptionalHeader.BaseOfCode;

    // �������ݵ�0x1Cλ��. 
    memcpy_s(pbFileBuffer + dwLfanew, dwDataSize, pData, dwDataSize);

    // 0x02 ~ dwLfanew ֮����ֽڿ�������ʹ��.
    PCSTR message = " By.Joney ";
    memcpy_s(pbFileBuffer + 2, dwLfanew - 2, message, strlen(message));

    // ����lfanew ָ���µ�λ��
    pDosHeader->e_lfanew = dwLfanew;

    // �ͷ���Դ
    free(pData);
    return TRUE;
}

/* �����з���PEͷ������(����,�󶨵����)*/
BOOL RearrangeHeaderData(PVOID pFileBuffer)
{

    PVOID pDataBuf = NULL;
    DWORD dwDataBufSize = 0;

    DWORD dwSizeOfHeaders = 0;
    dwSizeOfHeaders = CheckPE64(pFileBuffer) ?
        GetOptionalHeader64(pFileBuffer)->SizeOfHeaders : GetOptionalHeader(pFileBuffer)->SizeOfHeaders;

    // ͳ����ʱ�ռ��С
    for (size_t i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
    {
        PIMAGE_DATA_DIRECTORY pDataDirectory = GetDataDirectory(pFileBuffer, i);
        if (pDataDirectory->VirtualAddress == 0 || pDataDirectory->Size == 0)
        {
            continue;
        }

        if (pDataDirectory->VirtualAddress < dwSizeOfHeaders)
        {
            dwDataBufSize += pDataDirectory->Size;
        }
    }



    // ��Դ���ݸ��Ƶ���ʱ�ռ�. ���޸�����Ŀ¼
    if (dwDataBufSize != 0)
    {
        // ����ռ�
        pDataBuf = malloc(dwDataBufSize);
        ZeroMemory(pDataBuf, dwDataBufSize);

        PBYTE pbDataBuf = (PBYTE)pDataBuf;

        // ��λ��ƫ��, �����Headerβ����ƫ����.
        DWORD dwNewDataOffset = 0;

        for (size_t i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
        {
            PIMAGE_DATA_DIRECTORY pDataDirectory = GetDataDirectory(pFileBuffer, i);
            if (pDataDirectory->VirtualAddress == 0 || pDataDirectory->Size == 0)
            {
                continue;
            }

            if (pDataDirectory->VirtualAddress < dwSizeOfHeaders)
            {
                // ����Ŀ¼��RVAС�� sizeOfHeaders ˵�����ݴ�����ͷ��.

                // Դ����ָ��
                PVOID pData = (PVOID)((DWORD)pFileBuffer + pDataDirectory->VirtualAddress);

                // ���Ƶ����ݵ���ʱ�ռ�
                memcpy_s(pbDataBuf, pDataDirectory->Size, pData, pDataDirectory->Size);
                pbDataBuf += pDataDirectory->Size;  // ƫ��ָ��

                // ���Դ����
                memset(pData, 0, pDataDirectory->Size);

                // �޸�����Ŀ¼
                dwNewDataOffset += pDataDirectory->Size;
                pDataDirectory->VirtualAddress = dwSizeOfHeaders - dwNewDataOffset;
            }
        }
    }

    if (pDataBuf != NULL)
    {
        PVOID pNewData = (PVOID)((DWORD)pFileBuffer + (dwSizeOfHeaders - dwDataBufSize));
        memcpy_s(pNewData, dwDataBufSize, pDataBuf, dwDataBufSize);

        // �ͷ���Դ
        free(pDataBuf);
    }
    return TRUE;
}

BOOL ResizeDosStub(PVOID pFileBuffer, DWORD size, DWORD* pdwRealSize)
{
    DWORD dwRealSize = 0;
    // ��4�ֽڶ���, �������4�ֽڶ���. �ᵼ�¼���ʧ��. 
    dwRealSize = AlignedSize(size, 4);

    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBuffer);

    // ��ǰdos-stub��С
    DWORD dwCurrentStubSize = GetDosStubSize(pFileBuffer);

    // ������Ҫƫ�Ƶ�λ��
    DWORD dwOffset = pDosHeader->e_lfanew;
    if (pDosHeader->e_lfanew < sizeof(IMAGE_DOS_HEADER))
    {
        // ���DOSͷ��NT���ص��ĵ�. ��Ҫ����
        dwOffset += (sizeof(IMAGE_DOS_HEADER) - pDosHeader->e_lfanew);
    }
    dwOffset += (dwRealSize - dwCurrentStubSize);


    // ����ƫ����Ҫ����ʣ��Ŀ��ÿռ��Ƿ��㹻
    if (pDosHeader->e_lfanew < dwOffset)
    {
        DWORD dwNeedSize = dwOffset - pDosHeader->e_lfanew;     // ��Ҫ�Ĵ�С
        DWORD dwHeaderAvailableSize = GetHeaderAvailableSize(pFileBuffer); // ���ÿռ�

        if (dwNeedSize > dwHeaderAvailableSize)
        {
            // ��offset��ǰ��
            DWORD off = AlignedSize((dwNeedSize - dwHeaderAvailableSize), 4);
            dwOffset = dwOffset - off;
            dwRealSize = dwRealSize - off;
        }
    }

    if (pdwRealSize != NULL) *pdwRealSize = dwRealSize;

    // �ƶ�Header
    return MoveHeaders(pFileBuffer, dwOffset);
}

BOOL MoveHeaders(const PVOID pFileBuffer, DWORD dwOffset)
{
    /*
        ����
        1. �ж�ƫ�Ƶ�λ���Ƿ����
        2. ��������
        3. �޸�e_lfanew
        */
    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBuffer);

    if (dwOffset < sizeof(IMAGE_DOS_HEADER))
    {
        // �ֶ�����dos-stub������ DOSͷ��NTͷ�ص�.
        return FALSE;
    }

    if (dwOffset > pDosHeader->e_lfanew)
    {
        // ���ƫ����Ҫ�жϿ��ÿռ��Ƿ��㹻

        // ������Ҫƫ�Ƶ���
        DWORD dwNeeds = dwOffset - pDosHeader->e_lfanew;
        // �жϿ��ÿռ��Ƿ��㹻
        if (dwNeeds > GetHeaderAvailableSize(pFileBuffer))
        {
            return FALSE;
        }
    }

    // ������Ҫ�ƶ���ͷ��С PE���(4�ֽ�) + FileHeader��С + ��ѡͷ��С + ���нڱ�
    DWORD dwMoveHeadersSize = 4
        + sizeof(IMAGE_FILE_HEADER)
        + GetFileHeader(pFileBuffer)->SizeOfOptionalHeader
        + GetSectionNumbers(pFileBuffer) * sizeof(IMAGE_SECTION_HEADER);

    // ��������
    PVOID pDataTemp = malloc(dwMoveHeadersSize);
    PVOID pDataSrc = (PVOID)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PVOID pDataTarget = (PVOID)((DWORD)pFileBuffer + dwOffset);
    memcpy_s(pDataTemp, dwMoveHeadersSize, pDataSrc, dwMoveHeadersSize);
    // �ÿ�Դ����
    memset(pDataSrc, 0, dwMoveHeadersSize);
    // ���Ƶ�Ŀ��λ��
    memcpy_s(pDataTarget, dwMoveHeadersSize, pDataTemp, dwMoveHeadersSize);

    // �޸�e_lfanew
    pDosHeader->e_lfanew = dwOffset;

    // �ͷ���Դ
    free(pDataTemp);
    return TRUE;
}

BOOL IatInject(const PVOID pFileBuffer, DWORD dwFileBufferSize, PCTSTR strDllName, PCTSTR* pFunctionNameArr, DWORD dwFunctions, OUT PVOID* out_pNewFileBuffer, DWORD* out_dwNewFileBufferSize)
{
    /*
        ����
        1. �����ռ���������µĵ��������. ������Ҫ̫��ռ�.  �ṩ0x1000���ֽ� �㹻��.  �¿ռ���Ҫ���IAT�� ������Ҫ��֤�ɶ���д��ִ��
        2. Ǩ��ԭIID���鵽�µ�λ��, ���IID��������
        3. �޸�����Ŀ¼
    */


    // ��ӽ�
    DWORD dwNewSectionSize;
    DWORD dwNewFileBufferSize;
    PVOID pNewSection;
    PVOID pnNewFileBuffer;
    if (InsertSection(pFileBuffer, dwFileBufferSize, 0x1000,
        IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE,
        ".IAT", &pnNewFileBuffer, &pNewSection, &dwNewFileBufferSize, &dwNewSectionSize) == FALSE)
    {
        // ��ӽ�ʧ��
        return FALSE;
    }

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(pnNewFileBuffer);    // ��ѡͷ(PE32�ṹ)
    PIMAGE_DATA_DIRECTORY pImportDataDirectory = GetDataDirectory(pnNewFileBuffer, IMAGE_DIRECTORY_ENTRY_IMPORT); // ���������Ŀ¼


    PCHAR pDataCursor = (PCHAR)pNewSection; // ƫ��ָ��. ÿ��������ݺ�ƫ��ָ��




    // �ƶ�ԭ�����
    PVOID pImport = (PVOID)((DWORD)pnNewFileBuffer + Rva2Foa(pnNewFileBuffer, pImportDataDirectory->VirtualAddress));
    DWORD dwOriginalIDDSize = pImportDataDirectory->Size - sizeof(IMAGE_IMPORT_DESCRIPTOR); // ԭIDD�����С(��������ȫ��ṹ)
    if (pImportDataDirectory->VirtualAddress != 0 && pImportDataDirectory->Size != 0)
    {
        CopyMemory(pDataCursor, pImport, dwOriginalIDDSize); // IID��������ȫ��ṹ������.
        ZeroMemory(pImport, dwOriginalIDDSize); // ���ԭ����

        pDataCursor = pDataCursor + dwOriginalIDDSize;
    }

    // ������ImportDescriptor
    PIMAGE_IMPORT_DESCRIPTOR pNewImportDescriptor = pDataCursor;     // ��IIDָ��
    pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_DESCRIPTOR);     // ����IIDԤ����λ

    // ��IID����������һ��ȫ��ṹ(��ΪIID����������)
    ZeroMemory(pDataCursor, sizeof(IMAGE_IMPORT_DESCRIPTOR));
    pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_DESCRIPTOR);


    // �޸�����Ŀ¼
    DWORD dwNewImportFoa = (DWORD)pNewSection - (DWORD)pnNewFileBuffer;
    DWORD dwNewImportRva = Foa2Rva(pnNewFileBuffer, dwNewImportFoa);    // Rva
    DWORD dwNewImportSize = (DWORD)pDataCursor - (DWORD)pNewSection;   // IID�����С
    pImportDataDirectory->VirtualAddress = dwNewImportRva;
    pImportDataDirectory->Size = dwNewImportSize;

    // ����INT��IAT
    PVOID pInt = NULL;
    PVOID pIat = NULL;

    // Ԥ��INT��λ��
    if (CheckPE64(pnNewFileBuffer))
    {
        // �����ֽ�
        DWORD offset = (DWORD)pDataCursor - (DWORD)pnNewFileBuffer;
        pDataCursor = pDataCursor + offset % 8;

        // PE32+ INT/IAT һ����λ����8�ֽ�
        pInt = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA64) * (dwFunctions + 1);  // INTԤ������

        pIat = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA64) * (dwFunctions + 1);  // IATԤ������

    }
    else
    {
        // �����ֽ�
        DWORD offset = (DWORD)pDataCursor - (DWORD)pnNewFileBuffer;
        pDataCursor = pDataCursor + offset % 4;

        // PE32 INT/IAT һ����λ����4�ֽ�
        pInt = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA32) * (dwFunctions + 1);  // INTԤ������

        pIat = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA32) * (dwFunctions + 1);  // IATԤ������
    }

    // ����DLL����
    PSTR pDllName = pDataCursor;
#ifdef _UNICODE
    WideCharToMultiByte(CP_OEMCP, 0, strDllName, -1, pDllName, wcslen(strDllName) + 1, NULL, NULL);
#else
    strcpy_s(pDllName, dwDllNameSize, strDllName);
#endif // _UNICODE

    DWORD dwDllNameSize = strlen(pDllName) + 1;
    pDataCursor = pDataCursor + dwDllNameSize;

    // �����ֽ�
    DWORD offset = (DWORD)pDataCursor - (DWORD)pFileBuffer;
    pDataCursor = pDataCursor + offset % 8;

    // ����INT��IAT
    for (size_t i = 0; i < dwFunctions; i++)
    {
        PSTR strName = NULL;
#ifdef _UNICODE
        char dllNameBuffer[MAX_PATH];
        WideCharToMultiByte(CP_OEMCP, 0, pFunctionNameArr[i], MAX_PATH, dllNameBuffer, wcslen(pFunctionNameArr[i]) + 1, NULL, NULL);
        strName = dllNameBuffer;
#else
        strName = pFunctionNameArr[i];
#endif // _UNICODE

        DWORD strLen = strlen(strName);

        // ����Hint�ͺ�����
        PIMAGE_IMPORT_BY_NAME pHintName = (PIMAGE_IMPORT_BY_NAME)pDataCursor;
        pHintName->Hint = 0;
        strcpy_s(pHintName->Name, strLen + 1, strName);

        pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_BY_NAME) + strLen;

        // ����INT/IAT��
        if (CheckPE64(pnNewFileBuffer))
        {
            ULONGLONG dwHintnameFoa = (DWORD)pHintName - (DWORD)pnNewFileBuffer;
            ULONGLONG dwHintnameRva = Foa2Rva(pnNewFileBuffer, dwHintnameFoa);
            ((PIMAGE_THUNK_DATA64)(pInt))[i].u1.AddressOfData = dwHintnameRva;
            ((PIMAGE_THUNK_DATA64)(pIat))[i].u1.AddressOfData = dwHintnameRva;
        }
        else
        {
            DWORD dwHintnameFoa = (DWORD)pHintName - (DWORD)pnNewFileBuffer;
            DWORD dwHintnameRva = Foa2Rva(pnNewFileBuffer, dwHintnameFoa);
            ((PIMAGE_THUNK_DATA32)(pInt))[i].u1.AddressOfData = dwHintnameRva;
            ((PIMAGE_THUNK_DATA32)(pIat))[i].u1.AddressOfData = dwHintnameRva;
        }
    }

    // �����IID�ṹ
    pNewImportDescriptor->Name = Foa2Rva(pnNewFileBuffer, (DWORD)pDllName - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->OriginalFirstThunk = Foa2Rva(pnNewFileBuffer, (DWORD)pInt - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->FirstThunk = Foa2Rva(pnNewFileBuffer, (DWORD)pIat - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->TimeDateStamp = 0;
    pNewImportDescriptor->ForwarderChain = 0xFFFFFFFF;

    // ���ð󶨵����
    PIMAGE_DATA_DIRECTORY pDataDirectoryBoundImport = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    pDataDirectoryBoundImport->Size = 0;
    pDataDirectoryBoundImport->VirtualAddress = 0;

    if (out_pNewFileBuffer != NULL) *out_pNewFileBuffer = pnNewFileBuffer;
    if (out_dwNewFileBufferSize != NULL) *out_dwNewFileBufferSize = dwNewFileBufferSize;
    return TRUE;


}

