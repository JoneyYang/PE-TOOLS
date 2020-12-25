#ifndef ENCRY_H_20191220
#define ENCRY_H_20191220
#pragma once

#include <windows.h>

#define ENCRY_EKY 0x77;

/*
加密数据
pSource 源数据
dwSrouceSize 源数据大小
pEncryptedData 加密后的数据    在函数内分配内存, 需要调用FreeData来释放
dwEncryptedSize 加密后的大小
*/
BOOL EncryptData(PVOID pSource, DWORD dwSourceSize);


/*
解密数据
pEncryptedData 加密的数据
dwEncryptedSize 加密数据的大小
pUnecrytedData 解密后的数据   在函数内分配内存, 需要调用FreeData来释放
dwUnencryptedSize 解密后数据的大小
*/
BOOL UnencryData(PVOID pEncryptedData, DWORD dwEncryptedSize);


#endif // !ENCRY_H_20191220