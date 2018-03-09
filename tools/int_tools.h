/*
 * File:   int_tools.cpp
 * Author: dyer
 *
 * 处理整数值的一些工具函数。
 * Created on February 20, 2017, 10:47 AM
 */
#ifndef WIFISKYNET_INT_TOOLS_H
#define WIFISKYNET_INT_TOOLS_H

#include <cstdint>

 // 函数名称: PutLExx
 // 函数功能: 向buffer中按照小头序写入xx位整数。
 // 函数返回: 返回写入整数后buffer新的起始位置
 // 函数参数: pBuf		待写入的buffer起始位置
 // 函数参数: nValue	整数值
 // 注意事项: 
void* PutLE16(void *pBuf, uint16_t nValue);
void* PutLE32(void *pBuf, uint32_t nValue);
void* PutLE64(void *pBuf, uint64_t nValue);

/**
* GetLExx
* 从buffer中取出对应长度的小头序整数。
* @param pBuf      待取出整数的buffer起始位置
* @return 对应的整数。
*/
uint16_t GetLE16(void *pBuf);
uint32_t GetLE32(void *pBuf);
uint64_t GetLE64(void *pBuf);


#endif //WIFISKYNET_INT_TOOLS_H