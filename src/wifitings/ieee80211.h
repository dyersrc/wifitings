/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ieee80211.h
 * Author: dyer
 *
 * Created on 2016年6月25日, 下午1:00
 */

#ifndef IEEE80211_H
#define IEEE80211_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pcap.h>
#include "../include/wifitings.h"
//#include "../include/define.h"


#pragma pack(push, 1)

//typedef struct __ieee80211_frame {//ieee80211帧
//    uint8_t            nType;                   //帧类型
//    uint8_t            nDS;                      //TO DS 与 与 From DSbit
//    uint8_t            nProtected_Flag;     //保护标志
//    uint8_t            zAddress1[6];          //mac1
//    uint8_t            zAddress2[6];          //mac2
//    uint8_t            zAddress3[6];          //mac3
//    uint8_t            zAddress4[6];          //mac4
//    uint16_t           nFN;
//    uint16_t           nSN;
//    
//}ieee80211_frame, *pieee80211_frame;

typedef struct __ieee80211_frame_parse_param {//ieee80211帧参数解析结果
    //SSID
    struct {
        bool                   bExisted; //true：存在
        int8_t                nHide;     //-1:不确定，1:隐藏SSID，0：显示SSID
        char                   szSSID[66];
    }ssid;    
    //信道
    struct {
        bool                   bExisted; //true：存在
        int8_t                nChannel;
    }channel;   
    //保护方式
    struct {
        bool                   bExisted; //true：存在
        int8_t                nPrivacy;  //0x00:不加密,0x01:WEP,0x02:WPA,0x03:WEP|WPA,0x04:WPA2,0x05:WEP|WPA2,0x06:WPA|WPA2,0x07:WEP|WPA|WPA2
    }privacy;
    //加密算法
    struct {
        bool                   bExisted; //true：存在
        int8_t                nEncrypt; //0:不加密,1:WEP40,2:WEP104,3:WEP40|WEP104,4:TKIP,8:CCMP,12:TKIP|CCMP
    }encrypt;
    //认证方式 
    struct {
        bool                   bExisted; //true：存在
        int8_t                nAuth; //0:不认证,1:PSK,2:RADIUS
    }auth;
    //WPS状态
    struct {
        bool                   bExisted; //true：存在
        int8_t                nWPS;    //0:没有开启WPS,1:启用WPS
    }wps;
    
}IEEE80211_FRAME_PARSE_PARAM, *PIEEE80211_FRAME_PARSE_PARAM;

//typedef struct __ieee80211_frame_param {//ieee80211帧参数据
//    uint32_t          nMax;                    //缓冲区最大值
//    uint32_t          nLen;                    //数据长度
//    unsigned char*  pData;                   //数据
//    
//}ieee80211_frame_param, *pieee80211_frame_param;


#pragma pack(pop)

class CIeee80211 {
    CIeee80211(const CIeee80211& orig);
public:
    CIeee80211();
    ~CIeee80211();
    
    //解析
    bool parse(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPpacket);
    
    //得到radiotap
    PRADIOAP_DATA getRadiotap();   
    
    //得到ieee80211
    PIEEE80211_FRAME getIeee80211();
    
    //得到ieee80211参数
    PIEEE80211_FRAME_PARAM getIeee80211Param();
    
    //得到ieee80211解析参数
    PIEEE80211_FRAME_PARSE_PARAM getIeee80211ParseParam();
    
    
protected:
    //解析radiotap
    bool parseRadiotap(const struct pcap_pkthdr* pPkthdr,  const unsigned char* pPacket, uint16_t& nLen);
    
    //解析ieee80211帧
    bool parseIeee80211(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen);
    
    //解析ieee80211帧参数
    bool parseIeee80211Param(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen);
    
    //解析beacon包参数
    bool parseBeaconOrProbeResponseParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen, int nType);
    
    //解析加密算法和认证方式
    bool parseEncryptAuth(const unsigned char* pPacket, int nSize);
    
    //解析WPS
    bool parseWPS(const unsigned char* pPacket, int nSize);
    
    //解析Association Request包参数
    bool parseAssociationRequestParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen);
    
    //解析Probe Request包参数
    bool parseProbeRequestParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen);
    
    //解析Data包参数
    bool parseDataParam(const struct pcap_pkthdr* pPkthdr, const unsigned char* pPacket, uint16_t& nLen);
    
    //解析身份
    bool parseIdentity();   
    
    //80211 freq转信道
    int ieee80211_frequency_to_channel(int nFreq);
    
    //分配参数缓冲区
    bool allocBufferOfParam(uint32_t nSize);
    
    //打印解析结果
    void printResult();

public:
#define SG_IEEE80211      0x65656569                  //ieee
    unsigned long               m_ulSignature;             //签名 = SG_IEEE80211
    
protected:
    RADIOAP_DATA           m_radiotap_data;          //radiotap解析结果
    IEEE80211_FRAME          m_ieee80211_frame;      //ieee80211帧解析结果
    IEEE80211_FRAME_PARAM m_ieee80211_frame_param;//ieee80211帧原始参数数据，Sequence number后面的数据
    IEEE80211_FRAME_PARSE_PARAM m_ieee80211_frame_parse_param;//ieee80211帧参数解析结果
    bool                           m_bParseSuccess;         //解析成功
    bool                           m_bEixstParseParam;     //解析帧参数成功
    bool                           m_bEixstFrameParam;    //存在帧参数数据
};

#endif /* IEEE80211_H */

