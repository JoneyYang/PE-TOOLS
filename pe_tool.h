#ifndef _PE_TOOL_H
#define _PE_TOOL_H
#pragma once


#pragma warning(disable:4127)
#include <wtypes.h>
#include <stdio.h>
#include <tchar.h>


/* 获取DOS头指针 */
PIMAGE_DOS_HEADER GetDosHeader(const PVOID  pFileBuffer);

/* 获取标准PE头 */
PIMAGE_FILE_HEADER GetFileHeader(const PVOID pFileBuffer);

/* 获取拓展PE头(可选PE头) */
PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID pFileBuffer);

/* 获取拓展PE头(PE32+)*/
PIMAGE_OPTIONAL_HEADER64 GetOptionalHeader64(const PVOID pFileBuffer);


/* 获取节表数量 */
DWORD GetSectionNumbers(const PVOID pFileBuffer);

/* 获取节表 */
PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID pFileBuffer, DWORD index);

/* 获取数据目录 */
PIMAGE_DATA_DIRECTORY GetDataDirectory(const PVOID pFileBuffer, DWORD index);

/* 获取导入表 */
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID pFileBuffer, DWORD index);

/* 获取绑定导入表 */
PIMAGE_BOUND_IMPORT_DESCRIPTOR GetBoundImportDescriptor(const PVOID pFileBuffer);

/* 获取导入表数量 */
DWORD GetImportDescriptorNumbers(const PVOID pFileBuffer);

/* 获取导出表 */
PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID pFileBuffer);

/* 获取重定位表 */
PIMAGE_BASE_RELOCATION GetRelocation(const PVOID pFileBuffer);

/* 获取资源表目录*/
PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory(const PVOID pFileBuffer);

/* 获取头部可用位置(节表尾部开始)*/
PVOID GetHeaderAvailble(const PVOID pFileBuffer);

/* RVR转FOA */
DWORD Rva2Foa(const PVOID pFileBuffer, DWORD rva);

/* FOA转RVA */
DWORD Foa2Rva(const PVOID pFileBuffer, DWORD foa);

/*
    计算对齐后的大小
    unalignedSize 为对齐的大小
    aligenment 对齐参数. 大小按该值对齐
    */
DWORD AlignedSize(DWORD unalignedSize, DWORD aligenment);

/* 检查是否是PE文件*/
BOOL CheckPeFile(const PVOID pFileBuffer);

/* 查看是否为PE32+ (64位PE)*/
BOOL CheckPE64(const PVOID pFileBuffer);

/* 计算DOS-STUB大小*/
DWORD GetDosStubSize(const PVOID pFileBuffer);

/* 计算头部可用连续空间大小(一部分数据会放在头部, 需要判断)*/
DWORD GetHeaderAvailableSize(const PVOID pFileBuffer);

/*
读取文件到内存
IN lp_FilePath 文件路径
OUT pFileBuffer FileBuffer
OUT bufferSize 文件大小(FileBuffer)大小
*/
BOOL ReadPeFile(LPTSTR str_FilePath, OUT PVOID* pFileBuffer, OUT DWORD *fileSize);

/*
保存PE文件到本地
strFileName 保存路径
pFileBuffer FileBuffer
dwFileSize 文件大小
*/
BOOL SavePeFile(LPTSTR strFileName, PVOID pFileBuffer, DWORD dwFileSize);

/*
ImageBuffer 拉伸框 ImageBuffer
*/
BOOL FileBufferToImageBuffer(PVOID pFileBuffer, OUT PVOID *p_ImageBufferOut, DWORD *p_ImageBufferSize);

/*
新增节
*/
BOOL InsertSection(PVOID pFileBuffer, DWORD fileBufferSize, DWORD rawSize, DWORD characteristics, PSTR name,
    OUT PVOID* out_pNewFileBuffer, OUT PVOID* out_pNewSection, OUT PDWORD out_newFileBufferSize, OUT PDWORD out_newSectionSize);

/*
合并所有节
*/
BOOL MergeAllSections(PVOID pFileBuffer, PVOID *pMergedFileBuffer, DWORD *pdwMergedFileSize);
/*
重叠 DOS头 和 NT头
pFileBuffer : FileBuffer
*/
BOOL OverlapDosAndNt(PVOID pFileBufer);

/* 重排列放在PE头的数据(例如,绑定导入表)*/
BOOL RearrangeHeaderData(PVOID pFileBuffer);

/* 
动态调整dos-stub大小
size : 希望调整的大小
pdwRealSize : 实际调整的大小(实际大小必须要按照4字节对齐);
*/
BOOL ResizeDosStub(PVOID pFileBuffer, DWORD size, DWORD *pdwRealSize);

/* 
移动头到指定位置, 移动内容包括 NT-headers, 所有节表
dwOffset : FileBuffer到目标位置的偏移量
*/
BOOL MoveHeaders(const PVOID pFileBuffer,DWORD dwOffset);

/*
IAT注入DLL
pFileBuffer : FileBuffer
strDllName : 需要注入的DLL名
pFunctionNameArr : 需要导入的函数名指针数组
dwFunctions : 导入的函数个数
out_pNewFileBuffer : 新FileBuffer指针
out_dwNewFileBufferSize : 新FileBuffer大小
*/
BOOL IatInject(const PVOID pFileBuffer, DWORD dwFileBufferSize, PCTSTR strDllName, PCTSTR *pFunctionNameArr, DWORD dwFunctions, OUT PVOID *out_pNewFileBuffer, DWORD *out_dwNewFileBufferSize);

#endif
