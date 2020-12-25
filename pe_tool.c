#define _CRT_SECURE_NO_WARNINGS

#define OFFSET(Type, member) (DWORD)&( ((Type*)0)->member) )

#include "pe_tool.h"

/* 获取DOS头指针 */
PIMAGE_DOS_HEADER GetDosHeader(const PVOID pFileBuffer)
{
    // 文件的首地址 就是DOS头的偏移
    return (PIMAGE_DOS_HEADER)pFileBuffer;
}

/* 获取标准PE头 */
PIMAGE_FILE_HEADER GetFileHeader(const PVOID pFileBuffer)
{
    // 获取DOS头信息
    PIMAGE_DOS_HEADER p_DosHeader = GetDosHeader(pFileBuffer);
    LONG e_lfanew = p_DosHeader->e_lfanew;

    // 偏移 : e_lfanew + 4个字节的PE头标识  
    DWORD offset = e_lfanew + 0x4;

    return (PIMAGE_FILE_HEADER)((DWORD)pFileBuffer + offset);
}

/* 获取拓展PE头(可选PE头) */
PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID pFileBuffer)
{
    // 获取标准PE头指针
    PIMAGE_FILE_HEADER p_FileHeader = GetFileHeader(pFileBuffer);

    // 可选PE头紧挨着标准PE头, 相对偏移就是 sizeof(IMAGE_FILE_HEADER)
    DWORD offsetByFileHeader = sizeof(IMAGE_FILE_HEADER);

    return (PIMAGE_OPTIONAL_HEADER32)((DWORD)p_FileHeader + offsetByFileHeader);
}

PIMAGE_OPTIONAL_HEADER64 GetOptionalHeader64(const PVOID pFileBuffer)
{
    return (PIMAGE_OPTIONAL_HEADER64)GetOptionalHeader(pFileBuffer);
}

/* 获取节表数量 */
DWORD GetSectionNumbers(const PVOID pFileBuffer)
{
    // 通过标准PE头可以获取到节表数量
    return GetFileHeader(pFileBuffer)->NumberOfSections;
}


/* 获取节表 */
PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID pFileBuffer, DWORD index)
{
    // 获取可选PE头
    PIMAGE_OPTIONAL_HEADER p_OptionalHeader = GetOptionalHeader(pFileBuffer);

    // 通过标准PE头获取可选PE头大小
    DWORD offsetByOptionalHeader = (GetFileHeader(pFileBuffer)->SizeOfOptionalHeader);

    // 索引偏移
    DWORD offsetByIndex = index * sizeof(IMAGE_SECTION_HEADER);

    return  (PIMAGE_SECTION_HEADER)((DWORD)p_OptionalHeader + offsetByOptionalHeader + offsetByIndex);
}


/* 获取数据目录 */
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

/* 获取导入表 */
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID pFileBuffer, DWORD index)
{
    // 获取导入表数据目录
    PIMAGE_DATA_DIRECTORY p_DateDirectoryEntryImport = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_IMPORT);

    if (p_DateDirectoryEntryImport->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD importDescriptorRva = p_DateDirectoryEntryImport->VirtualAddress;
    DWORD importDescriptorFoa = Rva2Foa(pFileBuffer, importDescriptorRva);

    return (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + importDescriptorFoa) + index;
}

/* 获取绑定导入表 */
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

/* 获取导出表 */
PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID pFileBuffer)
{
    // 获取导出表数据目录
    PIMAGE_DATA_DIRECTORY p_ExportDataDirectory = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_EXPORT);

    if (p_ExportDataDirectory->VirtualAddress == 0)
    {
        return NULL;
    }

    DWORD exportDirectoryRva = p_ExportDataDirectory->VirtualAddress;
    DWORD exportDirectoryFoa = Rva2Foa(pFileBuffer, exportDirectoryRva);

    return (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + exportDirectoryFoa);
}


/* 获取资源表目录*/
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

/* 获取头部可用位置(节表尾部开始)*/
PVOID GetHeaderAvailble(const PVOID pFileBuffer)
{
    PIMAGE_SECTION_HEADER pLastSectionHeader = GetSectionHeader(pFileBuffer, GetSectionNumbers(pFileBuffer) - 1);
    return (PVOID)((DWORD)pLastSectionHeader + sizeof(IMAGE_SECTION_HEADER));
}

/* 获取重定位表 */
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


/* RVR转FOA */
DWORD Rva2Foa(const PVOID pFileBuffer, DWORD rva)
{
    int numberOfSections = GetSectionNumbers(pFileBuffer);
    PIMAGE_SECTION_HEADER firstSection = GetSectionHeader(pFileBuffer, 0);

    // 用第一个节表的地址判断RVA是否落在节里面.
    if (rva < firstSection->VirtualAddress)
    {
        // 节前面的数据RVA和FOA是相同的. 直接返回. 
        return rva;
    }

    // 判断RVA落在哪个节上
    PIMAGE_SECTION_HEADER targetSection = NULL;
    for (int i = numberOfSections - 1; i >= 0; i--)
    {
        targetSection = GetSectionHeader(pFileBuffer, i);

        if (rva >= targetSection->VirtualAddress)
        {
            break;
        }
    }

    // 计算偏移
    DWORD offset = targetSection->VirtualAddress - targetSection->PointerToRawData;  // 计算这个节的偏移, 只要在同一个节中, 文件中数据与ImageBuffer中数据的偏移都是一样的. 
    DWORD foa = rva - offset;
    return foa;
}

DWORD Foa2Rva(const PVOID pFileBuffer, DWORD foa)
{
    DWORD numberOfSections = GetSectionNumbers(pFileBuffer);
    PIMAGE_SECTION_HEADER firstSection = GetSectionHeader(pFileBuffer, 0);

    // 用第一个节表的地址判断RVA是否落在节里面.
    if (foa < firstSection->PointerToRawData)
    {
        // 节前面的数据RVA和FOA是相同的. 直接返回. 
        return foa;
    }

    // 判断RVA落在哪个节上
    PIMAGE_SECTION_HEADER targetSection = NULL;
    for (size_t i = numberOfSections - 1; i >= 0; i--)
    {
        targetSection = GetSectionHeader(pFileBuffer, i);

        if (foa >= targetSection->PointerToRawData)
        {
            break;
        }
    }

    // 计算偏移
    DWORD offset = targetSection->VirtualAddress - targetSection->PointerToRawData;  // 计算这个节的偏移, 只要在同一个节中, 文件中数据与ImageBuffer中数据的偏移都是一样的. 
    DWORD rva = foa + offset;
    return rva;


}

/*
计算对齐后的大小
unalignedSize 为对齐的大小
aligenment 对齐参数. 大小按该值对齐
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

/* 获取导入表数量 */
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


/* 查看是否为PE32+ (64位PE)*/
BOOL CheckPE64(const PVOID pFileBuffer)
{
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(pFileBuffer);
    return pOptionalHeader->Magic == 0x020B;        // 当可选头的结构是 IMAGE_OPTIONAL_HEADER64时 Magic值是 0x020B
}

/* 计算DOS-STUB大小*/
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

/* 计算头部可用连续空间大小(一部分数据会放在头部, 需要判断)*/
DWORD GetHeaderAvailableSize(const PVOID pFileBuffer)
{
    // 获取最靠前数据的offset
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

    // 获取节表尾部的offset
    DWORD dwAvailableOffset = (DWORD)(GetHeaderAvailble(pFileBuffer)) - (DWORD)pFileBuffer;

    return dwHeadDataOffset - dwAvailableOffset;
}

/*
读取文件到内存
IN lp_FilePath 文件路径
OUT pFileBuffer FileBuffer
OUT bufferSize 文件大小(FileBuffer)大小
*/
BOOL ReadPeFile(LPTSTR lp_FilePath, OUT PVOID* pFileBuffer, OUT DWORD* bufferSize)
{
    PVOID p_newBuffer = NULL;
    DWORD fileSize = 0;
    FILE* pFile = NULL;


    if (lp_FilePath == NULL)
    {
        printf("文件路径为空\n");
        return FALSE;
    }

    pFile = _tfopen(lp_FilePath, TEXT("rb"));
    if (!pFile)
    {
        printf("打开文件失败!");
        return FALSE;
    }

    // 设置流偏移到文件结尾
    fseek(pFile, 0, SEEK_END);
    // 获取当前流的偏移位置(文件结尾位置=文件大小)
    fileSize = ftell(pFile);
    // 设置流偏移到文件开头
    fseek(pFile, 0, SEEK_SET);

    // 分配内存空间
    p_newBuffer = malloc(fileSize);
    if (!p_newBuffer)
    {
        printf("分配空间失败!");
        fclose(pFile);
        return FALSE;
    }

    // 读取文件内容, 返回0表示成功
    size_t n = fread(p_newBuffer, fileSize, 1, pFile);
    if (!n)
    {
        printf("读取数据失败!");
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

    // 打开文件
    HANDLE hFILE = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFILE == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // 写文件
    if (WriteFile(hFILE, pFileBuffer, dwFileSize, NULL, NULL) == FALSE)
    {
        result = FALSE;
    }

    CloseHandle(hFILE);
    return result;
}

/*
ImageBuffer 拉伸框 ImageBuffer
*/
BOOL FileBufferToImageBuffer(PVOID const pFileBuffer, OUT PVOID* p_ImageBufferOut, DWORD* p_ImageBufferSize)
{
    if (CheckPeFile(pFileBuffer) == FALSE)
    {
        printf("不是PE文件\n");
        return FALSE;
    }

    PIMAGE_OPTIONAL_HEADER p_OptionalHeader = GetOptionalHeader(pFileBuffer);
    DWORD imageBufferSize = AlignedSize(p_OptionalHeader->SizeOfImage, p_OptionalHeader->SectionAlignment);
    DWORD numberOfSections = GetSectionNumbers(pFileBuffer);

    // 申请内存空间
    PVOID pImageBuffer = malloc(imageBufferSize);
    ZeroMemory(pImageBuffer, imageBufferSize); 

    // 依次贴代码到内存空间
    PVOID p_ImageBufferCursor = pImageBuffer; // 游标指针.  
    PVOID pFileBufferCursor = pFileBuffer; // 游标指针.  

    // 将所有头贴到内存
    memcpy_s(p_ImageBufferCursor, p_OptionalHeader->SizeOfHeaders, pFileBufferCursor, p_OptionalHeader->SizeOfHeaders);

    // 遍历所有节
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

    // 新节表位置
    PVOID pNewSectionHeader = (PVOID)((DWORD)pLastSectionHeader + sizeof(IMAGE_SECTION_HEADER));


    // 通过各种方式整理头部. 腾出一块空间存放节表
    while (GetHeaderAvailableSize(pFileBuffer) < sizeof(IMAGE_SECTION_HEADER))
    {
        // 重新排列存放在头部的数据(例如:绑定导入表)
        RearrangeHeaderData(pFileBuffer);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        // 将dos-stub压缩
        ResizeDosStub(pFileBuffer, 0, NULL);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        // 重叠DOS/NT头
        OverlapDosAndNt(pFileBuffer);
        if (GetHeaderAvailableSize(pFileBuffer) >= sizeof(IMAGE_SECTION_HEADER)) break;

        /* 合并节对原文件改动太大, 这里不使用*/
        return FALSE;
    }


    // 新节Rva地址
    DWORD dwNewSectionRva = AlignedSize(pLastSectionHeader->Misc.VirtualSize, dwSectionAlignmen) + pLastSectionHeader->VirtualAddress;
    // 新节Foa地址
    DWORD dwNewSectionFoa = AlignedSize(pLastSectionHeader->SizeOfRawData, dwFileAlignment) + pLastSectionHeader->PointerToRawData;
    // 新节大小(内存大小和文件大小一样)
    DWORD dwNewSectionSize = AlignedSize(addSize, dwSectionAlignmen);

    // 计算新ImageSize, 需要按照内存对齐后计算
    DWORD dwNewImageSize = dwNewSectionRva + dwNewSectionSize;
    // 计算新FileBuffer Size
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

    // 插入节表
    memcpy_s(pNewSectionHeader, sizeof(IMAGE_SECTION_HEADER), &sectionHeader, sizeof(IMAGE_SECTION_HEADER));

    // 修过节表数
    pFileHeader->NumberOfSections += 1;    // 节数量加1
    // 修改ImageSize
    pOptionalHeader->SizeOfImage = dwNewImageSize;        // SizeOfImage 32/64偏移一样不需要分别处理. 

    // 申请内存空间
    PVOID pNewFileBuffer = malloc(dwNewFileBufferSize);
    if (pNewFileBuffer == NULL)
    {
        return FALSE;
    }
    ZeroMemory(pNewFileBuffer, dwNewFileBufferSize);    // 初始化数据

    // 复制内容
    memcpy_s(pNewFileBuffer, dwNewFileBufferSize, pFileBuffer, fileBufferSize);

    if (out_pNewFileBuffer != NULL) *out_pNewFileBuffer = pNewFileBuffer;
    if (out_pNewSection != NULL) *out_pNewSection = (PVOID)((DWORD)pNewFileBuffer + dwNewSectionFoa);
    if (out_newFileBufferSize != NULL) *out_newFileBufferSize = dwNewFileBufferSize;
    if (out_newSectionSize != NULL) *out_newSectionSize = dwNewSectionSize;

    return TRUE;
}

/*
合并所有节
*/
BOOL MergeAllSections(PVOID pFileBuffer, PVOID* pMergedFileBuffer, DWORD* pdwMergedFileSize)
{
    /*
        1. 把所有节合并到一个节内.  所有节按照内存对齐的方式排列.
        2. 整个文件的大小很变大
        3. 这样做可以空出很多节表空间可以使用.


        流程
        1. 将FileBuffer拉伸到ImageBuffer
        2. 遍历所有节表 合并节表的属性.
        3. 计算合并节的大小
        */

    if (pFileBuffer == NULL)
    {
        return FALSE;
    }

    // 将FileBuffer 拉伸到 ImageBuffer
    PVOID pImageBuffer = NULL;
    DWORD dwImageSize = 0;
    if (FileBufferToImageBuffer(pFileBuffer, &pImageBuffer, &dwImageSize) == FALSE)
    {
        return FALSE;
    }

    // 遍历所有节表, 合并节表属性.
    DWORD characteristics = 0;
    for (DWORD i = 0; i < GetSectionNumbers(pFileBuffer); i++)
    {
        PIMAGE_SECTION_HEADER p_SectionHeader = GetSectionHeader(pFileBuffer, i);
        characteristics = characteristics | p_SectionHeader->Characteristics;
    }

    // 计算'合并节'的大小
    PIMAGE_SECTION_HEADER pImageBufFirstSection = GetSectionHeader(pImageBuffer, 0);
    DWORD mergedVirtualSize = dwImageSize - pImageBufFirstSection->VirtualAddress;
    DWORD mergedSizeOfRawData = dwImageSize - pImageBufFirstSection->VirtualAddress;


    // 修改第一张节表(ImageBuuffer)的属性
    pImageBufFirstSection->PointerToRawData = pImageBufFirstSection->VirtualAddress;
    pImageBufFirstSection->Characteristics = characteristics;
    pImageBufFirstSection->SizeOfRawData = mergedSizeOfRawData;
    pImageBufFirstSection->Misc.VirtualSize = mergedVirtualSize;


    // 修改节表数量为1
    GetFileHeader(pImageBuffer)->NumberOfSections = 1;

    // 清空其他节表
    memset(
        (PVOID)((DWORD)pImageBufFirstSection + sizeof(IMAGE_SECTION_HEADER)),   // 第二张偏移
        0,
        (GetSectionNumbers(pFileBuffer) - 1) * sizeof(IMAGE_SECTION_HEADER)
    );

    *pMergedFileBuffer = pImageBuffer;
    *pdwMergedFileSize = dwImageSize;
}

BOOL OverlapDosAndNt(PVOID pFileBufer)
{
    /*
        1. 将NT头位置移动到某偏移处.  保证OptionalHeader.BaseOfCode 重叠到 DosHeader.e_lfanew上面.
        2. NT头的移动会影响到节表偏移, 所以需要一起移动.
        3. 0x01~0x02位置用于保存MZ头不能动, 0x03~e_lfanew 之间的空间. 可以填充任意数据.

        流程
        1. 统计总字节数 NT头(PE标记+FILE-HEADER + OPTIONAL-HEADER) + 节表(节表数量*节表大小)
        2. 将数据复制到偏移处. 注意:直接从后往前复制可能会出现重叠区复制问题.
        3. 修改DOS.e_lfanew 指向正确的位置
        4. 把数据置空
        */

    PBYTE pbFileBuffer = pFileBufer;

    // 计算需要复制的数据大小
    DWORD dwDataSize = 0;
    PIMAGE_FILE_HEADER pFileHeader = GetFileHeader(pFileBufer);
    DWORD dwSizeOfOptionalHeader = pFileHeader->SizeOfOptionalHeader;
    DWORD dwNumberOfSections = pFileHeader->NumberOfSections;
    dwDataSize += 4;    // PE标记
    dwDataSize += sizeof(IMAGE_FILE_HEADER);    // File_Header
    dwDataSize += dwSizeOfOptionalHeader;       // 可选头大小
    dwDataSize += (dwNumberOfSections * sizeof(IMAGE_SECTION_HEADER));  // 所有节表大小

    // 提取数据
    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBufer);
    PVOID pOrignalData = (PVOID)((DWORD)pFileBufer + pDosHeader->e_lfanew);
    PVOID pData = malloc(dwDataSize);
    memcpy_s(pData, dwDataSize, pOrignalData, dwDataSize);  // 复制数据

    // 将没有用的内存置空
    ZeroMemory(pOrignalData, dwDataSize);   // 源数据的内存地址(NT头+节表)
    ZeroMemory(pbFileBuffer + 2, sizeof(IMAGE_DOS_HEADER) - 2);    // 保留'MZ'标记, 清空整个Dos头.
    pFileHeader = NULL;     // 原NT头已清空, 不能继续使用.

    // e_lfanew的偏移 - NT头.BaseOfCode偏移 = PE头的偏移
    DWORD dwLfanew = (DWORD) & ((IMAGE_DOS_HEADER*)0)->e_lfanew - (DWORD) & ((IMAGE_NT_HEADERS*)0)->OptionalHeader.BaseOfCode;

    // 复制数据到0x1C位置. 
    memcpy_s(pbFileBuffer + dwLfanew, dwDataSize, pData, dwDataSize);

    // 0x02 ~ dwLfanew 之间的字节可以随意使用.
    PCSTR message = " By.Joney ";
    memcpy_s(pbFileBuffer + 2, dwLfanew - 2, message, strlen(message));

    // 设置lfanew 指向新的位置
    pDosHeader->e_lfanew = dwLfanew;

    // 释放资源
    free(pData);
    return TRUE;
}

/* 重排列放在PE头的数据(例如,绑定导入表)*/
BOOL RearrangeHeaderData(PVOID pFileBuffer)
{

    PVOID pDataBuf = NULL;
    DWORD dwDataBufSize = 0;

    DWORD dwSizeOfHeaders = 0;
    dwSizeOfHeaders = CheckPE64(pFileBuffer) ?
        GetOptionalHeader64(pFileBuffer)->SizeOfHeaders : GetOptionalHeader(pFileBuffer)->SizeOfHeaders;

    // 统计临时空间大小
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



    // 将源数据复制到临时空间. 并修复数据目录
    if (dwDataBufSize != 0)
    {
        // 分配空间
        pDataBuf = malloc(dwDataBufSize);
        ZeroMemory(pDataBuf, dwDataBufSize);

        PBYTE pbDataBuf = (PBYTE)pDataBuf;

        // 新位置偏移, 相对于Header尾部的偏移量.
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
                // 数据目录的RVA小于 sizeOfHeaders 说明数据存在于头部.

                // 源数据指针
                PVOID pData = (PVOID)((DWORD)pFileBuffer + pDataDirectory->VirtualAddress);

                // 复制到数据到临时空间
                memcpy_s(pbDataBuf, pDataDirectory->Size, pData, pDataDirectory->Size);
                pbDataBuf += pDataDirectory->Size;  // 偏移指针

                // 清空源数据
                memset(pData, 0, pDataDirectory->Size);

                // 修复数据目录
                dwNewDataOffset += pDataDirectory->Size;
                pDataDirectory->VirtualAddress = dwSizeOfHeaders - dwNewDataOffset;
            }
        }
    }

    if (pDataBuf != NULL)
    {
        PVOID pNewData = (PVOID)((DWORD)pFileBuffer + (dwSizeOfHeaders - dwDataBufSize));
        memcpy_s(pNewData, dwDataBufSize, pDataBuf, dwDataBufSize);

        // 释放资源
        free(pDataBuf);
    }
    return TRUE;
}

BOOL ResizeDosStub(PVOID pFileBuffer, DWORD size, DWORD* pdwRealSize)
{
    DWORD dwRealSize = 0;
    // 按4字节对齐, 如果不按4字节对齐. 会导致加载失败. 
    dwRealSize = AlignedSize(size, 4);

    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBuffer);

    // 当前dos-stub大小
    DWORD dwCurrentStubSize = GetDosStubSize(pFileBuffer);

    // 计算需要偏移的位置
    DWORD dwOffset = pDosHeader->e_lfanew;
    if (pDosHeader->e_lfanew < sizeof(IMAGE_DOS_HEADER))
    {
        // 如果DOS头和NT是重叠的的. 需要计算
        dwOffset += (sizeof(IMAGE_DOS_HEADER) - pDosHeader->e_lfanew);
    }
    dwOffset += (dwRealSize - dwCurrentStubSize);


    // 往后偏移需要考虑剩余的可用空间是否足够
    if (pDosHeader->e_lfanew < dwOffset)
    {
        DWORD dwNeedSize = dwOffset - pDosHeader->e_lfanew;     // 需要的大小
        DWORD dwHeaderAvailableSize = GetHeaderAvailableSize(pFileBuffer); // 可用空间

        if (dwNeedSize > dwHeaderAvailableSize)
        {
            // 将offset往前推
            DWORD off = AlignedSize((dwNeedSize - dwHeaderAvailableSize), 4);
            dwOffset = dwOffset - off;
            dwRealSize = dwRealSize - off;
        }
    }

    if (pdwRealSize != NULL) *pdwRealSize = dwRealSize;

    // 移动Header
    return MoveHeaders(pFileBuffer, dwOffset);
}

BOOL MoveHeaders(const PVOID pFileBuffer, DWORD dwOffset)
{
    /*
        流程
        1. 判断偏移的位置是否合理
        2. 复制数据
        3. 修复e_lfanew
        */
    PIMAGE_DOS_HEADER pDosHeader = GetDosHeader(pFileBuffer);

    if (dwOffset < sizeof(IMAGE_DOS_HEADER))
    {
        // 手动设置dos-stub不能让 DOS头和NT头重叠.
        return FALSE;
    }

    if (dwOffset > pDosHeader->e_lfanew)
    {
        // 向后偏移需要判断可用空间是否足够

        // 计算需要偏移的量
        DWORD dwNeeds = dwOffset - pDosHeader->e_lfanew;
        // 判断可用空间是否足够
        if (dwNeeds > GetHeaderAvailableSize(pFileBuffer))
        {
            return FALSE;
        }
    }

    // 计算需要移动的头大小 PE标记(4字节) + FileHeader大小 + 可选头大小 + 所有节表
    DWORD dwMoveHeadersSize = 4
        + sizeof(IMAGE_FILE_HEADER)
        + GetFileHeader(pFileBuffer)->SizeOfOptionalHeader
        + GetSectionNumbers(pFileBuffer) * sizeof(IMAGE_SECTION_HEADER);

    // 复制数据
    PVOID pDataTemp = malloc(dwMoveHeadersSize);
    PVOID pDataSrc = (PVOID)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PVOID pDataTarget = (PVOID)((DWORD)pFileBuffer + dwOffset);
    memcpy_s(pDataTemp, dwMoveHeadersSize, pDataSrc, dwMoveHeadersSize);
    // 置空源数据
    memset(pDataSrc, 0, dwMoveHeadersSize);
    // 复制到目标位置
    memcpy_s(pDataTarget, dwMoveHeadersSize, pDataTemp, dwMoveHeadersSize);

    // 修复e_lfanew
    pDosHeader->e_lfanew = dwOffset;

    // 释放资源
    free(pDataTemp);
    return TRUE;
}

BOOL IatInject(const PVOID pFileBuffer, DWORD dwFileBufferSize, PCTSTR strDllName, PCTSTR* pFunctionNameArr, DWORD dwFunctions, OUT PVOID* out_pNewFileBuffer, DWORD* out_dwNewFileBufferSize)
{
    /*
        流程
        1. 创建空间用来存放新的导入表数据. 不会需要太多空间.  提供0x1000个字节 足够了.  新空间需要存放IAT表 所以需要保证可读可写可执行
        2. 迁移原IID数组到新的位置, 清空IID数组区域
        3. 修复数据目录
    */


    // 添加节
    DWORD dwNewSectionSize;
    DWORD dwNewFileBufferSize;
    PVOID pNewSection;
    PVOID pnNewFileBuffer;
    if (InsertSection(pFileBuffer, dwFileBufferSize, 0x1000,
        IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE,
        ".IAT", &pnNewFileBuffer, &pNewSection, &dwNewFileBufferSize, &dwNewSectionSize) == FALSE)
    {
        // 添加节失败
        return FALSE;
    }

    PIMAGE_OPTIONAL_HEADER pOptionalHeader = GetOptionalHeader(pnNewFileBuffer);    // 可选头(PE32结构)
    PIMAGE_DATA_DIRECTORY pImportDataDirectory = GetDataDirectory(pnNewFileBuffer, IMAGE_DIRECTORY_ENTRY_IMPORT); // 导入表数据目录


    PCHAR pDataCursor = (PCHAR)pNewSection; // 偏移指针. 每次填充数据后偏移指针




    // 移动原导入表
    PVOID pImport = (PVOID)((DWORD)pnNewFileBuffer + Rva2Foa(pnNewFileBuffer, pImportDataDirectory->VirtualAddress));
    DWORD dwOriginalIDDSize = pImportDataDirectory->Size - sizeof(IMAGE_IMPORT_DESCRIPTOR); // 原IDD数组大小(不算最后的全零结构)
    if (pImportDataDirectory->VirtualAddress != 0 && pImportDataDirectory->Size != 0)
    {
        CopyMemory(pDataCursor, pImport, dwOriginalIDDSize); // IID数组最后的全零结构不复制.
        ZeroMemory(pImport, dwOriginalIDDSize); // 清空原数据

        pDataCursor = pDataCursor + dwOriginalIDDSize;
    }

    // 构建新ImportDescriptor
    PIMAGE_IMPORT_DESCRIPTOR pNewImportDescriptor = pDataCursor;     // 新IID指针
    pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_DESCRIPTOR);     // 给新IID预留空位

    // 最IID数组最后添加一个全零结构(作为IID数组结束标记)
    ZeroMemory(pDataCursor, sizeof(IMAGE_IMPORT_DESCRIPTOR));
    pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_DESCRIPTOR);


    // 修复数据目录
    DWORD dwNewImportFoa = (DWORD)pNewSection - (DWORD)pnNewFileBuffer;
    DWORD dwNewImportRva = Foa2Rva(pnNewFileBuffer, dwNewImportFoa);    // Rva
    DWORD dwNewImportSize = (DWORD)pDataCursor - (DWORD)pNewSection;   // IID数组大小
    pImportDataDirectory->VirtualAddress = dwNewImportRva;
    pImportDataDirectory->Size = dwNewImportSize;

    // 构建INT和IAT
    PVOID pInt = NULL;
    PVOID pIat = NULL;

    // 预留INT表位置
    if (CheckPE64(pnNewFileBuffer))
    {
        // 对齐字节
        DWORD offset = (DWORD)pDataCursor - (DWORD)pnNewFileBuffer;
        pDataCursor = pDataCursor + offset % 8;

        // PE32+ INT/IAT 一个单位长度8字节
        pInt = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA64) * (dwFunctions + 1);  // INT预留长度

        pIat = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA64) * (dwFunctions + 1);  // IAT预留长度

    }
    else
    {
        // 对齐字节
        DWORD offset = (DWORD)pDataCursor - (DWORD)pnNewFileBuffer;
        pDataCursor = pDataCursor + offset % 4;

        // PE32 INT/IAT 一个单位长度4字节
        pInt = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA32) * (dwFunctions + 1);  // INT预留长度

        pIat = pDataCursor;
        pDataCursor = pDataCursor + sizeof(IMAGE_THUNK_DATA32) * (dwFunctions + 1);  // IAT预留长度
    }

    // 插入DLL名称
    PSTR pDllName = pDataCursor;
#ifdef _UNICODE
    WideCharToMultiByte(CP_OEMCP, 0, strDllName, -1, pDllName, wcslen(strDllName) + 1, NULL, NULL);
#else
    strcpy_s(pDllName, dwDllNameSize, strDllName);
#endif // _UNICODE

    DWORD dwDllNameSize = strlen(pDllName) + 1;
    pDataCursor = pDataCursor + dwDllNameSize;

    // 对齐字节
    DWORD offset = (DWORD)pDataCursor - (DWORD)pFileBuffer;
    pDataCursor = pDataCursor + offset % 8;

    // 构建INT和IAT
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

        // 插入Hint和函数名
        PIMAGE_IMPORT_BY_NAME pHintName = (PIMAGE_IMPORT_BY_NAME)pDataCursor;
        pHintName->Hint = 0;
        strcpy_s(pHintName->Name, strLen + 1, strName);

        pDataCursor = pDataCursor + sizeof(IMAGE_IMPORT_BY_NAME) + strLen;

        // 修正INT/IAT项
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

    // 填充新IID结构
    pNewImportDescriptor->Name = Foa2Rva(pnNewFileBuffer, (DWORD)pDllName - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->OriginalFirstThunk = Foa2Rva(pnNewFileBuffer, (DWORD)pInt - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->FirstThunk = Foa2Rva(pnNewFileBuffer, (DWORD)pIat - (DWORD)pnNewFileBuffer);
    pNewImportDescriptor->TimeDateStamp = 0;
    pNewImportDescriptor->ForwarderChain = 0xFFFFFFFF;

    // 重置绑定导入表
    PIMAGE_DATA_DIRECTORY pDataDirectoryBoundImport = GetDataDirectory(pFileBuffer, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    pDataDirectoryBoundImport->Size = 0;
    pDataDirectoryBoundImport->VirtualAddress = 0;

    if (out_pNewFileBuffer != NULL) *out_pNewFileBuffer = pnNewFileBuffer;
    if (out_dwNewFileBufferSize != NULL) *out_dwNewFileBufferSize = dwNewFileBufferSize;
    return TRUE;


}

