/*
 * File:   int_tools.cpp
 * Author: dyer
 *
 * 处理整数值的一些工具函数。
 * Created on February 20, 2017, 10:47 AM
 */
#include "int_tools.h"

 // 函数名称: PutLExx
 // 函数功能: 向buffer中按照小头序写入xx位整数。
 // 函数返回: 返回写入整数后buffer新的起始位置
 // 函数参数: pBuf		待写入的buffer起始位置
 // 函数参数: nValue	整数值
 // 注意事项: 
void* PutLE16(void *pBuf, uint16_t nValue) {
    uint8_t *pIn = (uint8_t *)pBuf;
    *pIn++ = (nValue & 0xff);
    *pIn++ = (nValue >> 8) & 0xff;
    return pIn;
}
void* PutLE32(void *pBuf, uint32_t nValue) {
    uint8_t *pIn = (uint8_t *)pBuf;
    *pIn++ = (nValue & 0xff);
    *pIn++ = (nValue >> 8) & 0xff;
    *pIn++ = (nValue >> 16) & 0xff;
    *pIn++ = (nValue >> 24) & 0xff;
    return pIn;
}
void* PutLE64(void *pBuf, uint64_t nValue) {
    uint8_t *pIn = (uint8_t *)pBuf;
    *pIn++ = (nValue & 0xff);
    *pIn++ = (nValue >> 8) & 0xff;
    *pIn++ = (nValue >> 16) & 0xff;
    *pIn++ = (nValue >> 24) & 0xff;
    *pIn++ = (nValue >> 32) & 0xff;
    *pIn++ = (nValue >> 40) & 0xff;
    *pIn++ = (nValue >> 48) & 0xff;
    *pIn++ = (nValue >> 56) & 0xff;
    return pIn;
}

/**
* GetLExx
* 从buffer中取出对应长度的小头序整数。
* @param pBuf      待取出整数的buffer起始位置
* @return 对应的整数。
*/
uint16_t GetLE16(void *pBuf) {
    uint8_t *pIn = (uint8_t *)pBuf;
    uint16_t nRet = pIn[0];
    nRet |= ((uint16_t)pIn[1]) << 8;
    return nRet;
}
uint32_t GetLE32(void *pBuf) {
    uint8_t *pIn = (uint8_t *)pBuf;
    uint32_t nRet = pIn[0];
    nRet |= ((uint32_t)pIn[1]) << 8;
    nRet |= ((uint32_t)pIn[2]) << 16;
    nRet |= ((uint32_t)pIn[3]) << 24;
    return nRet;
}
uint64_t GetLE64(void *pBuf) {
    uint8_t *pIn = (uint8_t *)pBuf;
    uint64_t nRet = pIn[0];
    nRet |= ((uint64_t)pIn[1]) << 8;
    nRet |= ((uint64_t)pIn[2]) << 16;
    nRet |= ((uint64_t)pIn[3]) << 24;
    nRet |= ((uint64_t)pIn[4]) << 32;
    nRet |= ((uint64_t)pIn[5]) << 40;
    nRet |= ((uint64_t)pIn[6]) << 48;
    nRet |= ((uint64_t)pIn[7]) << 56;
    return nRet;
}