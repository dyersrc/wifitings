/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   realwifitings.h
 * Author: dyer
 *
 * Created on 2017年2月7日, 下午4:30
 */

#ifndef REALWIFITINGS_H
#define REALWIFITINGS_H

#include <cinttypes>
#include "../../tools/llargekeymap.h"
//#include "../include/wifitings.h"
#include "wireless.h"
#include "liw.h"
#include "wifipcap.h"
#include "../include/filedef.h"
#include "ieee80211.h"

class CRealWifiTings : public CWifiTings {
public:
    CRealWifiTings();
    ~CRealWifiTings();
    
    /**
     * 设置系统根目录
     * @param pszRootDir--系统根目录
     * @return true：成功，false：失败
     */
    bool setRootDir(const char* pszRootDir);
    
    /**
     * 设置网卡ID
     * @param nId--网卡ID
     * @return true：成功，false：失败
     */
    bool setAdapterId(int nId);
    
    /**
     * 设置网卡名
     * @param pszAdapterName--网卡名
     * @return true:成功，false：失败
     */
    bool setAdapterName(const char* pszAdapterName);
    
    /**
     * 获取网卡名
     * @return NULL：失败返回
     */
    char* getAdapterName();
    
    /**
     * 获取WIFI设备身份信息
     * @param zMac--MAC地址
     * @param wirelessIdentity--接收身份信息
     * @return true:成功，false：失败
     */
    bool getWirelessIdentity(uint8_t zMac[6],  WIRELESS_IDENTITY& wirelessIdentity);
    
    /**
     * 设置频段
     * @param nFreq--2：2.4G，5：5G
     * @return true：成功，false：false
     */
    bool setFrequencyChanne(int nFreq);
    
    /**
     * 设置预设的频段
     * @return -1：未设置， 2：2.4G，5：5G
     */
    int getFrequencyChanne();
    
    /**
     * 得到网卡工作状态
     * @return -1：失败，0:正常，1：抓包模块已经停止，2：信道管理线程已经停止,3:模块已经停止
     */
    int getWorkState();
    
    /**
     * 设置网卡工作信道列表
     * @param zListChannel--信道列表,最大支持13个
     * @param nCounts--实际输入信道数
     * @return true：设置成功
     */
    bool setChannel(int8_t zListChannel[], int nCounts);
    
    /**
     * 得到网卡当前信道
     * @return 0：失败
     */
    int8_t getChannel();
    
    /**
     * 设置数据包保存参数，该函数在start以前设置有效
     * @param pszFilePath--保存文件
     * @param bFilterMac--需要过滤MAC标志
     * @param zFilterMac--需要过滤的mac
     * @return true：成功，false：失败
     */
    bool setSavePcap(const char* pszFilePath, bool bFilterMac, uint8_t zFilterMac[6]);
    
    /**
    * 设置抓包过滤器
    * @param pFilter--过滤表达式
    * @return true:成功，false：失败
    */
    bool setPcapFilter(const char* pszFilter);
    
    /**
     * 注册帧数据回调函数，
     * @param pObject--调用对象，将作为pFrameCallback的第一个参数
     * @param pFrameCallback--回调函数
     * @return true：成功，false：失败
     */
    bool registeFrameCallback(void* pObject, PFRAME_CALLBACK pFrameCallback);
        
    /**
     * 启动
     */
    bool start();
    
    /**
     * 停止
     */
    bool stop();
    
    /**
     * 发送数据包,多线程安全
     * @param unChannel--发送信道
     * @param pPacket--数据包
     * @param nSize--数据包长度
     * @return true：成功，false：失败
     */
    bool sendPacket(int8_t nChannel, const unsigned char* pPacket, int nSize);
        
protected:    
    /**
     * 通过iw得到当前网卡
     * @return -1：失败
     */
    int getCurrChannelFromIw();
    
    /**
    * 线程执行函数
    */
    void excute();
    
    //工作线程函数
    static void __worker_proc(void* pThis);
    
    //抓包回调函数
    static void pcapcallback(u_char* pObject, const struct pcap_pkthdr * pPkthdr, const u_char*pPacket);
    
    //添加ap
    PWIRELESS_AP addAp(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket);
    
    //添加station
    PWIRELESS_STATION addStation(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket);
    
    //添加unknow
    PWIRELESS_DEVICE addUnknow(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket);
    
    //处理帧数据
    void handleframe(const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket);
      
protected:
    char            m_szRootDir[512];           //系统根目录，以'/'结尾    
    int              m_nCountsOfRealPkt;       //抓取到有效包数
    char            m_szAdapterName[256];   //网卡名
    int              m_nAdapterId;               //网卡ID，有外面设置，-1：未设置
    
    CWifiPcap     m_WifiPcap;                   //抓包对象
    CLiw            m_Liw;                         //信道设置对象
    int               m_nFreq;                      //信道频段，2：2.4G，5：5G
    
    void*           m_pObject;                   //保存registeFrameCallback的第一个参数
    PFRAME_CALLBACK m_pFrameCallback;//保存registeFrameCallback的第二个参数
    
    uint8_t        m_zDumpFilterMac[6];      //保存setSavePcap的第三个参数
    bool            m_bFilterMac;                 //保存setSavePcap的第二个参数
    bool            m_bDumppcap;               //保存数据包标志
    
    int               m_nCountsOfWorkChannel;//工作信道数
    int8_t          m_listOfWorkChannel[200]; //工作信道列表    
    int               m_nIndexOfChannel;        //当前信道偏移
    
    int8_t          m_nCurChannel;              //当前信道
    
    boost::mutex  m_ChannelLock;             //信道锁
    bool              m_bStoped;                  //工作线程被外部停止标志
    boost::thread* m_pThreadWorker;       //工作线程
    
    CIeee80211      m_Ieee80211;              //ieee80211解析对象
    
public:
    /**
     * 超时检测函数
     */
    static void outtimeThreadProc();
    
    /**
     * 遍历AP表
     * @param pObject--当前对象
     * @param nKey--键
     * @param pValue--值
     */
    static void foreachApCallback(void* pObject, uint64_t nKey, void* pValue);
    
    /**
     * 遍历Station表
     * @param pObject--当前对象
     * @param nKey--键
     * @param pValue--值
     */
    static void foreachStationCallback(void* pObject, uint64_t nKey, void* pValue);
    
    /**
     * 遍历Unknow表
     * @param pObject--当前对象
     * @param nKey--键
     * @param pValue--值
     */
    static void foreachUnknowCallback(void* pObject, uint64_t nKey, void* pValue);
    
    /**
     * 遍历Free表
     * @param pObject--当前对象
     * @param nKey--键
     * @param pValue--值
     */
    static void foreachFreeCallback(void* pObject, uint64_t nKey, void* pValue);
        
    static void printMacFromKey(void* pObject, uint64_t nKey, void* pValue) {
        uint8_t zMac[6];
        memcpy(zMac, &nKey, 6);
        printf("nKey: %" PRIu64 ", zMac: %2X:%2X:%2X:%2X:%2X:%2X\n", 
                nKey, zMac[0], zMac[1], zMac[2], zMac[3], zMac[4], zMac[5]);
    }

public:
    static boost::thread*           m_pOutTimeThreadWorker;//工作线程
    static bool                         m_bOutTimeStoped;
    static void*                        m_pCallbackObject; //离线回调对象
    static PFRAME_CALLBACK    m_pOffLineCallback;//离线回调函数
    static CLLargeKeyMap          m_mapAp;             //AP列表
    static CLLargeKeyMap          m_mapStation;       //已经连接AP
    static CLLargeKeyMap          m_mapUnknow;      //未知身份终端
    static CLLargeKeyMap          m_mapFree;           //未知身份终端
    static uint64_t                     m_nOffLineTime;    //离线时间
    static uint64_t                     m_nFreeTime;        //回收时间
};

#endif /* REALWIFITINGS_H */

