/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wifitings.h
 * Author: dyer
 *
 * Created on 2017年1月22日, 下午8:06
 */

#ifndef WIFITINGS_H
#define WIFITINGS_H

#include <stdint.h>
#include <sys/time.h>

#define FRAME_TYPES_ASSOCIATION_REQUEST            0x00   //连接要求  station
#define FRAME_TYPES_ASSOCIATION_RESPONSE           0x01   //连接应答 AP
#define FRAME_TYPES_REASSOCIATIOM_REQUEST         0x02   //重新连接要求 station 
#define FRAME_TYPES_REASSOCIATIOM_RESPONSE        0x03   //重新连接应答 AP 
#define FRAME_TYPES_PROBE_REQUEST                       0x04   //探查要求 station
#define FRAME_TYPES_PROBE_RESPONSE                      0x05   //探查应答 AP
#define FRAME_TYPES_BEACON                                   0x08   //导引信号 AP
#define FRAME_TYPES_ANNOUNCEMENT_TRAFFIC          0x09   //数据代传指示通知信号 ??
#define FRAME_TYPES_DISASSOCIATION                       0x0A   //解除连接 AP | station
#define FRAME_TYPES_AUTHENTICATION                     0x0B   //身份验证  AP | station
#define FRAME_TYPES_DEAUTHENTICATION                  0x0C   //解除认证 AP | station

#define FRAME_TYPES_DATA_DATA                           0x20   //数据
#define FRAME_TYPES_DATA_DATA_CF_ACK              0x21   //Data+CF-Ack
#define FRAME_TYPES_DATA_DATA_CF_POLL            0x22   //Data+CF-Ack+CF-Poll
#define FRAME_TYPES_DATA_DATA_CF_ACK_POLL    0x23   //Data+CF-Ack+CF-Poll
#define FRAME_TYPES_DATA_DATA_NULL                 0x24   //Null data (无数据：未发送数据)
#define FRAME_TYPES_DATA_CF_ACK                       0x25   //CF-Ack (未发送数据)
#define FRAME_TYPES_DATA_CF_POLL                      0x26   //CF-Poll (未发送数据)
#define FRAME_TYPES_DATA_CF_ACK_POLL              0x27   //CF-Ack+CF-Poll
#define FRAME_TYPES_DATA_QOS_DATA                   0x28   //QoS Data
#define FRAME_TYPES_DATA_QOS_DATA_CF_ACK     0x29   //QoS Data
#define FRAME_TYPES_DATA_QOS_DATA_CF_POLL    0x2A   //QoS Data
#define FRAME_TYPES_DATA_QOS_DATA_CF_ACK_POLL 0x2B   //QoS Data
#define FRAME_TYPES_DATA_QOS_DATA_NULL         0x2C   //QoS Data
#define FRAME_TYPES_DATA_QOS_CF_ACK               0x2D   //QoS Data
#define FRAME_TYPES_DATA_QOS_CF_POLL              0x2E   //QoS Data
#define FRAME_TYPES_DATA_QOS_CF_ACK_POLL      0x2F   //QoS Data

#define FRAME_TYPES_CONTROL_BLOCK_ACK            0x19   //Block ack

#define FRAME_DS_00                                              0x00   //所有管理与控制帧 IBSS (非基础型数据帧)
#define FRAME_DS_01                                              0x01    //基础网络里无线工作站所发送的数据帧
#define FRAME_DS_10                                              0x02   //基础网络里无线工作站所收到的数据帧
#define FRAME_DS_11                                              0x03   //无线桥接器上的数据帧

#define PRIVACY_NO                                               0x00   //加密方式：不加密
#define PRIVACY_WEP                                             0x01   //加密方式：wep
#define PRIVACY_WPA                                             0x02   //加密方式：wpa
#define PRIVACY_WEP_WPA                                     0x03   //加密方式：wep或wpa
#define PRIVACY_WPA2                                           0x04   //加密方式：wpa2
#define PRIVACY_WEP_WPA2                                   0x05   //加密方式：wep或wpa2
#define PRIVACY_WPA_WPA2                                   0x06   //加密方式：wpa或wpa2
#define PRIVACY_WEP_WPA_WPA2                           0x07   //加密方式：wep或wpa或wpa2

#define ENCRYPT_NO                                               0x00
#define ENCRYPT_WEP40                                          0x01
#define ENCRYPT_WEP104                                         0x02
#define ENCRYPT_TKIP                                             0x04
#define ENCRYPT_CCMP                                            0x08
#define ENCRYPT_WRAP                                            0x10

#define AUTH_NO                                                    0x00
#define AUTH_PSK                                                   0x01
#define AUTH_MGT                                                  0x02

#define IDENTITY_AP                                               1        //身份类型 AP
#define IDENTITY_STATION                                      2        //身份类型 station
#define IDENTITY_BRIDGE                                         3        //身份类型 bridge
#define IDENTITY_BRIDGE_STATION                          4        //身份类型 bridge下的station
#define IDENTITY_BRIDGE_AP                                   5        //身份类型 bridge下的AP
#define IDENTITY_BRIDGE_UNKNOW                           6        //身份类型 bridge未知
#define IDENTITY_UNKNOW                                       7        //未知

#pragma pack(push, 1)

typedef struct __tag_wireless_identity {
    int               nAdapterId;            //网卡ID,-1:未设置
    uint8_t        unIdentity;              //1:AP, 2:station,3:bridge，4：bridge下的station，5：bridge下的AP，6：bridge未知，7：未知
    uint8_t        zMac[6];                 //无线设备MAC地址  
    uint8_t        unChanged;            //身份信息发生了变化，除信道和信号以外的信息发生变化,1:变化，0：未变化
    uint8_t        unOnline;                //1：在线，0：离线
    struct __tag_ap {
        int8_t     nPrivacy;                //保护方式 -1：未确认， 0x00:不加密,0x01:WEP,0x02:WPA,0x03:WEP|WPA,0x04:WPA2,0x05:WEP|WPA2,0x06:WPA|WPA2,0x07:WEP|WPA|WPA2
        int8_t     nEncrypt;               //加密算法 -1：未确认，0:不加密,1:WEP40,2:WEP104,3:WEP40|WEP104,4:TKIP,8:CCMP,12:TKIP|CCMP
        int8_t     nAuth;                  //认证方式 -1：未确认，0:不认证,1:PSK,2:RADIUS
        int8_t     nWPS;                   //-1：未确认，1：开启WPS，0：不开启WPS
        int8_t     nHideSSID;            //隐藏SSID,1:隐藏，0：不隐藏 ，-1：未确认，
        char        szSSID[66];
    }ap;
    struct __tag_station {
        int8_t    nConnected;      //连接AP标志,1:连接，0：未连接
        uint8_t   zMacOfAP[6];       //无线设备连接AP的MAC地址
    }station;
    int8_t        nChannel;              //无线设备实际信道，-1：未确认，
    int8_t        nSignal;                //无线设备有效信号，-127：未确认，
    
}WIRELESS_IDENTITY, *PWIRELESS_IDENTITY;

typedef struct __antenna {
    uint8_t       nId;                     //天线ID
    int8_t         nSignal;              //信号
    
}ANTENNA,*PANTENNA;

typedef struct __radiotap_data {//radiotap
    struct timeval      ts;	                    //time stamp
    int8_t                nChannel;           //解析信道
    uint16_t             nChannel_flags;   //信道标识
    int8_t                nSignal;              //天线信号强度，单位dBm
    int8_t                nNoise;              //天线信噪强度,单位dBm
    int8_t                nAntenna;          //天线数，最大支持4根天线
    ANTENNA           zAntenna[4];
    
}RADIOAP_DATA, *PRADIOAP_DATA;

typedef struct __ieee80211_frame {  
    
    uint8_t            nType;                   //帧类型
    uint8_t            nDS;                      //TO DS 与 与 From DSbit
    uint8_t            nProtected_Flag;     //保护标志
    uint8_t            zAddress1[6];          //mac1
    uint8_t            zAddress2[6];          //mac2
    uint8_t            zAddress3[6];          //mac3
    uint8_t            zAddress4[6];          //mac4
    uint16_t           nFN;
    uint16_t           nSN;
    
}IEEE80211_FRAME, *PIEEE80211_FRAME;

typedef struct __ieee80211_frame_param {//ieee80211帧参数 
    
    uint32_t          nMax;                    //缓冲区最大值
    uint32_t          nLen;                    //数据长度
    unsigned char*  pData;                   //数据
    
}IEEE80211_FRAME_PARAM, *PIEEE80211_FRAME_PARAM;

#pragma pack(pop)

//帧数据回调函数定义
typedef void (*PFRAME_CALLBACK)(
        void* pObject,//注册回调函数时输入的对象
        PWIRELESS_IDENTITY pWirelessIdentity, //当前数据帧发送者的身份信息
        PRADIOAP_DATA pRadioapData,//当前数据帧radioap部分的信息
        PIEEE80211_FRAME pIeee80211Frame,//当前数据帧80211部分的信息
        PIEEE80211_FRAME_PARAM pIeee80211Param,//80211的参数部分
        const void* pOriginalData, //从网卡上抓到的原始数据
        int nOriginalSize//原始数据长度
); 

class CWifiTings {    
public:
    /**
     * 设置系统根目录
     * @param pszRootDir--系统根目录
     * @return true：成功，false：失败
     */
    virtual bool setRootDir(const char* pszRootDir) = 0;
    
    /**
     * 设置网卡ID
     * @param nId--网卡ID
     * @return true：成功，false：失败
     */
    virtual bool setAdapterId(int nId) = 0;
    
    /**
     * 设置网卡名
     * @param pszAdapterName--网卡名
     * @return true:成功，false：失败
     */
    virtual bool setAdapterName(const char* pszAdapterName) = 0;
    
    /**
     * 获取网卡名
     * @return NULL：失败返回
     */
    virtual char* getAdapterName() = 0;
    
    /**
     * 获取WIFI设备身份信息
     * @param zMac--MAC地址
     * @param wirelessIdentity--接收身份信息
     * @return true:成功，false：失败
     */
    virtual bool getWirelessIdentity(uint8_t zMac[6],  WIRELESS_IDENTITY& wirelessIdentity) = 0;
    
    /**
     * 设置频段
     * @param nFreq--2：2.4G，5：5G
     * @return true：成功，false：false
     */
    virtual bool setFrequencyChanne(int nFreq) = 0;
    
    /**
     * 设置预设的频段
     * @return -1：未设置， 2：2.4G，5：5G
     */
    virtual int getFrequencyChanne() = 0;
    
    /**
     * 得到网卡工作状态
     * @return -1：失败，0:正常，1：抓包模块已经停止，2：信道管理线程已经停止,3:模块已经停止
     */
    virtual int getWorkState() = 0;
    
    /**
     * 设置网卡工作信道列表
     * @param zListChannel--信道列表,最大支持13个
     * @param nCounts--实际输入信道数
     * @return true：设置成功
     */
    virtual bool setChannel(int8_t zListChannel[], int nCounts) = 0;
    
    /**
     * 得到网卡当前信道
     * @return 0：失败
     */
    virtual int8_t getChannel() = 0;
    
    /**
     * 设置数据包保存参数，该函数在start以前设置有效
     * @param pszFilePath--保存文件
     * @param bFilterMac--需要过滤MAC标志
     * @param zFilterMac--需要过滤的mac
     * @return true：成功，false：失败
     */
    virtual bool setSavePcap(const char* pszFilePath, bool bFilterMac, uint8_t zFilterMac[6]) = 0;
    
    /**
    * 设置抓包过滤器
    * @param pFilter--过滤表达式
    * @return true:成功，false：失败
    */
    virtual bool setPcapFilter(const char* pszFilter) = 0;
    
    /**
     * 注册帧数据回调函数，
     * @param pObject--调用对象，将作为pFrameCallback的第一个参数
     * @param pFrameCallback--回调函数
     * @return true：成功，false：失败
     */
    virtual bool registeFrameCallback(void* pObject, PFRAME_CALLBACK pFrameCallback) = 0;
        
    /**
     * 启动
     */
    virtual bool start() = 0;
    
    /**
     * 停止
     */
    virtual bool stop() = 0;
    
    /**
     * 发送数据包,多线程安全
     * @param unChannel--发送信道
     * @param pPacket--数据包
     * @param nSize--数据包长度
     * @return true：成功，false：失败
     */
    virtual bool sendPacket(int8_t nChannel, const unsigned char* pPacket, int nSize) = 0;
};


#endif /* WIFIBASE_H */

