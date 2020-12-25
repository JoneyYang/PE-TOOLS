
#include "encry.h"

BOOL EncryptData(PVOID pSource, DWORD dwSourceSize)
{
    if (pSource == NULL || dwSourceSize == 0) return FALSE;

    for (size_t i = 0; i < dwSourceSize; i++)
    {
        ((char*)pSource)[i] = ((char*)pSource)[i] ^ ENCRY_EKY;
    }

    return TRUE;
}

BOOL UnencryData(PVOID pEncryptedData, DWORD dwEncryptedSize)
{
    if (pEncryptedData == NULL || dwEncryptedSize == 0) return FALSE;

    for (size_t i = 0; i < dwEncryptedSize; i++)
    {
        ((char*)pEncryptedData)[i] = ((char*)pEncryptedData)[i] ^ ENCRY_EKY;
    }

    return TRUE;
}

