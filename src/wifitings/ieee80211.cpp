/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ieee80211.cpp
 * Author: dyer
 * 
 * Created on 2016年6月25日, 下午1:00
 */

#include "../../include/DbgPrint.h"
#include "ieee80211.h"
#include "../../tools/int_tools.h"

#pragma pack(push, 1)

typedef struct __ieee80211_radiotap_header {
    uint8_t             nVersion;    /* set to 0 */
    uint8_t             nPad;
    uint16_t            nLen;         /* entire length */
    uint32_t            zPresent[1];   /* fields present */
}ieee80211_radiotap_header, *pieee80211_radiotap_header;

typedef struct __ieee80211_frame_control {
    uint8_t             nTypes;            //帧类新
    uint8_t             nFlags;             //帧标志
    
} ieee80211_frame_control, *pieee80211_frame_control;

typedef struct __fixed_parameters_6 {
    uint16_t            nCapabilitiesInfo;
    uint16_t            nStatusCode;
    uint16_t            nAssociationId;
}fixed_parameters_6, *pfixed_parameters_6;

typedef struct __fixed_parameters_12 {
    uint64_t            nTimestamp;
    uint16_t            nBeacon_interval;
    uint16_t            nCapabilities_info;
}fixed_parameters_12, *pfixed_parameters_12;

typedef struct __tagged_pamaeters_tag {
    uint8_t              nTag;       //tag号,
                                          //0:SSID 
                                          //1:supported rates 
                                          //3:DS parameter set
    uint8_t              nLength;   //data长度
    uint8_t              zValue[];
    
}tagged_pamaeters_tag, *ptagged_pamaeters_tag;

typedef struct __tagged16_pamaeters_tag {
    uint16_t              nTag;       //tag号                                          
    uint16_t              nLength;   //data长度
    uint8_t              zValue[];
    
}tagged16_pamaeters_tag, *ptagged16_pamaeters_tag;

#pragma pack(pop)


/**
 * 构造函数
 */
CIeee80211::CIeee80211() {
    m_ulSignature = SG_IEEE80211;
    memset(&m_radiotap_data, 0x00, sizeof(RADIOAP_DATA));
    memset(&m_ieee80211_frame, 0x00, sizeof(IEEE80211_FRAME));  
    memset(&m_ieee80211_frame_parse_param, 0x00, sizeof(IEEE80211_FRAME_PARSE_PARAM));
    m_ieee80211_frame_param.nMax = 0;
    m_ieee80211_frame_param.nLen = 0;
    m_ieee80211_frame_param.pData = NULL;
    m_bParseSuccess = false;
    m_bEixstParseParam = false;
}

/**
 * 析构函数
 */
CIeee80211::~CIeee80211() {
    m_ulSignature = 0;
    if (NULL != m_ieee80211_frame_param.pData ) {
        free(m_ieee80211_frame_param.pData);
        m_ieee80211_frame_param.nMax = 0;
        m_ieee80211_frame_param.nLen = 0;
        m_ieee80211_frame_param.pData = NULL;
    }
}

/**
 * 解析
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @return true：成功，false：失败
 */
bool CIeee80211::parse(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket) {
    
    uint32_t  nOffset = 0;
    uint16_t  nLen = 0;
    
    m_bParseSuccess = false;
    m_bEixstFrameParam = false;
    m_bEixstParseParam = false;
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    
    //解析radiotap
    if (!parseRadiotap(pPkthdr, pPacket, nLen)) {//解析radiotap失败
        return false;
    }
    nOffset += nLen;
    if (nOffset >= pPkthdr->len) { //数据错误
        return false;
    }
    
    struct pcap_pkthdr Pkthdr = { 0 };
    Pkthdr.caplen = pPkthdr->caplen - nOffset;
    Pkthdr.len = pPkthdr->len - nOffset;
    Pkthdr.ts = pPkthdr->ts;
    //解析ieee80211帧
    nLen = 0;
    if (!parseIeee80211(&Pkthdr, &pPacket[nOffset], nLen)) {
        return false;
    }
    m_bParseSuccess = true;
    nOffset += nLen;
    if (nOffset >= pPkthdr->len) { //数据错误
        return m_bParseSuccess;
    }
    Pkthdr.caplen = pPkthdr->caplen - nOffset;
    Pkthdr.len = pPkthdr->len - nOffset;
    Pkthdr.ts = pPkthdr->ts;
    if (parseIeee80211Param(&Pkthdr, &pPacket[nOffset], nLen)) {
        m_bEixstParseParam = true;
    }   
    
#ifdef __DEBUG_PRINT_03
    printResult();
#endif
    
    return m_bParseSuccess;
}

/**
 * 得到radiotap
 * @return 
 */
PRADIOAP_DATA CIeee80211::getRadiotap() {
    
    if (!m_bParseSuccess) {
        return NULL;
    }
    
    return &m_radiotap_data;
}

/**
 * 得到ieee80211
 * @return 
 */
PIEEE80211_FRAME CIeee80211::getIeee80211() {
    
    if (!m_bParseSuccess) {
        return NULL;
    }
    
    return &m_ieee80211_frame;
}

/**
 * 得到ieee80211参数
 * @return 
 */
PIEEE80211_FRAME_PARAM CIeee80211::getIeee80211Param() {
    
    if (!m_bParseSuccess || !m_bEixstFrameParam) {
        return NULL;
    }
    
    return &m_ieee80211_frame_param;
}

/**
 * 得到ieee80211解析参数
 * @return 
 */
PIEEE80211_FRAME_PARSE_PARAM CIeee80211::getIeee80211ParseParam() {
    if (!m_bParseSuccess || !m_bEixstParseParam) {
        return NULL;
    }
    
    return &m_ieee80211_frame_parse_param;
}

/**
 * 解析radiotap,暂时只解析信道，信道标识，天线信号强度，天线信噪强度
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--radiotap长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseRadiotap(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen) {
    
    unsigned char*   pPos = NULL;
    bool bRet = false;
    int nSize = 0;
    
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    nLen = 0;
    nSize = sizeof(ieee80211_radiotap_header);
    if (pPkthdr->len < nSize) {
        return false;
    }
    pieee80211_radiotap_header pRadiotapHdr = (pieee80211_radiotap_header)pPacket;
    if (0 != pRadiotapHdr->nVersion) {
        return false;
    }
    //little-endian radiotap header length
    nLen = GetLE16(&pRadiotapHdr->nLen);
    
    if (nLen <= 4) {
        return false;
    }    
    memset(&m_radiotap_data, 0x00, sizeof(RADIOAP_DATA));
    
    int nAntenna = 0;
    uint32_t zPresent[4] = { 0 };
    zPresent[nAntenna] = GetLE32(&pRadiotapHdr->zPresent[0]);
    pPos = (unsigned char*)&pPacket[sizeof(ieee80211_radiotap_header)];
    while (nSize < nLen && (0x20000000 & (zPresent[nAntenna])) && nAntenna < 4) {
        nAntenna++;
        zPresent[nAntenna] = GetLE32(&pRadiotapHdr->zPresent[nAntenna]);
        nSize += 4;
        pPos += 4;
    }
    m_radiotap_data.nAntenna = 0;
    bool bAntenna = false;
    int nThan = 0;
    for (int i = 0; i <= nAntenna; i++) {
        bAntenna = false;
        if ( 0x00000001 & zPresent[i] ) {//TSFT
            nThan = (nSize%8);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 8;
            pPos += 8;
        } 
        if ( 0x00000002 & zPresent[i] ) {//Flags
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00000004 & zPresent[i] ) {//Rate
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00000008 & zPresent[i] ) {//Channel
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 4;
            if (i < 1) {
                if (pPkthdr->len < nSize) {
                    return false;
                }
                uint16_t            nChannel;
                uint16_t            nChannel_flag;
                int32_t             nFreq;
                //memcpy(&nChannel, pPos, 2);
                nChannel = GetLE16(pPos);
                nFreq = (int32_t)nChannel;
                pPos += 2;
                memcpy(&nChannel_flag, pPos, 2);
                m_radiotap_data.nChannel_flags = nChannel_flag;
                pPos += 2;
                m_radiotap_data.nChannel = ieee80211_frequency_to_channel(nFreq);
                bRet = true;
            } else {
                pPos += 4;
            }
        }
        if ( 0x00000010 & zPresent[i] ) {//FHSS
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00000020 & zPresent[i] ) {//dBm Antenna Signal
            if (pPkthdr->len < nSize + 1) {
                return false;
            }
            if (i < 1) {     
                memcpy(&(m_radiotap_data.nSignal), pPos, 1);    
                bRet = true;  
            } else {
                bAntenna = true;
                memcpy(&(m_radiotap_data.zAntenna[m_radiotap_data.nAntenna].nSignal), pPos, 1);              
            }       
            nSize += 1;
            pPos += 1;  
        }
        if ( 0x00000040 & zPresent[i] ) {//dBm Antenna Noise
            nSize += 1;
            if (pPkthdr->len < nSize) {
                return false;
            }
            memcpy(&(m_radiotap_data.nNoise), pPos, 1);
            pPos += 1;            
        }
        if ( 0x00000080 & zPresent[i] ) {//Lock Quality
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00000100 & zPresent[i] ) {//TX Attenuation
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00000200 & zPresent[i] ) {//dB TX Attenuation
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00000400 & zPresent[i] ) {//dBm TX Power
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00000800 & zPresent[i] ) {//Antenna
            nSize += 1;
            if (pPkthdr->len < nSize) {
                return false;
            }
            if (i >= 1) {   
                bAntenna = true;
                memcpy(&(m_radiotap_data.zAntenna[m_radiotap_data.nAntenna].nId), pPos, 1);                              
            }
            pPos += 1; 
        }
        if ( 0x00001000 & zPresent[i] ) {//dB Antenna Signal
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00002000 & zPresent[i] ) {//dB Antenna Noise
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00004000 & zPresent[i] ) {//RX Flags
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00008000 & zPresent[i] ) {//TX Flags
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 2;
            pPos += 2;
        }
        if ( 0x00010000 & zPresent[i] ) {//Rts Retries
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00020000 & zPresent[i] ) {//Data Retries
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00040000 & zPresent[i] ) {//Channel+ ??????????
            nSize += 1;
            pPos += 1;
        }
        if ( 0x00080000 & zPresent[i] ) {//MCS
            nSize += 3;
            pPos += 3;
        }
        if ( 0x00100000 & zPresent[i] ) {//A-MPDU Status
            nThan = (nSize%4);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 8;
            pPos += 8;
        }
        if ( 0x00200000 & zPresent[i] ) {//VHT information
            nThan = (nSize%2);
            if (nThan != 0) {
                nSize += nThan;
                pPos += nThan;
            }
            nSize += 12;
            pPos += 12;
        }
//        if ( 0x00400000 & zPresent[i] ) {//VHT information //IEEE80211_RADIOTAP_TIMESTAMP
//            nSize += 3;
//            pPos += 3;
//        }
        if (bAntenna) {
            m_radiotap_data.nAntenna++;  
        }
    }
    m_radiotap_data.ts = pPkthdr->ts;
    //........

    return bRet;
}

/**
 * 解析ieee80211帧
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--radiotap长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseIeee80211(const struct pcap_pkthdr* pPkthdr, const  unsigned char* pPacket, uint16_t& nLen) {
    
    unsigned char*   pPos = NULL;
    int nSize = 0;
//    bool bRet = false;
    
    m_bEixstFrameParam = false;
    
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    nLen = 0;
    nSize = sizeof(ieee80211_frame_control);
    if (pPkthdr->len < nSize) {
        return false;
    }
    
    memset(&m_ieee80211_frame, 0x00, sizeof(IEEE80211_FRAME));
    
    pieee80211_frame_control pCtrlFrame = (pieee80211_frame_control)pPacket;
    uint8_t nType_Subtype = pCtrlFrame->nTypes;
    if (0 != (nType_Subtype & 0x03)) {//协议版本检查，目前，802.11 MAC 只有 一个版本；它的协议编号为 0
        return false;
    }
    
    uint8_t  nType = ((nType_Subtype >> 2) & 0x03);
    if (0 != nType /*&& 1 != it_type*/ && 2 != nType) { //检查是否是管理帧或数据帧
        return false;
    }
    uint8_t nSubtype = ((nType_Subtype >> 4) & 0x0F);
    uint8_t nDS = (pCtrlFrame->nFlags & 0x03);
    
    //保护标志位
    if ((pCtrlFrame->nFlags & 0x40) == 0x40) {
        m_ieee80211_frame.nProtected_Flag = 1;
    }
    
//    if (1 == it_type && 0x09 != it_subtype) {
//        return false;
//    }
    
    pPos = (unsigned char*)&pPacket[sizeof(ieee80211_frame_control)];
    nLen += sizeof(ieee80211_frame_control);
    
    //跳过Duration 
    nSize += 2;
    pPos += 2;
    nLen += 2;
    
    //读第一个mac
    nSize += 6;
    if (pPkthdr->len < nSize) {
        return false;
    }
    memcpy(m_ieee80211_frame.zAddress1, pPos, 6);
    pPos += 6;
    nLen += 6;
    
    //读取第二个mac
    nSize += 6;
    if (pPkthdr->len < nSize) {
        return false;
    }
    memcpy(m_ieee80211_frame.zAddress2, pPos, 6);
    pPos += 6;
    nLen += 6;
    
//    if (1 == it_type && 0x09 == it_subtype) {//Blocl Ack控制帧，只解析到第二个mac
//        m_ieee80211_frame.it_type = (it_type << 4);
//        m_ieee80211_frame.it_type = m_ieee80211_frame.it_type & 0x30;
//        m_ieee80211_frame.it_type += it_subtype;
//        m_ieee80211_frame.it_ds = 0;
//        return true;
//    }
    
    //读取第三个mac
    nSize += 6;
    if (pPkthdr->len < nSize) {
        return false;
    }
    memcpy(m_ieee80211_frame.zAddress3, pPos, 6);
    pPos += 6;
    nLen += 6;
    
    if (0 == nType) {
        if (0x00 == nSubtype || 0x04 == nSubtype || 0x05 == nSubtype ||
                0x08 == nSubtype || 0x0A == nSubtype || 0x0B == nSubtype ||
                0x0C == nSubtype) {
            //0x00:Association request（连接要求）, 0x04:Probe request（探查要求）,0x05:Probe response（探查应答）,
            //0x08:Beacon（导引信号）,0x0B:Authentication（身份验证），0x0A:Disassociation（解除连接），0x0C:Deauthentication（解除认证）
            
            //读FN和SN
            nSize += 2;
            if (pPkthdr->len < nSize) {
                return false;
            }
            uint16_t nSN_FN = 0;
            nSN_FN = GetLE16(pPos);
            //memcpy(&nSN_FN, pPos, 2);       
            pPos += 2;
            nLen += 2;
            m_ieee80211_frame.nFN = (0x000F & nSN_FN);
            m_ieee80211_frame.nSN = (0x0FFF & (nSN_FN >>  4));
            
            //读参数           
            if (pPkthdr->len - nLen > 0) {//存在参数
                if (!allocBufferOfParam(pPkthdr->len - nLen)) {
                    return false;
                }
                //保存帧参数
                m_ieee80211_frame_param.nLen = pPkthdr->len - nLen;
                memcpy(m_ieee80211_frame_param.pData, pPos, m_ieee80211_frame_param.nLen);
//                nLen += m_ieee80211_frame_param.nLen;
                m_bEixstFrameParam = true;
            }
        }
        m_ieee80211_frame.nType = nSubtype;
        m_ieee80211_frame.nDS = 0;        
    } else if (2 == nType) {        
        //读FN和SN
        nSize += 2;
        if (pPkthdr->len < nSize) {
            return false;
        }        
        uint16_t it_sn_fn = 0;
        //memcpy(&it_sn_fn, pPos, 2);       
        it_sn_fn = GetLE16(pPos);
        pPos += 2;
        nLen += 2;
        
        if (3 == nDS) {//WDS数据帧
            //读取第四个mac
            nSize += 6;
            if (pPkthdr->len < nSize) {
                return false;
            }
            memcpy(m_ieee80211_frame.zAddress4, pPos, 6);
            pPos += 6;
        }
        
        m_ieee80211_frame.nFN = (0x000F & it_sn_fn);
        m_ieee80211_frame.nSN = (0x0FFF & (it_sn_fn >>  4));

        m_ieee80211_frame.nType = (nType << 4);
        m_ieee80211_frame.nType = m_ieee80211_frame.nType & 0x30;
        m_ieee80211_frame.nType += nSubtype;
        m_ieee80211_frame.nDS = nDS;
    } else {
        return false;
    } 
    
    return true;
}

/**
 * 解析ieee80211帧参数
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--radiotap长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseIeee80211Param(const struct pcap_pkthdr* pPkthdr, const  unsigned char* pPacket, uint16_t& nLen) {
    
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }  
    nLen = 0;
    m_ieee80211_frame_parse_param.ssid.bExisted = false;
    m_ieee80211_frame_parse_param.ssid.nHide = -1;
    memset(m_ieee80211_frame_parse_param.ssid.szSSID, 0x00, 66);
    m_ieee80211_frame_parse_param.channel.bExisted = false;
    m_ieee80211_frame_parse_param.channel.nChannel = 0;
    m_ieee80211_frame_parse_param.privacy.bExisted = false;
    m_ieee80211_frame_parse_param.privacy.nPrivacy = 0;
    m_ieee80211_frame_parse_param.encrypt.bExisted = false;
    m_ieee80211_frame_parse_param.encrypt.nEncrypt = 0;
    m_ieee80211_frame_parse_param.auth.bExisted = false;
    m_ieee80211_frame_parse_param.auth.nAuth = 0;
    m_ieee80211_frame_parse_param.wps.bExisted = false;
    m_ieee80211_frame_parse_param.wps.nWPS = 0;
    
    if (FRAME_TYPES_PROBE_RESPONSE == m_ieee80211_frame.nType) {//0x05:探查应答 AP
        return parseBeaconOrProbeResponseParam(pPkthdr, pPacket, nLen, 2);               
    } if (FRAME_TYPES_BEACON == m_ieee80211_frame.nType) { //0x08:导引信号 AP
        return parseBeaconOrProbeResponseParam(pPkthdr, pPacket, nLen, 1);
    } else if (FRAME_TYPES_ASSOCIATION_REQUEST == m_ieee80211_frame.nType) {//0x00:连接要求  station
        return parseAssociationRequestParam(pPkthdr, pPacket, nLen);
    } else if (FRAME_TYPES_PROBE_REQUEST == m_ieee80211_frame.nType) {//0x04:探查请求
        return parseProbeRequestParam(pPkthdr, pPacket, nLen);
    } else if (m_ieee80211_frame.nType >= FRAME_TYPES_DATA_DATA && 
                m_ieee80211_frame.nType <= FRAME_TYPES_DATA_QOS_CF_ACK_POLL) {//是数据帧
        return parseDataParam(pPkthdr, pPacket, nLen);            
    } 
    
    return false;
}

/**
 * 解析beacon包参数
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--radiotap长度
 * @param nType--1:beacon, 2:Probe Response
 * @return true：成功，false：失败
 */
bool CIeee80211::parseBeaconOrProbeResponseParam(const struct pcap_pkthdr* pPkthdr, const  unsigned char* pPacket, 
        uint16_t& nLen, int nType) {
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    if (1 != nType && 2 != nType) {
        return false;
    }
    bool bRet = false;
    int nOffset = 0;
    nLen = 0;
    pfixed_parameters_12 pFixedparameters_12 = (pfixed_parameters_12)(&pPacket[nOffset]);
    nOffset += sizeof(fixed_parameters_12);    
    if (nOffset > pPkthdr->len) {
        return false;
    }
    nLen += sizeof(fixed_parameters_12);
    
    while (nOffset < pPkthdr->len) {
        ptagged_pamaeters_tag  pTag = (ptagged_pamaeters_tag)(&pPacket[nOffset]);
        nOffset += (sizeof(tagged_pamaeters_tag) + pTag->nLength);
        if (nOffset > pPkthdr->len) {
            break;
        }
        nLen += pTag->nLength;
        switch (pTag->nTag) {
            case 0x00: {//SSID
                if (1 == nType) {//beacon
                    if (pTag->nLength > 0) {
                        if (0x00 != pTag->zValue[0]) {//显示SSID
                            m_ieee80211_frame_parse_param.ssid.nHide = 0;
                            if (pTag->nLength <= 64) {
                                memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, pTag->nLength);
                            } else {
                                memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, 64);
                            }
                        } else {//隐藏SSID
                            m_ieee80211_frame_parse_param.ssid.nHide = 1;
                        }
                    } else {//隐藏SSID
                        m_ieee80211_frame_parse_param.ssid.nHide = 1;
                    }
                    m_ieee80211_frame_parse_param.ssid.bExisted = true;
                    bRet = true;
                } else {
                    if (pTag->nLength > 0) {
                        if (pTag->nLength <= 64) {
                            memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, pTag->nLength);
                        } else {
                            memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, 64);
                        }
                        m_ieee80211_frame_parse_param.ssid.bExisted = true;
                        bRet = true;
                    }
                }               
                break;
            }
            case 0x03: {//信道
                if (1 == pTag->nLength) {
                    m_ieee80211_frame_parse_param.channel.bExisted = true;
                    m_ieee80211_frame_parse_param.channel.nChannel = pTag->zValue[0];
                    bRet = true;
                }
                break;
            }
            case 0xDD: {//WAP保护方式或WPS                
                if (pTag->nLength >= 8) {//WAP
                    if (0 == memcmp(pTag->zValue, "\x00\x50\xF2\x01\x01\x00", 6)) {                        
                        m_ieee80211_frame_parse_param.privacy.bExisted = true;
                        m_ieee80211_frame_parse_param.privacy.nPrivacy |= PRIVACY_WPA;
                        bRet = true;
                        //解析加密算法和认证方式
                        parseEncryptAuth(&pTag->zValue[10], pTag->nLength - 10);                         
                    } else if (0 == memcmp(pTag->zValue, "\x00\x50\xF2\x04", 4)) {
                        parseWPS(&pTag->zValue[4], pTag->nLength - 4);                        
                    }                   
                } else if (pTag->nLength >= 4){//WPS
                    if (0 == memcmp(pTag->zValue, "\x00\x50\xF2\x04", 4)) {
                        parseWPS(&pTag->zValue[4], pTag->nLength - 4);                        
                    }  
                }                
                break;
            }
            case 0x30: {
                m_ieee80211_frame_parse_param.privacy.bExisted = true;
                m_ieee80211_frame_parse_param.privacy.nPrivacy |= PRIVACY_WPA2;
                bRet = true;
                //解析加密算法和认证方式
                parseEncryptAuth(&pTag->zValue[6], pTag->nLength - 6);                           
                break;
            }
        }
    } 
    if (!m_ieee80211_frame_parse_param.privacy.bExisted) {
        if ((pFixedparameters_12->nCapabilities_info & 0x0010) == 0x0010) {
            m_ieee80211_frame_parse_param.privacy.bExisted = true;
            m_ieee80211_frame_parse_param.privacy.nPrivacy = PRIVACY_WEP;
            m_ieee80211_frame_parse_param.encrypt.bExisted = true;
            m_ieee80211_frame_parse_param.encrypt.nEncrypt = ENCRYPT_WEP40 | ENCRYPT_WEP104;
            m_ieee80211_frame_parse_param.auth.bExisted = true;
            m_ieee80211_frame_parse_param.auth.nAuth = AUTH_PSK;
            bRet = true;
        } else {
            m_ieee80211_frame_parse_param.privacy.bExisted = true;
            m_ieee80211_frame_parse_param.privacy.nPrivacy = PRIVACY_NO;
            m_ieee80211_frame_parse_param.encrypt.bExisted = true;
            m_ieee80211_frame_parse_param.encrypt.nEncrypt = ENCRYPT_NO;
            m_ieee80211_frame_parse_param.auth.bExisted = true;
            m_ieee80211_frame_parse_param.auth.nAuth = AUTH_NO;
            bRet = true;
        }       
    }
    m_ieee80211_frame_parse_param.wps.bExisted = true;
    
    return bRet;
} 

/**
 * 解析加密算法和认证方式
 * @param pPacket--包数据
 * @param nSize--包数据长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseEncryptAuth(const  unsigned char* pPacket, int nSize) {
    
    if (NULL == pPacket || nSize <= 0) {
        return false;
    }
    int nOffsetChild = 0; 
    uint16_t nCounts = 0;
    //memcpy(&nCounts, &pPacket[nOffsetChild], 2);
    nCounts = GetLE16((void*)(pPacket + nOffsetChild));
    nOffsetChild += 2;
    while (nOffsetChild + 4 <= nSize && nCounts > 0) {
        switch (pPacket[nOffsetChild + 3]) {
            case 1: {//WEP40
                m_ieee80211_frame_parse_param.encrypt.bExisted = true;
                m_ieee80211_frame_parse_param.encrypt.nEncrypt |= ENCRYPT_WEP40;
                break;
            }
            case 2: {//TKIP
                m_ieee80211_frame_parse_param.encrypt.bExisted = true;
                m_ieee80211_frame_parse_param.encrypt.nEncrypt |= ENCRYPT_TKIP;
                break;
            }
            case 3: {//WRAP
                m_ieee80211_frame_parse_param.encrypt.bExisted = true;
                m_ieee80211_frame_parse_param.encrypt.nEncrypt |= ENCRYPT_WRAP;
                break;
            }
            case 4: {//CCMP
                m_ieee80211_frame_parse_param.encrypt.bExisted = true;
                m_ieee80211_frame_parse_param.encrypt.nEncrypt |= ENCRYPT_CCMP;
                break;
            }
            case 5: {//WEP104
                m_ieee80211_frame_parse_param.encrypt.bExisted = true;
                m_ieee80211_frame_parse_param.encrypt.nEncrypt |= ENCRYPT_WEP104;
                break;
            }
        }
        nCounts --;
        nOffsetChild += 4;
    }
    //解析认证方式
    nCounts = 0;
    //memcpy(&nCounts, &pPacket[nOffsetChild], 2);
    nCounts = GetLE16((void*)(pPacket + nOffsetChild));
    nOffsetChild += 2;
    while (nOffsetChild + 4 <= nSize && nCounts > 0) {
        switch (pPacket[nOffsetChild + 3]) {
            case 1: {
                m_ieee80211_frame_parse_param.auth.bExisted = true;
                m_ieee80211_frame_parse_param.auth.nAuth |= AUTH_MGT;
                break;
            }
            case 2: {
                m_ieee80211_frame_parse_param.auth.bExisted = true;
                m_ieee80211_frame_parse_param.auth.nAuth |= AUTH_PSK;
                break;
            }                                
        }
        nCounts --;
        nOffsetChild += 4;
    } 
    
    return m_ieee80211_frame_parse_param.encrypt.bExisted || m_ieee80211_frame_parse_param.auth.bExisted;
}

/**
 * 解析WPS
 * @param pPacket--包数据
 * @param nSize--包数据长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseWPS(const unsigned char* pPacket, int nSize) {
    
    bool bRet = false;    
    int nOffsetChild = 0;
    ptagged16_pamaeters_tag  pTag16 = NULL;
    bool bExistState = false;
    bool bExistLocked = false;
    int8_t nState = 0;
    int8_t nLocked = 0;
    int8_t nMeth = 0;
    while (nOffsetChild < nSize) {
        pTag16 = (ptagged16_pamaeters_tag)(&pPacket[nOffsetChild]);
        //nOffsetChild += (sizeof(tagged16_pamaeters_tag) + ((pTag16->nLength >> 8) & 0x00FF) +  ((pTag16->nLength << 8) & 0xFF00));
    
        nOffsetChild += (sizeof(tagged16_pamaeters_tag) + GetLE16(&pTag16->nLength));
        if (nOffsetChild > nSize) {
            break;
        }
        uint16_t unTag = GetLE16(&pTag16->nTag);
        if (0x1044 == unTag) { //0x1044
            bExistState = true;
            nState = pTag16->zValue[0];
        } else if (0x1057 == unTag) { //0x1057
            bExistLocked = true;
            nLocked = pTag16->zValue[0];
        } else if (0x1053 == unTag) { //0x1053
            nMeth = (pTag16->zValue[0] << 8) + pTag16->zValue[1];
        }
    }
    if (bExistState && ((uint8_t)nState != 0xFF)) {
        bRet = true;
        m_ieee80211_frame_parse_param.wps.bExisted = true;
        if (bExistLocked || 1 != nLocked) {
            m_ieee80211_frame_parse_param.wps.nWPS = 1;
        } else {
            m_ieee80211_frame_parse_param.wps.nWPS = 0;
        }
    } else if (bExistLocked) {
        bRet = true;
        m_ieee80211_frame_parse_param.wps.bExisted = true;
        if (1 != nLocked) {
            m_ieee80211_frame_parse_param.wps.nWPS = 1;
        } else {
            m_ieee80211_frame_parse_param.wps.nWPS = 0;
        }
    }
    
    return bRet;
}

/**
 * 解析Association Request包参数
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--被解析调数据的长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseAssociationRequestParam(const struct pcap_pkthdr* pPkthdr, const  unsigned char* pPacket, uint16_t& nLen) {
    
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    int nOffset = 4;   
    nLen = 0;
    if (nOffset >= pPkthdr->len) {
        return false;
    }
    nLen = 4;
    while (nOffset < pPkthdr->len) {
        ptagged_pamaeters_tag  pTag = (ptagged_pamaeters_tag)(&pPacket[nOffset]);
        nOffset += (sizeof(tagged_pamaeters_tag) + pTag->nLength);
        if (nOffset > pPkthdr->len) {
            break;
        }
        nLen += pTag->nLength;
        switch (pTag->nTag) {
            case 0x00: {//SSID
                if (pTag->nLength > 0) {
                    if (pTag->nLength <= 64) {
                        memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, pTag->nLength);
                    } else {
                        memcpy(m_ieee80211_frame_parse_param.ssid.szSSID, pTag->zValue, 64);
                    }
                    m_ieee80211_frame_parse_param.ssid.bExisted = true;
                    return true;
                }              
                break;
            }
        }
    } 
    
    return false;
}

/**
 * 解析Probe Request包参数
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--被解析调数据的长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseProbeRequestParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen) {
    
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    int nOffset = 0;   
    nLen = 0;
    while (nOffset < pPkthdr->len) {
        ptagged_pamaeters_tag  pTag = (ptagged_pamaeters_tag)(&pPacket[nOffset]);
        nOffset += (sizeof(tagged_pamaeters_tag) + pTag->nLength);
        if (nOffset > pPkthdr->len) {
            break;
        }
        nLen += pTag->nLength;
        switch (pTag->nTag) {
            case 0x03: {//channel
                if (1 == pTag->nLength) {
                    m_ieee80211_frame_parse_param.channel.bExisted = true;
                    m_ieee80211_frame_parse_param.channel.nChannel = pTag->zValue[0];
                    return true;
                }      
                break;
            }
        }
    } 
    
    return false;
}

/**
 * 解析Data包参数
 * @param pPkthdr--包头
 * @param pPpacket--包数据
 * @param nLen--被解析调数据的长度
 * @return true：成功，false：失败
 */
bool CIeee80211::parseDataParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen) {
    if (NULL == pPkthdr || NULL == pPacket) {
        return false;
    }
    int nOffset = 0;
    nLen = 0;
    if (FRAME_TYPES_DATA_QOS_DATA == m_ieee80211_frame.nType) {//跳过QoS
        nOffset += 2;
        nLen += 2;
    }
    if (nOffset + 26 > pPkthdr->len) {
        return false;
    }
    if (1 != m_ieee80211_frame.nProtected_Flag) {
        return false;
    }
    if (0x20 == (pPacket[nOffset + 3] & 0x20)) {//WPA
        m_ieee80211_frame_parse_param.privacy.bExisted = true;
        m_ieee80211_frame_parse_param.privacy.nPrivacy = PRIVACY_WPA;
    } else {//WEP
        m_ieee80211_frame_parse_param.privacy.bExisted = true;
        m_ieee80211_frame_parse_param.privacy.nPrivacy = PRIVACY_WEP;
        if (0x00 != (pPacket[nOffset + 3] & 0xC0)) {
            m_ieee80211_frame_parse_param.encrypt.bExisted = true;
            m_ieee80211_frame_parse_param.encrypt.nEncrypt = ENCRYPT_WEP40;
        } else {
            m_ieee80211_frame_parse_param.encrypt.bExisted = true;
            m_ieee80211_frame_parse_param.encrypt.nEncrypt = ENCRYPT_WEP40 | ENCRYPT_WEP104;
        }
    }
    nLen += 8;
    
    return true;    
}

/**
 * 80211 freq转信道
 * @param freq
 * @return 
 */
int CIeee80211::ieee80211_frequency_to_channel(int nFreq) {
    /* see 802.11-2007 17.3.8.3.2 and Annex J */
    if (nFreq == 2484) {
            return 14;
    }
    else if (nFreq < 2484) {
            return (nFreq - 2407) / 5; 
    }
    else if (nFreq >= 4910 && nFreq <= 4980) {
            return (nFreq - 4000) / 5;
    }
    else if (nFreq <= 45000)  {/* DMG band lower limit */
            return (nFreq - 5000) / 5;
    }
    else if (nFreq >= 58320 && nFreq <= 64800) {
            return (nFreq - 56160) / 2160;
    }
    
    return 0;
}

/**
 * 分配参数缓冲区
 * @param it_size--
 * @return true：成功，false：失败
 */
bool CIeee80211::allocBufferOfParam(uint32_t nSize) {
    if (nSize <= 0 || nSize > 65535) {
        return false;
    }
    if (0 == m_ieee80211_frame_param.nMax || NULL == m_ieee80211_frame_param.pData ||
            m_ieee80211_frame_param.nMax < nSize) {
        //释放空间
        if (NULL != m_ieee80211_frame_param.pData) {
            free(m_ieee80211_frame_param.pData);
            m_ieee80211_frame_param.pData = NULL;
            m_ieee80211_frame_param.nMax = 0;
            m_ieee80211_frame_param.nLen = 0;
        }
            
        if (nSize < 256) {
            m_ieee80211_frame_param.pData = (unsigned char*)malloc(256);
            if (NULL == m_ieee80211_frame_param.pData) {
                return false;
            }
            m_ieee80211_frame_param.nLen = 0;
            m_ieee80211_frame_param.nMax = 256;
        } else {
            m_ieee80211_frame_param.pData = (unsigned char*)malloc(nSize);
            if (NULL == m_ieee80211_frame_param.pData) {
                return false;
            }
            m_ieee80211_frame_param.nLen = 0;
            m_ieee80211_frame_param.nMax = nSize;
            memset(m_ieee80211_frame_param.pData, 0x00, m_ieee80211_frame_param.nMax);
        }
    } else {
        m_ieee80211_frame_param.nLen = 0;
        memset(m_ieee80211_frame_param.pData, 0x00, m_ieee80211_frame_param.nMax);
    }
    
    return true;
}

//打印解析结果
void CIeee80211::printResult() {
    DebugPrint03("-----------Ieee80211 parase result-----------------\r\n");
    DebugPrint03("Type: 0x%02X\r\n", m_ieee80211_frame.nType);
    DebugPrint03("DS: %d\r\n", m_ieee80211_frame.nDS);
    DebugPrint03("Protected_Flag: %d\r\n", m_ieee80211_frame.nProtected_Flag);
    DebugPrint03("mac1: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_ieee80211_frame.zAddress1[0], m_ieee80211_frame.zAddress1[1],
            m_ieee80211_frame.zAddress1[2], m_ieee80211_frame.zAddress1[3], m_ieee80211_frame.zAddress1[4],
            m_ieee80211_frame.zAddress1[5]);
    DebugPrint03("mac2: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_ieee80211_frame.zAddress2[0], m_ieee80211_frame.zAddress2[1],
            m_ieee80211_frame.zAddress2[2], m_ieee80211_frame.zAddress2[3], m_ieee80211_frame.zAddress2[4],
            m_ieee80211_frame.zAddress2[5]);
    DebugPrint03("mac3: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_ieee80211_frame.zAddress3[0], m_ieee80211_frame.zAddress3[1],
            m_ieee80211_frame.zAddress3[2], m_ieee80211_frame.zAddress3[3], m_ieee80211_frame.zAddress3[4],
            m_ieee80211_frame.zAddress3[5]);
    DebugPrint03("mac4: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_ieee80211_frame.zAddress4[0], m_ieee80211_frame.zAddress4[1],
            m_ieee80211_frame.zAddress4[2], m_ieee80211_frame.zAddress4[3], m_ieee80211_frame.zAddress4[4],
            m_ieee80211_frame.zAddress4[5]);    
    DebugPrint03("Channel: %d\r\n", m_radiotap_data.nChannel);
    DebugPrint03("nSignal: %d\r\n", m_radiotap_data.nSignal);
    if (m_ieee80211_frame_parse_param.auth.bExisted) {
        DebugPrint("Auth: %d\r\n", m_ieee80211_frame_parse_param.auth.nAuth);
    }
    if (m_ieee80211_frame_parse_param.channel.bExisted) {
        DebugPrint03("Param Channel: %d\r\n", m_ieee80211_frame_parse_param.channel.nChannel);
    }
    if (m_ieee80211_frame_parse_param.encrypt.bExisted) {
        DebugPrint03("Encrypt: 0x%02X\r\n", m_ieee80211_frame_parse_param.encrypt.nEncrypt);
    }
    if (m_ieee80211_frame_parse_param.privacy.bExisted) {
        DebugPrint03("Privacy: 0x%02X\r\n", m_ieee80211_frame_parse_param.privacy.nPrivacy);
    }
    if (m_ieee80211_frame_parse_param.ssid.bExisted) {
        DebugPrint03("Hide SSID: %d\r\n", m_ieee80211_frame_parse_param.ssid.nHide);
        DebugPrint03("SSID: %s\r\n", m_ieee80211_frame_parse_param.ssid.szSSID);
    }
    if (m_ieee80211_frame_parse_param.wps.bExisted) {
        DebugPrint03("WPS: %d\r\n", m_ieee80211_frame_parse_param.wps.nWPS);
    }
    DebugPrint03("-----------Ieee80211 parase result-----------------\r\n");
}
