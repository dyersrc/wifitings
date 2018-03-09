/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CLTea.h
 * Author: user
 *
 * Created on 2016年12月6日, 上午10:51
 */

#ifndef LTEA_H
#define LTEA_H

#ifdef  _WINDOWS

#ifdef LTEA_EXPORTS
#define LTEA_API __declspec(dllexport)  
#else
#define LTEA_API __declspec(dllimport)
#endif

#else

#ifdef LTEA_EXPORTS
#define LTEA_API __attribute__((visibility("default")))
#else
#define LTEA_API 
#endif

#endif 

class LTEA_API CLTea {
public:
    CLTea();
    virtual ~CLTea();
    
    /**
     * 加密
     * @param pszSrc--需加密数据
     * @param nSizeOfSrc--需加密数据长度
     * @param pszEncrypt--接收密文,可以和pszSrc使用相同缓冲区，但是缓冲区必须比明文长度大12个字节
     * @param nSizeOfEncrypt--输入加密缓冲区大小，输出密文长度（明文长度+12）
     * @return 成功返回true，失败返回false
     */
    bool encrypt(unsigned char* pszSrc, unsigned int nSizeOfSrc,
                            unsigned char* pszEncrypt, unsigned int& nSizeOfEncrypt);

    /**
     * 解密
     * @param pszSrc--密文
     * @param nSizeOfSrc-密文长度
     * @param pszDecrypt--接收明文，可以和pszSrc使用相同缓冲区
     * @param nSizeOfDecrypt--输入解密缓冲区大小，输出明文长度
     * @return 成功返回true，失败返回false
     */
    bool decrypt(unsigned char* pszSrc, unsigned int nSizeOfSrc,
                            unsigned char* pszDecrypt, unsigned int& nSizeOfDecrypt);
};

#endif /* CLTEA_H */

