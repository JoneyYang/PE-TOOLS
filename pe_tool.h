#ifndef _PE_TOOL_H
#define _PE_TOOL_H
#pragma once


#pragma warning(disable:4127)
#include <wtypes.h>
#include <stdio.h>
#include <tchar.h>


/* ��ȡDOSͷָ�� */
PIMAGE_DOS_HEADER GetDosHeader(const PVOID  pFileBuffer);

/* ��ȡ��׼PEͷ */
PIMAGE_FILE_HEADER GetFileHeader(const PVOID pFileBuffer);

/* ��ȡ��չPEͷ(��ѡPEͷ) */
PIMAGE_OPTIONAL_HEADER GetOptionalHeader(const PVOID pFileBuffer);

/* ��ȡ��չPEͷ(PE32+)*/
PIMAGE_OPTIONAL_HEADER64 GetOptionalHeader64(const PVOID pFileBuffer);


/* ��ȡ�ڱ����� */
DWORD GetSectionNumbers(const PVOID pFileBuffer);

/* ��ȡ�ڱ� */
PIMAGE_SECTION_HEADER GetSectionHeader(const PVOID pFileBuffer, DWORD index);

/* ��ȡ����Ŀ¼ */
PIMAGE_DATA_DIRECTORY GetDataDirectory(const PVOID pFileBuffer, DWORD index);

/* ��ȡ����� */
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(const PVOID pFileBuffer, DWORD index);

/* ��ȡ�󶨵���� */
PIMAGE_BOUND_IMPORT_DESCRIPTOR GetBoundImportDescriptor(const PVOID pFileBuffer);

/* ��ȡ��������� */
DWORD GetImportDescriptorNumbers(const PVOID pFileBuffer);

/* ��ȡ������ */
PIMAGE_EXPORT_DIRECTORY GetExportDirectory(const PVOID pFileBuffer);

/* ��ȡ�ض�λ�� */
PIMAGE_BASE_RELOCATION GetRelocation(const PVOID pFileBuffer);

/* ��ȡ��Դ��Ŀ¼*/
PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory(const PVOID pFileBuffer);

/* ��ȡͷ������λ��(�ڱ�β����ʼ)*/
PVOID GetHeaderAvailble(const PVOID pFileBuffer);

/* RVRתFOA */
DWORD Rva2Foa(const PVOID pFileBuffer, DWORD rva);

/* FOAתRVA */
DWORD Foa2Rva(const PVOID pFileBuffer, DWORD foa);

/*
    ��������Ĵ�С
    unalignedSize Ϊ����Ĵ�С
    aligenment �������. ��С����ֵ����
    */
DWORD AlignedSize(DWORD unalignedSize, DWORD aligenment);

/* ����Ƿ���PE�ļ�*/
BOOL CheckPeFile(const PVOID pFileBuffer);

/* �鿴�Ƿ�ΪPE32+ (64λPE)*/
BOOL CheckPE64(const PVOID pFileBuffer);

/* ����DOS-STUB��С*/
DWORD GetDosStubSize(const PVOID pFileBuffer);

/* ����ͷ�����������ռ��С(һ�������ݻ����ͷ��, ��Ҫ�ж�)*/
DWORD GetHeaderAvailableSize(const PVOID pFileBuffer);

/*
��ȡ�ļ����ڴ�
IN lp_FilePath �ļ�·��
OUT pFileBuffer FileBuffer
OUT bufferSize �ļ���С(FileBuffer)��С
*/
BOOL ReadPeFile(LPTSTR str_FilePath, OUT PVOID* pFileBuffer, OUT DWORD *fileSize);

/*
����PE�ļ�������
strFileName ����·��
pFileBuffer FileBuffer
dwFileSize �ļ���С
*/
BOOL SavePeFile(LPTSTR strFileName, PVOID pFileBuffer, DWORD dwFileSize);

/*
ImageBuffer ����� ImageBuffer
*/
BOOL FileBufferToImageBuffer(PVOID pFileBuffer, OUT PVOID *p_ImageBufferOut, DWORD *p_ImageBufferSize);

/*
������
*/
BOOL InsertSection(PVOID pFileBuffer, DWORD fileBufferSize, DWORD rawSize, DWORD characteristics, PSTR name,
    OUT PVOID* out_pNewFileBuffer, OUT PVOID* out_pNewSection, OUT PDWORD out_newFileBufferSize, OUT PDWORD out_newSectionSize);

/*
�ϲ����н�
*/
BOOL MergeAllSections(PVOID pFileBuffer, PVOID *pMergedFileBuffer, DWORD *pdwMergedFileSize);
/*
�ص� DOSͷ �� NTͷ
pFileBuffer : FileBuffer
*/
BOOL OverlapDosAndNt(PVOID pFileBufer);

/* �����з���PEͷ������(����,�󶨵����)*/
BOOL RearrangeHeaderData(PVOID pFileBuffer);

/* 
��̬����dos-stub��С
size : ϣ�������Ĵ�С
pdwRealSize : ʵ�ʵ����Ĵ�С(ʵ�ʴ�С����Ҫ����4�ֽڶ���);
*/
BOOL ResizeDosStub(PVOID pFileBuffer, DWORD size, DWORD *pdwRealSize);

/* 
�ƶ�ͷ��ָ��λ��, �ƶ����ݰ��� NT-headers, ���нڱ�
dwOffset : FileBuffer��Ŀ��λ�õ�ƫ����
*/
BOOL MoveHeaders(const PVOID pFileBuffer,DWORD dwOffset);

/*
IATע��DLL
pFileBuffer : FileBuffer
strDllName : ��Ҫע���DLL��
pFunctionNameArr : ��Ҫ����ĺ�����ָ������
dwFunctions : ����ĺ�������
out_pNewFileBuffer : ��FileBufferָ��
out_dwNewFileBufferSize : ��FileBuffer��С
*/
BOOL IatInject(const PVOID pFileBuffer, DWORD dwFileBufferSize, PCTSTR strDllName, PCTSTR *pFunctionNameArr, DWORD dwFunctions, OUT PVOID *out_pNewFileBuffer, DWORD *out_dwNewFileBufferSize);

#endif
