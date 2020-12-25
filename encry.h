#ifndef ENCRY_H_20191220
#define ENCRY_H_20191220
#pragma once

#include <windows.h>

#define ENCRY_EKY 0x77;

/*
��������
pSource Դ����
dwSrouceSize Դ���ݴ�С
pEncryptedData ���ܺ������    �ں����ڷ����ڴ�, ��Ҫ����FreeData���ͷ�
dwEncryptedSize ���ܺ�Ĵ�С
*/
BOOL EncryptData(PVOID pSource, DWORD dwSourceSize);


/*
��������
pEncryptedData ���ܵ�����
dwEncryptedSize �������ݵĴ�С
pUnecrytedData ���ܺ������   �ں����ڷ����ڴ�, ��Ҫ����FreeData���ͷ�
dwUnencryptedSize ���ܺ����ݵĴ�С
*/
BOOL UnencryData(PVOID pEncryptedData, DWORD dwEncryptedSize);


#endif // !ENCRY_H_20191220