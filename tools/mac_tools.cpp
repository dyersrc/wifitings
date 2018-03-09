/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <string.h>
#include "mac_tools.h"

/**
 * 字符串mac转换长整型mac
 * @param it_zmac-byte新型mac
 * @return 长整型mac
 */
uint64_t bmac_to_imac(uint8_t it_zmac[6]) {
    
    uint64_t nMac = 0;
    memcpy(&nMac, it_zmac, 6);
    return nMac;
}
/**
 * 字符串mac转换长整型mac
 * @param pszMac-字符串mac
 * @param it_mac--接收长整型mac
 * @return true：成功，false：失败
 */
bool smac_to_imac(const char* pszMac, uint64_t& it_mac) {
    
    if (NULL == pszMac) {
        return false;
    }
    it_mac = 0;
    uint8_t      it_zmac[6] = { 0 };
    if (smac_to_bmac(pszMac, it_zmac)) {
        memcpy(&it_mac, it_zmac, 6);
        return true;    
    }    
    
    return false;    
}

/**
 * 字符串mac转换bytes
 * @param pszMac-字符串mac
 * @param it_zmac--接收byte新型mac，长度大于等于6
 * @return true：成功，false：失败
 */
bool smac_to_bmac(const char* pszMac, uint8_t* it_zmac) {
    
    if (NULL == pszMac || NULL == it_zmac) {
        return false;
    }
    //eg: FC-AA-14-02-E7-D2 或 FC:AA:14:02:E7:D2
    if (strlen(pszMac) != 17) {
        return false;
    }
    uint8_t it_high;
    uint8_t it_low;
    uint8_t it_value;
    int i = 0;
    int n = 0;
    while (i <= 17 && n < 6) {
        it_high = 0;
        if (!hex_to_byte(pszMac[i], it_high)) {
            return false;
        }
        i++;
        
        it_low = 0;
        if (!hex_to_byte(pszMac[i], it_low)) {
            return false;
        }
        i++;
        it_high = it_high << 4;
        it_value = (it_high&0xF0);
        
        it_low = (it_low&0x0F);
        it_value += it_low;
        
        it_zmac[n] = it_value;
        
        n++;        
        i++;
    }
    if (n >= 6) { 
        return true;
    }
    
    return false;    
}

/**
 * bytes类型mac转字符串mac
 * @param it_zmac--接收byte新型mac
 * @param pszMac--接收字符串mac
 * @param nSize--接收缓冲区大小
 * @return true：成功，false：失败
 */
bool bmac_to_smac(uint8_t it_zmac[6], char* pszMac, int nSize) {
    if (NULL == pszMac || nSize < 18) {
        return false;
    }
    snprintf(pszMac, nSize, "%02X:%02X:%02X:%02X:%02X:%02X", 
            it_zmac[0], it_zmac[1], it_zmac[2], it_zmac[3], it_zmac[4], it_zmac[5]);
    
    return true;
}

/**
 * mac前三段转换为int64
 * @param pszMacPer--mac字符串
 * @param it_macpre--接收mac前三段转换的长整型，如FC:AA:14:02:E7:D2，将FC:AA:14转换
 * @return true：成功，false：失败
 */
bool smacpre_to_imacpre(const char* pszMacPer, uint64_t& it_macpre) {
    
    if (NULL == pszMacPer) {
        return false;
    }
    //eg: FC-AA-14-02-E7-D2 或 FC:AA:14:02:E7:D2
    if (strlen(pszMacPer) != 8) {
        return false;
    }
    uint8_t  it_mac_pre[6] = { 0 };
    uint8_t it_high;
    uint8_t it_low;
    uint8_t it_value;
    int i = 0;
    int n = 0;
    while (i <= 8 && n < 3) {
        it_high = 0;
        if (!hex_to_byte(pszMacPer[i], it_high)) {
            return false;
        }
        i++;
        
        it_low = 0;
        if (!hex_to_byte(pszMacPer[i], it_low)) {
            return false;
        }
        i++;
        it_high = it_high << 4;
        it_value = (it_high&0xF0);
        
        it_low = (it_low&0x0F);
        it_value += it_low;
        
        it_mac_pre[n] = it_value;
        
        n++;        
        i++;
    }
    it_macpre = 0;
    memcpy(&it_macpre, it_mac_pre, 3);
    
    return true;  
}

/**
 * 十六进制字符转byte
 * @param cHex--
 * @param it_value--
 * @return true：成功，false：失败
 */
bool hex_to_byte(const char cHex, uint8_t& it_value) {
    
    switch(cHex) {
        case '0': it_value = 0x0; break;
        case '1': it_value = 0x1; break;
        case '2': it_value = 0x2; break;
        case '3': it_value = 0x3; break;
        case '4': it_value = 0x4; break;
        case '5': it_value = 0x5; break;
        case '6': it_value = 0x6; break;
        case '7': it_value = 0x7; break;
        case '8': it_value = 0x8; break;
        case '9': it_value = 0x9; break;
        case 'A': it_value = 0xA; break;
        case 'B': it_value = 0xB; break;
        case 'C': it_value = 0xC; break;
        case 'D': it_value = 0xD; break;
        case 'E': it_value = 0xE; break;
        case 'F': it_value = 0xF; break;
        case 'a': it_value = 0xA; break;
        case 'b': it_value = 0xB; break;
        case 'c': it_value = 0xC; break;
        case 'd': it_value = 0xD; break;
        case 'e': it_value = 0xE; break;
        case 'f': it_value = 0xF; break;
        default: return false;
    }

    return true;
}