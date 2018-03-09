/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mac_tools.h
 * Author: dyer
 *
 * Created on 2017年1月11日, 下午2:02
 */

#ifndef MAC_TOOLS_H
#define MAC_TOOLS_H

#include <stdint.h>

/**
 * 字符串mac转换长整型mac
 * @param it_zmac-byte新型mac
 * @return 长整型mac
 */
uint64_t bmac_to_imac(uint8_t it_zmac[6]);

/**
 * 字符串mac转换长整型mac
 * @param pszMac-字符串mac
 * @param it_mac--接收长整型mac
 * @return true：成功，false：失败
 */
bool smac_to_imac(const char* pszMac, uint64_t& it_mac);

/**
 * 字符串mac转换bytes
 * @param pszMac-字符串mac
 * @param it_zmac--接收byte新型mac，长度大于等于6
 * @return true：成功，false：失败
 */
bool smac_to_bmac(const char* pszMac, uint8_t* it_zmac);

/**
 * bytes类型mac转字符串mac
 * @param it_zmac--byte新型mac
 * @param pszMac--接收字符串mac
 * @param nSize--接收缓冲区大小
 * @return true：成功，false：失败
 */
bool bmac_to_smac(uint8_t it_zmac[6], char* pszMac, int nSize);

/**
 * mac前三段转换为int64
 * @param pszMacPer--mac字符串
 * @param it_macpre--接收mac前三段转换的长整型，如FC:AA:14:02:E7:D2，将FC:AA:14转换
 * @return true：成功，false：失败
 */
bool smacpre_to_imacpre(const char* pszMacPer, uint64_t& it_macpre);

/**
 * 十六进制字符转byte
 * @param cHex--
 * @param it_value--
 * @return true：成功，false：失败
 */
bool hex_to_byte(const char cHex, uint8_t& it_value) ;


#endif /* MAC_TOOLS_H */

