/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   realwifitings.cpp
 * Author: dyer
 * 
 * Created on 2017年2月7日, 下午4:30
 */

#include <unistd.h>
#include "../../include/DbgPrint.h"
#include "../../include/leelog.h"
#include "../../tools/time_tools.h"
#include "../../tools/cmd.h"
#include "../../tools/stringprt.h"
#include "realwifitings.h"
#include "wireless.h"

boost::thread*           CRealWifiTings::m_pOutTimeThreadWorker = NULL;//工作线程
bool                         CRealWifiTings::m_bOutTimeStoped = true;
void*                        CRealWifiTings::m_pCallbackObject = NULL; //离线回调对象
PFRAME_CALLBACK    CRealWifiTings::m_pOffLineCallback = NULL;//离线回调函数
CLLargeKeyMap          CRealWifiTings::m_mapAp;            //AP列表
CLLargeKeyMap          CRealWifiTings::m_mapStation;       //已经连接AP
CLLargeKeyMap          CRealWifiTings::m_mapUnknow;      //未知身份终端
CLLargeKeyMap          CRealWifiTings::m_mapFree;           //未知身份终端
uint64_t                     CRealWifiTings::m_nOffLineTime = WIRELEE_OUTTIME;
uint64_t                     CRealWifiTings::m_nFreeTime = WIRELEE_OUTTIME_FREE;

CRealWifiTings::CRealWifiTings() {
    memset(m_szRootDir, 0x00, sizeof(m_szRootDir));
    memset(m_szAdapterName, 0x00, sizeof(m_szAdapterName));    
    m_nCountsOfWorkChannel = 0;//工作信道数
    memset(m_listOfWorkChannel, 0x00, sizeof(m_listOfWorkChannel));
    m_nIndexOfChannel = 0;
    m_nCurChannel = 0;
    memset(m_zDumpFilterMac, 0x00, sizeof(m_zDumpFilterMac));
    m_bFilterMac = false;
    m_bDumppcap = false;
    m_nCountsOfRealPkt = 0;
    m_pObject = NULL;
    m_pFrameCallback = NULL;
    m_bStoped = true;
    m_pThreadWorker = NULL;
    m_nFreq = -1;
    m_nAdapterId = -1;
}

CRealWifiTings::~CRealWifiTings() {
    stop();
}

/**
* 设置系统根目录
* @param pszRootDir--系统根目录
* @return true：成功，false：失败
*/
bool CRealWifiTings::setRootDir(const char* pszRootDir) {
    if (NULL == pszRootDir) {
            return false;
    }
    strcpy(m_szRootDir, pszRootDir);
    if ('/' != m_szRootDir[strlen(m_szRootDir) - 1])
    {
        strcat(m_szRootDir, "/");
    }

    return true;
}

/**
* 设置网卡ID
* @param nId--网卡ID
* @return true：成功，false：失败
*/
bool CRealWifiTings::setAdapterId(int nId) {
    m_nAdapterId = nId;
    return false;
}

/**
* 设置网卡名
* @param pszAdapterName--网卡名
* @return true:成功，false：失败
*/
bool CRealWifiTings::setAdapterName(const char* pszAdapterName) {
    
    if (NULL == pszAdapterName || strlen(pszAdapterName) <= 0) {
            return false;
    }
    strcpy(m_szAdapterName, pszAdapterName);

    return true;
}

/**
* 获取网卡名
* @return NULL：失败返回
*/
char* CRealWifiTings::getAdapterName() {    
    return m_szAdapterName;
}

/**
* 获取WIFI设备身份信息
* @param zMac--MAC地址
* @param wirelessIdentity--接收身份信息
* @return true:成功，false：失败
*/
bool CRealWifiTings::getWirelessIdentity(uint8_t zMac[6],  WIRELESS_IDENTITY& wirelessIdentity) {

    uint64_t nKey = 0;
    memcpy(&nKey, zMac, 6);
    PWIRELESS_AP pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey);
    if (NULL != pAp) {
        wirelessIdentity.nAdapterId = -1;
        wirelessIdentity.unIdentity = pAp->wireless.unIdentity;
        wirelessIdentity.nChannel = pAp->nChannel;    
        if (-1 == wirelessIdentity.nChannel && pAp->wireless.nLastChannel > 0) {
            wirelessIdentity.nChannel = pAp->wireless.nLastChannel;
        }
        wirelessIdentity.nSignal = pAp->wireless.nLastSignal;
        wirelessIdentity.unOnline = pAp->wireless.unOnline;
        memcpy(wirelessIdentity.zMac, pAp->wireless.zMac, 6);
        wirelessIdentity.ap.nAuth = pAp->nAuth;
        wirelessIdentity.ap.nEncrypt = pAp->nEncrypt;
        wirelessIdentity.ap.nPrivacy = pAp->nPrivacy;
        wirelessIdentity.ap.nWPS = pAp->nWPS;
        wirelessIdentity.ap.nHideSSID = pAp->nHideSSID;
        snprintf(wirelessIdentity.ap.szSSID, 64, "%s", pAp->szSSID);
        wirelessIdentity.unChanged = 0;
        
        return true;
    }
    PWIRELESS_STATION pStation = (PWIRELESS_STATION)m_mapStation.findObject(nKey);
    //m_mapStation.foreach(this, printMacFromKey);
    if (NULL != pStation) {
        wirelessIdentity.nAdapterId = -1;
        wirelessIdentity.unIdentity = pStation->wireless.unIdentity;        
        wirelessIdentity.nChannel = pStation->nChannel;    
        if (-1 == wirelessIdentity.nChannel && pStation->wireless.nLastChannel > 0) {
            wirelessIdentity.nChannel = pStation->wireless.nLastChannel;
        }
        wirelessIdentity.nSignal = pStation->wireless.nLastSignal;
        wirelessIdentity.unOnline = pStation->wireless.unOnline;
        memcpy(wirelessIdentity.zMac, pStation->wireless.zMac, 6);
        wirelessIdentity.station.nConnected = pStation->nConnected;
        memcpy(wirelessIdentity.station.zMacOfAP, pStation->zMacOfAP, 6);
        wirelessIdentity.unChanged = 0;
        
        return true;
    }
    
    return false;
}

/**
* 设置频段
* @param nFreq--2：2.4G，5：5G
* @return true：成功，false：false
*/
bool CRealWifiTings::setFrequencyChanne(int nFreq) {
    m_nFreq = nFreq;
    return true;
}

/**
* 设置预设的频段
* @return -1：未设置， 2：2.4G，5：5G
*/
int CRealWifiTings::getFrequencyChanne() {
    return m_nFreq;
}

/**
* 得到网卡工作状态
* @return -1：失败，0:正常，1：抓包模块已经停止，2：信道管理线程已经停止,3:模块已经停止
*/
int CRealWifiTings::getWorkState() {
    
    int nRet = 0;
    if (m_bStoped) {
        nRet |= 2;
    }
    if (!m_WifiPcap.getRunningState()) {
        nRet |= 1;
    }
    
    return nRet;
}

/**
* 设置网卡工作信道列表
* @param zListChannel--信道列表,最大支持13个
* @param nCounts--实际输入信道数
* @return true：设置成功
*/
bool CRealWifiTings::setChannel(int8_t zListChannel[], int nCounts) {
    
    if (nCounts <= 0) {
        m_nCountsOfWorkChannel = 0;
        m_nCurChannel = -1;
        return true;
    }
    bool bRet = false;
    char szMsg[1024] = { 0 };
    char szTemp[256] = { 0};
    m_nCountsOfWorkChannel = 0;
    for (int i = 0; i < nCounts && m_nCountsOfWorkChannel < 200; i++) {
        if (zListChannel[i] > 0 && zListChannel[i] <= 200) {
             m_listOfWorkChannel[m_nCountsOfWorkChannel] = zListChannel[i];
             snprintf(szTemp, sizeof(szTemp),  "(%d)", zListChannel[i]);
             strcat(szMsg, szTemp);
             m_nCountsOfWorkChannel++;
             bRet = true;
        }       
    }
    printf("setChannel: %s{ counts: %d, %s }\r\n", m_szAdapterName, m_nCountsOfWorkChannel, szMsg);
    LOGINFO("setChannel", "%s{ counts: %d, %s }", m_szAdapterName, m_nCountsOfWorkChannel, szMsg);
    
    return bRet;
}

/**
* 得到网卡当前信道
* @return 0：失败
*/
int8_t CRealWifiTings::getChannel() {
    return m_nCurChannel;
}

/**
* 设置数据包保存参数，该函数在start以前设置有效
* @param pszFilePath--保存文件
* @param bFilterMac--需要过滤MAC标志
* @param zFilterMac--需要过滤的mac
* @return true：成功，false：失败
*/
bool CRealWifiTings::setSavePcap(const char* pszFilePath, bool bFilterMac, uint8_t zFilterMac[6]) {
    
    if (NULL == pszFilePath) {
        return false;
    }
    if (!m_WifiPcap.setDump(pszFilePath)) {
        return false;
    }
    m_bDumppcap = true;
    if (bFilterMac) {
        m_bFilterMac = bFilterMac;
        memcpy(m_zDumpFilterMac, zFilterMac, 6);
    }
    
    return true;
}

/**
* 设置抓包过滤器
* @param pFilter--过滤表达式
* @return true:成功，false：失败
*/
bool CRealWifiTings::setPcapFilter(const char* pszFilter) {
    if (NULL == pszFilter || strlen(pszFilter) <= 0) {
        return false;
    }
    
    return m_WifiPcap.setPcapFilter(pszFilter);
}

/**
* 注册帧数据回调函数，
* @param pObject--调用对象，将作为pFrameCallback的第一个参数
* @param pFrameCallback--回调函数
* @return true：成功，false：失败
*/
bool CRealWifiTings::registeFrameCallback(void* pObject, PFRAME_CALLBACK pFrameCallback) {
    m_pObject = pObject;
    m_pFrameCallback = pFrameCallback;
    
    return true;
}

/**
* 启动
*/
bool CRealWifiTings::start() {
    
    char szLogFile[512] = { 0 };    
    snprintf(szLogFile, sizeof(szLogFile), "%s%s/%s", m_szRootDir, FOLDER_LOG, FILE_RUN_LOG);
    INITASYNLOG(szLogFile, LEVEL_DEBUG);
    
    if (!m_Liw.init()) {
        LOGERROR("start", "[%s]:信道修改对象初始化失败", m_szAdapterName);
        return false;
    }
    if (!m_WifiPcap.setAdapterName(m_szAdapterName) ||
            !m_WifiPcap.setCallbackProc((unsigned char*)this, pcapcallback)) {
        return false;
    }
    //启动抓包模块
    if (!m_WifiPcap.start()) {
        LOGERROR("start", "[%s]:启动抓包模块失败", m_szAdapterName);
        return false;
    } 
    
    try {
        if (NULL == m_pThreadWorker || m_bStoped) {
            m_pThreadWorker = new boost::thread(__worker_proc, this);
            if (NULL == m_pThreadWorker) {
                LOGERROR("start", "[%s]:启动信道管理线程失败", m_szAdapterName);
                return false;
            }
        }
    } catch(boost::exception& e) {
        LOGABORT("start", "[%s]:启动信道管理线程异常", m_szAdapterName);
        return false;
    }
    LOGSUCCESS("start", "[%s]:启动成功", m_szAdapterName);
    
    return true;
}

/**
* 停止
*/
bool CRealWifiTings::stop() {
    
    m_bStoped = true;
    //退出线程
    try {
        if (NULL != m_pThreadWorker) {
            m_pThreadWorker->join();
            delete m_pThreadWorker;
            m_pThreadWorker = NULL;
        }        
    } catch (boost::exception& e) {
        return false;
    }
    
    if (!m_WifiPcap.stop()) {
        return false;
    }
    LOGSUCCESS("stop", "[%s]:停止成功", m_szAdapterName);
    
    return true;
}

/**
* 发送数据包,多线程安全
* @param nChannel--发送信道
* @param pPacket--数据包
* @param nSize--数据包长度
* @return true：成功，false：失败
*/
bool CRealWifiTings::sendPacket(int8_t nChannel, const unsigned char* pPacket, int nSize) {
    
    bool bRet = false;
    
    if (NULL == pPacket || nSize <= 0 || nChannel <= 0 || nChannel > 13) {
        return false;
    }
    try {
        boost::lock_guard<boost::mutex> lock(m_ChannelLock);
        int nTempChannel = 0;
        if (nChannel != m_nCurChannel) {
            nTempChannel = (int)nChannel;
            if (m_Liw.setChannel(m_szAdapterName, nTempChannel)) {
                m_nCurChannel = nChannel;
            }
        }
        if (0 == m_WifiPcap.sendpacket(pPacket, nSize)) {
            bRet = true;
        }
        if (nChannel != m_nCurChannel) {
            nTempChannel = (int)m_nCurChannel;
            m_Liw.setChannel(m_szAdapterName, nTempChannel);
        }
    } catch (...) {
        return false;
    }
    
    return bRet;
}

/**
* 通过iw得到当前网卡
* @return -1：失败
*/
int CRealWifiTings::getCurrChannelFromIw() {
    char szCmd[512] = { 0 };
    snprintf(szCmd, sizeof(szCmd), "iw dev %s info | awk '{if($1==\"channel\") print $2}'", m_szAdapterName);
    
    CCmd lpipe;
    if (!lpipe.open(szCmd, "r")) {
        return -1;
    }
    char szMessage[64] = { 0 };
    int nCountsOfWait = 0;
    while (!m_bStoped) {
        if (lpipe.out(szMessage, sizeof(szMessage), 3000)) {
            char szTmp[64] = { 0 };
            trimspace(szMessage, szTmp, 64);
            for (int i = 0; i < strlen(szTmp); i++) {                
                if (0 == szTmp[i] || '\n' == szTmp[i] || '\r' == szTmp[i]) {
                    break;
                }
                if (szTmp[i] < '0' || szTmp[i] > '9') {
                    return -1;
                }
            }
            lpipe.close();
            return atoi(szTmp);
        }        
        nCountsOfWait++;
        if (nCountsOfWait > 5) {//大约等到15s
            break;
        }
        usleep(500000);
    }    
    lpipe.close();
    
    return -1;
}

/**
* 线程执行函数
*/
void CRealWifiTings::excute() {
    
    int nChannel = 0;
    m_nIndexOfChannel = 0;
    while (!m_bStoped) {
        if (m_nCountsOfWorkChannel > 0) {
            if (m_nIndexOfChannel >= m_nCountsOfWorkChannel) {
                //已经工作到最后一个信道，回归到第一个信道重新执行
                m_nIndexOfChannel = 0;
            }        
            try {            
                boost::lock_guard<boost::mutex> lock(m_ChannelLock);
                nChannel = (int)m_listOfWorkChannel[m_nIndexOfChannel]; 
                if (m_nCurChannel != m_listOfWorkChannel[m_nIndexOfChannel]) {                               
                    if (m_Liw.setChannel(m_szAdapterName, nChannel)) {
                        m_nCurChannel = m_listOfWorkChannel[m_nIndexOfChannel];
                    }
                } else {
                    int nChannelFromIw = getCurrChannelFromIw();
                    if (nChannel != nChannelFromIw) {
                        m_Liw.setChannel(m_szAdapterName, nChannel);
                    }
                }
                m_nIndexOfChannel++;
            } catch (...) {

            }
        }
        //睡眠60毫秒
        usleep(60*1000);
    }    
    LOGWARNING("excute", "[%s]:信道管理线程已退出", m_szAdapterName);
}

//工作线程函数
void CRealWifiTings::__worker_proc(void* pThis) {
    CRealWifiTings* lpThis = (CRealWifiTings*)pThis;
    if (NULL == lpThis) {
        return;
    }
    lpThis->m_bStoped = false;
    lpThis->excute();
    lpThis->m_bStoped = true;
}

/**
 * 抓包回调函数
 * @param pObject--当前对象
 * @param pPkthdr--包头
 * @param pPacket--包数据
 */
void CRealWifiTings::pcapcallback(u_char* pObject, const struct pcap_pkthdr * pPkthdr, const u_char*pPacket) {
    
    CRealWifiTings* pThis = (CRealWifiTings*)pObject;
    if (NULL == pThis || NULL == pPkthdr || NULL == pPacket) {
        return;
    }   
    pThis->handleframe(pPkthdr, pPacket);
}

/**
 * 添加ap
 * @param unIdentity--身份
 * @param zMacOfTransmitter--发送mac
 * @param zMacOfBSSID--bssid的mac
 * @param pRadioapData--radiotap解析结果
 * @param pIeee80211Framee--ieee80211帧解析结果
 * @param pIeee80211FrameParseParam--ieee80211帧参数解析结果
 * @param pIeee80211FrameParam--ieee80211参数缓存
 * @param wirelessIdentity--接收身份信息
 * @param pPkthdr--
 * @param pPacket--
 * @return NULL：失败
 */
PWIRELESS_AP CRealWifiTings::addAp(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket) {
    
    if (NULL == pRadiotapData || NULL == pIeee80211Frame) {
        return NULL;
    }
    bool bInsert = false;
    int64_t nKey = 0;
    memcpy(&nKey, zMacOfTransmitter, 6);
    uint64_t nNow = getNowMillisecond();
    PWIRELESS_AP pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey);
    if (NULL == pAp) {//不存在
        pAp = (PWIRELESS_AP)malloc(sizeof(WIRELESS_AP));
        if (NULL == pAp) {
            return NULL;
        }
        memset(pAp, 0x00, sizeof(WIRELESS_AP));        
        pAp->wireless.unIdentity = 0;
        pAp->unLasttimeOfBridge = 0;
        pAp->unBridge = 0;
        pAp->wireless.nLastSignal = -127;
        pAp->wireless.nLastChannel = -1;
        pAp->nConfirm = 0;
        pAp->nChannel = -1;
        pAp->nAuth = -1;
        pAp->nEncrypt = -1;
        pAp->nHideSSID = -1;
        pAp->nPrivacy = -1;
        pAp->nWPS = -1;
        pAp->wireless.nFree = 0;
        pAp->wireless.unOnline = 0;
        PWIRELESS_STATION pStation = (PWIRELESS_STATION)m_mapStation.findObject(nKey);
        if (NULL != pStation) {//标记释放     
            if (IDENTITY_BRIDGE == unIdentity) {
                pAp->unLasttimeOfBridge = nNow;
                pAp->wireless.unIdentity = IDENTITY_BRIDGE;//标记桥                
            }
            if (NULL != m_mapStation.removeObject(nKey)) {
                pStation->wireless.nFree = 1;
                pStation->wireless.unLasttime = getNowMillisecond();
                if (!m_mapFree.insertObject(nKey, pStation)) {
                    usleep(10*1000);
                    free(pStation);
                }
            }            
        }         
        memcpy(pAp->wireless.zMac, zMacOfTransmitter, 6);
        bInsert = true;
        wirelessIdentity.unChanged = 1;
    } 
    if (1 == pAp->wireless.nFree) {
        return NULL;
    }
    //更新
    pAp->wireless.nFree = 0;
    if (0 != nNow && IDENTITY_BRIDGE == pAp->wireless.unIdentity && 0 != pAp->unLasttimeOfBridge&&
            nNow >= pAp->unLasttimeOfBridge + WIRELEE_BRIDGE_OUTTIME && IDENTITY_BRIDGE != unIdentity) {//
        pAp->unBridge = IDENTITY_AP;
    }
    if (IDENTITY_BRIDGE == unIdentity) {//桥数据
        pAp->unLasttimeOfBridge = nNow;
        pAp->unBridge = 1;        
        if (0 == pAp->wireless.unIdentity) {
            pAp->wireless.unIdentity = IDENTITY_AP;            
        }
    } else if (pAp->wireless.unIdentity != unIdentity && 
            IDENTITY_BRIDGE != pAp->wireless.unIdentity) {//身份发生变化,且之前没有确认身份是桥
        pAp->wireless.unIdentity = unIdentity;
        wirelessIdentity.unChanged = 1;
    }
    if (FRAME_TYPES_DISASSOCIATION != pIeee80211Frame->nType &&
            FRAME_TYPES_DEAUTHENTICATION != pIeee80211Frame->nType) {
        pAp->wireless.unLasttime = nNow;
        if (1 != pAp->wireless.unOnline) {//重新上线
            pAp->wireless.unOnline = 1;
            wirelessIdentity.unChanged = 1;
        }
    }      
    pAp->wireless.nLastSignal = pRadiotapData->nSignal;
    pAp->wireless.nLastChannel = pRadiotapData->nChannel;
    if (NULL != pIeee80211FrameParseParam) {//存在解析参数
        if (pIeee80211FrameParseParam->channel.bExisted) {
            pAp->nChannel = pIeee80211FrameParseParam->channel.nChannel;
        }         
        if (pIeee80211FrameParseParam->auth.bExisted) {//更新认证方式
            if (-1 == pAp->nAuth ||
                FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
                FRAME_TYPES_BEACON == pIeee80211Frame->nType //beacon包
                    ) {
                if (pAp->nAuth != pIeee80211FrameParseParam->auth.nAuth) {
                    pAp->nAuth = pIeee80211FrameParseParam->auth.nAuth;
                    wirelessIdentity.unChanged = 1;
                }
            }
        }
        if (pIeee80211FrameParseParam->encrypt.bExisted) {//更新加密算法
            if (-1 == pAp->nEncrypt  || 
                FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
                FRAME_TYPES_BEACON == pIeee80211Frame->nType //beacon包
                    ) {
                if (pAp->nEncrypt != pIeee80211FrameParseParam->encrypt.nEncrypt) {
                    pAp->nEncrypt = pIeee80211FrameParseParam->encrypt.nEncrypt;
                    wirelessIdentity.unChanged = 1;
                }
            }
        }
        if (pIeee80211FrameParseParam->privacy.bExisted) {//更新保护方式
            if (-1 == pAp->nPrivacy  || 
                FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
                FRAME_TYPES_BEACON == pIeee80211Frame->nType //beacon包
                    ) {
                if (pAp->nPrivacy != pIeee80211FrameParseParam->privacy.nPrivacy) {
                    pAp->nPrivacy = pIeee80211FrameParseParam->privacy.nPrivacy;
                    wirelessIdentity.unChanged = 1;
                }
            }
        }
        if (pIeee80211FrameParseParam->ssid.bExisted) {//更新ssid
            if (0 != strcmp(pAp->szSSID, pIeee80211FrameParseParam->ssid.szSSID) &&
                    strlen(pIeee80211FrameParseParam->ssid.szSSID) > 0) {
                wirelessIdentity.unChanged = 1;
                snprintf(pAp->szSSID, 66, "%s", pIeee80211FrameParseParam->ssid.szSSID);
            }                
            if (0 == pIeee80211FrameParseParam->ssid.nHide || 1 == pIeee80211FrameParseParam->ssid.nHide) {                    
                if (pAp->nHideSSID != pIeee80211FrameParseParam->ssid.nHide) {
                    pAp->nHideSSID = pIeee80211FrameParseParam->ssid.nHide;
                    wirelessIdentity.unChanged = 1;
                }
            }
        }
        if (pIeee80211FrameParseParam->wps.bExisted) {//更新ssid
            if (-1 == pAp->nWPS ||
                FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
                FRAME_TYPES_BEACON == pIeee80211Frame->nType //beacon包
                    ) {
                if (pAp->nWPS != pIeee80211FrameParseParam->wps.nWPS) {
                    pAp->nWPS = pIeee80211FrameParseParam->wps.nWPS;   
                    wirelessIdentity.unChanged = 1;
                }
            }
        }     
        if (FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
                FRAME_TYPES_BEACON == pIeee80211Frame->nType //beacon包
                ) {                 
            pAp->nConfirm = 1;
        }  
    }
//    if (nKey == 0xAA4EAB907514 && 
//        (FRAME_TYPES_PROBE_RESPONSE == pIeee80211Frame->nType || //探查应答包
//            FRAME_TYPES_BEACON == pIeee80211Frame->nType) //beacon包
//            ) {
//        int nnn = 0;
//    }
    //填写身份信息
    wirelessIdentity.unIdentity = pAp->wireless.unIdentity;
    wirelessIdentity.nChannel = pAp->nChannel;    
    wirelessIdentity.nSignal = -127;
    if (pAp->nChannel > 0 && pAp->nChannel == pAp->wireless.nLastChannel) {
        wirelessIdentity.nSignal = pAp->wireless.nLastSignal;
    }
    wirelessIdentity.unOnline = 1;
    memcpy(wirelessIdentity.zMac, pAp->wireless.zMac, 6);
    wirelessIdentity.ap.nAuth = pAp->nAuth;
    wirelessIdentity.ap.nEncrypt = pAp->nEncrypt;
    wirelessIdentity.ap.nPrivacy = pAp->nPrivacy;
    wirelessIdentity.ap.nWPS = pAp->nWPS;
    wirelessIdentity.ap.nHideSSID = pAp->nHideSSID;
    snprintf(wirelessIdentity.ap.szSSID, 66, "%s", pAp->szSSID);
    
    if (bInsert) {//插入到AP表
        if (!m_mapAp.insertObject(nKey, pAp)) {
            free(pAp);
            return NULL;
        }
    }
    if (2 == pIeee80211Frame->nDS && 
            pIeee80211Frame->nType >= FRAME_TYPES_DATA_DATA && 
            pIeee80211Frame->nType <= FRAME_TYPES_DATA_QOS_CF_ACK_POLL) {//是AP发出的数据在
        if ((pIeee80211Frame->zAddress1[0] % 2) != 1) {//不是广播或组播地址
            uint64_t nKey2 = 0;
            memcpy(&nKey2, pIeee80211Frame->zAddress1, 6);  
            PWIRELESS_STATION pStation = (PWIRELESS_STATION)m_mapStation.findObject(nKey2);
            if (NULL != pStation) {//标记终端连接状态
                if (0 != memcmp(pStation->zMacOfAP, zMacOfTransmitter, 6)) {
                    pStation->nConnected = 1;
                    pStation->nSet = 1;
                    pStation->nChannel = pAp->nChannel;
                    memcpy(pStation->zMacOfAP, zMacOfTransmitter, 6);
//                    if (0 == memcmp(m_zDumpFilterMac, pIeee80211Frame->zAddress1, 6)) {
//                        pStation->nConnected = 1;
//                        memcpy(pStation->zMacOfAP, zMacOfTransmitter, 6);
//                    }
                }
            }            
        }        
    }
    
    return pAp;
}

/**
 * 添加station
 * @param unIdentity--身份
 * @param zMacOfTransmitter--发送mac
 * @param zMacOfBSSID--bssid的mac
 * @param pRadiotapData--radiotap解析结果
 * @param pIeee80211Framee--ieee80211帧解析结果
 * @param pIeee80211FrameParseParam--ieee80211帧参数解析结果
 * @param pIeee80211FrameParam--ieee80211参数缓存
 * @param wirelessIdentity--接收身份信息
 * @param pPkthdr--
 * @param pPacket--
 * @return NULL：失败
 */
PWIRELESS_STATION CRealWifiTings::addStation(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket) {
    if (NULL == pRadiotapData || NULL == pIeee80211Frame) {
        return NULL;
    }
    bool bInsert = false;
    int64_t nKey = 0;
    int64_t nKey2 = 0;  
    memcpy(&nKey, zMacOfTransmitter, 6);    
    PWIRELESS_AP pAp = NULL;
    uint64_t nNow = getNowMillisecond();
    pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey);
    if (NULL != pAp && 1 == pAp->unBridge) { //已经确认过桥身份
        if (0 == nNow || 0 == pAp->unLasttimeOfBridge) {
            return NULL;
        }
        if (nNow >= pAp->unLasttimeOfBridge + WIRELEE_BRIDGE_OUTTIME) {
            goto update_station;
        }
        pAp->unLasttimeOfBridge = nNow;
        pAp->wireless.unIdentity = IDENTITY_BRIDGE;        
        pAp = addAp(IDENTITY_AP, zMacOfTransmitter, zMacOfBSSID,
                pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                wirelessIdentity, pPkthdr, pPacket);
        if (NULL != pAp) {    
            if (NULL != m_pFrameCallback) {
                m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                        pIeee80211FrameParam, pPacket, pPkthdr->len);
            }
        }
        return NULL;
    }    

update_station:
    
//    if (0 != memcmp(m_zDumpFilterMac, zMacOfTransmitter, 6)) {
//        return NULL;
//    }    
    PWIRELESS_STATION pStation = (PWIRELESS_STATION)m_mapStation.findObject(nKey);
    if (NULL == pStation) {//不存在
        pStation = (PWIRELESS_STATION)malloc(sizeof(WIRELESS_STATION));
        if (NULL == pStation) {
            return NULL;
        }
        memset(pStation, 0x00, sizeof(WIRELESS_STATION));
        pStation->wireless.unIdentity = unIdentity;
        pStation->wireless.nLastSignal = -127;
        pStation->wireless.nLastChannel = -1;
        pStation->nConnected = 0;
        pStation->nSet = 0;
        pStation->nChannel = -1;
        pStation->wireless.nFree = 0;
        pStation->wireless.unOnline = 0;
//        pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey);
        if (NULL != pAp) {//如果没有确认过桥，标记释放                 
            if (NULL != m_mapAp.removeObject(nKey)) {
                pAp->wireless.unLasttime = getNowMillisecond();
                pAp->wireless.nFree = 1;
                if (!m_mapFree.insertObject(nKey, pAp)) {
                    usleep(10*1000);
                    free(pAp);
                }
            } 
        }
        memcpy(pStation->wireless.zMac, zMacOfTransmitter, 6);
        bInsert = true;
        wirelessIdentity.unChanged = 1;
    }
    if (1 == pStation->wireless.nFree) {
        return NULL;
    }
    if (pStation->wireless.unIdentity != unIdentity) {//身份发生了变化
        pStation->wireless.unIdentity == unIdentity;
        wirelessIdentity.unChanged = 1;
    } else if (1 == pStation->nSet) {
        pStation->nSet = 0;
        wirelessIdentity.unChanged = 1;
    }
    if (FRAME_TYPES_DISASSOCIATION != pIeee80211Frame->nType &&
            FRAME_TYPES_DEAUTHENTICATION != pIeee80211Frame->nType) {
        pStation->wireless.unLasttime = nNow;
        if (1 != pStation->wireless.unOnline) {//重新上线
            pStation->wireless.unOnline = 1;
            pStation->nChannel = -1;
            wirelessIdentity.unChanged = 1;
        }
    }         
    pStation->wireless.nLastSignal = pRadiotapData->nSignal;
    pStation->wireless.nLastChannel = pRadiotapData->nChannel;
    
    if (pIeee80211Frame->nType >= FRAME_TYPES_DATA_DATA && 
            pIeee80211Frame->nType <= FRAME_TYPES_DATA_QOS_CF_ACK_POLL) {//已经连接AP
        nKey2 = 0;        
        if ((zMacOfBSSID[0] % 2) != 1) {//不是广播或组播地址
            memcpy(&nKey2, zMacOfBSSID, 6);         
            pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey2);
            if (NULL != pAp) {//找到连接的AP
                if (1 != pStation->nConnected) {
                    pStation->nConnected = 1;
                    wirelessIdentity.unChanged = 1;
                }      
                if (0 != memcmp(pStation->zMacOfAP, zMacOfBSSID, 6)) {
                    memcpy(pStation->zMacOfAP, zMacOfBSSID, 6);   
                    wirelessIdentity.unChanged = 1;
                }
                if (1 == pAp->nConfirm) {
                    if (pStation->nChannel != pAp->nChannel) {
                        pStation->nChannel = pAp->nChannel;
                        wirelessIdentity.unChanged = 1;
                    }
                } else {
                    if (NULL != pIeee80211FrameParseParam) {
                        if (pIeee80211FrameParseParam->channel.bExisted) {
                            pStation->nChannel = pIeee80211FrameParseParam->channel.nChannel;                            
                        }
                        if (pIeee80211FrameParseParam->encrypt.bExisted) {
                            pAp->nEncrypt = pIeee80211FrameParseParam->encrypt.nEncrypt;
                        }
                        if (pIeee80211FrameParseParam->privacy.bExisted) {
                            pAp->nPrivacy = pIeee80211FrameParseParam->privacy.nPrivacy;
                        }
                    }
                }
            }
        }
    } else {
        if (NULL != pIeee80211FrameParseParam) {
            if ((1 != pStation->nConnected || pStation->nChannel <=0) && pIeee80211FrameParseParam->channel.bExisted) {
                pStation->nChannel = pIeee80211FrameParseParam->channel.nChannel;
            }
            if (FRAME_TYPES_ASSOCIATION_REQUEST == pIeee80211Frame->nType &&
                    pIeee80211FrameParseParam->ssid.bExisted) {
                nKey2 = 0;        
                if ((zMacOfBSSID[0] % 2) != 1) {//不是广播或组播地址
                    memcpy(&nKey2, zMacOfBSSID, 6);         
                    pAp = (PWIRELESS_AP)m_mapAp.findObject(nKey2);
                    if (NULL != pAp) {//找到连接的AP
                        if (strlen(pAp->szSSID) <= 0) {
                            snprintf(pAp->szSSID, 66, "%s", pIeee80211FrameParseParam->ssid.szSSID);
                        }
                    }
                }
            }                
        }
    } 
    
    //填写身份信息
    wirelessIdentity.unIdentity = pStation->wireless.unIdentity;
    wirelessIdentity.nChannel = pStation->nChannel;    
    wirelessIdentity.nSignal = -127;
    if (1 == pStation->nConnected) {//已经连接AP,并且已经确认信道        
        if (pStation->nChannel == pStation->wireless.nLastChannel)  {
            wirelessIdentity.nSignal = pStation->wireless.nLastSignal;
        }                
    } else {
        wirelessIdentity.nSignal = pStation->wireless.nLastSignal;
    }

    wirelessIdentity.unOnline = 1;
    memcpy(wirelessIdentity.zMac, pStation->wireless.zMac, 6);
    wirelessIdentity.station.nConnected = pStation->nConnected;
    memcpy(wirelessIdentity.station.zMacOfAP, pStation->zMacOfAP, 6);
    
    if (bInsert) {
        if (!m_mapStation.insertObject(nKey, pStation)) {
            free(pStation);
            return NULL;
        }
    }
    
    return pStation;
}

/**
 * 添加unknow
 * @param unIdentity--身份
 * @param zMacOfTransmitter--发送mac
 * @param zMacOfBSSID--bssid的mac
 * @param pRadioapData--radiotap解析结果
 * @param pIeee80211Framee--ieee80211帧解析结果
 * @param pIeee80211FrameParseParam--ieee80211帧参数解析结果
 * @param pIeee80211FrameParam--ieee80211参数缓存
 * @param wirelessIdentity--接收身份信息
 * @param pPkthdr--
 * @param pPacket--
 * @return NULL：失败
 */
PWIRELESS_DEVICE CRealWifiTings::addUnknow(uint8_t unIdentity, uint8_t zMacOfTransmitter[6], uint8_t zMacOfBSSID[6],
            PRADIOAP_DATA pRadiotapData, PIEEE80211_FRAME pIeee80211Frame,
            PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam, PIEEE80211_FRAME_PARAM pIeee80211FrameParam,
            WIRELESS_IDENTITY& wirelessIdentity, const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket) {
    
    if (NULL == pRadiotapData || NULL == pIeee80211Frame) {
        return NULL;
    }
    
    bool bInsert = false;
    int64_t nKey = 0;
    memcpy(&nKey, zMacOfTransmitter, 6);
    PWIRELESS_DEVICE pUnknow =  (PWIRELESS_DEVICE)m_mapUnknow.findObject(nKey);
    if (NULL == pUnknow) {
        pUnknow = (PWIRELESS_DEVICE)malloc(sizeof(WIRELESS_DEVICE));
        if (NULL == pUnknow) {
            return NULL;
        }
        memset(pUnknow, 0x00, sizeof(WIRELESS_DEVICE));
        pUnknow->nLastSignal = -127;
        pUnknow->nLastChannel = -1;
        pUnknow->nFree = 0;
        bInsert = true;
    }
    if (1 == pUnknow->nFree) {
        return NULL;
    }
    pUnknow->unIdentity = unIdentity;
    pUnknow->unLasttime = getNowMillisecond();
    pUnknow->unOnline = 1;
    pUnknow->nLastSignal = pRadiotapData->nSignal;
    pUnknow->nLastChannel = pRadiotapData->nChannel;
    
    //填写身份信息
    wirelessIdentity.unChanged = 0;
    wirelessIdentity.unIdentity = pUnknow->unIdentity;
    wirelessIdentity.nChannel = -1;    
    wirelessIdentity.unOnline = 1;
    memcpy(wirelessIdentity.zMac, pUnknow->zMac, 6);
    
    if (bInsert) {
        if (!m_mapUnknow.insertObject(nKey, pUnknow)) {
            free(pUnknow);
            return NULL;
        }
    }
    
    return pUnknow;
}

/**
 * 处理帧数据
 * @param pPkthdr--
 * @param pPacket--
 */
void CRealWifiTings::handleframe(const struct pcap_pkthdr * pPkthdr, const unsigned char*pPacket) {
    
    if (m_nCountsOfWorkChannel <= 0) {//没有设置抓包工作信道，不做任何处理
        return;
    }    
    if (!m_Ieee80211.parse(pPkthdr, pPacket)) {
        DebugPrint01("handleframe: Parsing frame of ieee80211 failed\r\n");
        return;
    }    
    
    //得到解析信息
    PRADIOAP_DATA pRadiotapData = m_Ieee80211.getRadiotap();       //radiotap解析结果
    PIEEE80211_FRAME pIeee80211Frame = m_Ieee80211.getIeee80211(); //ieee80211帧解析结果
    PIEEE80211_FRAME_PARAM pIeee80211FrameParam = m_Ieee80211.getIeee80211Param(); //ieee80211参数缓存
    PIEEE80211_FRAME_PARSE_PARAM pIeee80211FrameParseParam = m_Ieee80211.getIeee80211ParseParam();//ieee80211帧参数解析结果    
    if (NULL == pRadiotapData || NULL == pIeee80211Frame) {
        DebugPrint("handleframe: Failed to get parse result\r\n");
        return;
    }    
    //
    if (m_bDumppcap) {
        if (m_bFilterMac) {
            if (0 == memcmp(m_zDumpFilterMac, pIeee80211Frame->zAddress2, 6)) {
                m_WifiPcap.dumpPacket(pPkthdr, pPacket);
            }
        } else {
            m_WifiPcap.dumpPacket(pPkthdr, pPacket);
        }
    }
    
    m_nCountsOfRealPkt++;
    DebugPrint01("%s: (Valid packet: %d, Total package: %d)\r\n", m_szAdapterName, m_WifiPcap.getCountsOfPackage());
    
    //非法数据包过滤
    if (0 == memcmp(pIeee80211Frame->zAddress1, pIeee80211Frame->zAddress2, 6)) {//错误数据
        DebugPrint("handleframe: wrong packet\r\n");
        return;
    }
    
    WIRELESS_IDENTITY wirelessIdentity = { 0 };
    PWIRELESS_AP pAp = NULL;
    PWIRELESS_STATION pStation = NULL;
    PWIRELESS_DEVICE pUnknow = NULL;   
    int64_t nKey = 0;
    wirelessIdentity.nAdapterId = m_nAdapterId;
    switch (pIeee80211Frame->nDS) {
        case 0: {
            //第1个mac是DS，第2个mac是SA，第3个mac是BSSID
            if (0 == memcmp(pIeee80211Frame->zAddress3, pIeee80211Frame->zAddress2, 6)) { //BSSID==SA:AP发送的数据             
                pAp = addAp(IDENTITY_AP, pIeee80211Frame->zAddress2, pIeee80211Frame->zAddress3,
                        pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                        wirelessIdentity, pPkthdr, pPacket);
                if (NULL != pAp) {    
                    if (NULL != m_pFrameCallback) {
                        m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                                pIeee80211FrameParam, pPacket, pPkthdr->len);
                    }
                }
            } else {//BSSID!=SA:station发送的数据
                pStation = (PWIRELESS_STATION)addStation(IDENTITY_STATION, pIeee80211Frame->zAddress2, pIeee80211Frame->zAddress3,
                        pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                        wirelessIdentity, pPkthdr, pPacket);
                if (NULL != pStation) {    
                    if (NULL != m_pFrameCallback) {
                        m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                                pIeee80211FrameParam, pPacket, pPkthdr->len);
                    }
                }
            }
            break;
        }
        case 1: {
            //第1个mac是BSSID，第2个mac是SA，第3个mac是DA
            //BSSID!=SA:station发送的数据
            pStation = addStation(IDENTITY_STATION, pIeee80211Frame->zAddress2, pIeee80211Frame->zAddress1,
                    pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                    wirelessIdentity, pPkthdr, pPacket);
            if (NULL != pStation) {       
                if (NULL != m_pFrameCallback) {
                    m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                            pIeee80211FrameParam, pPacket, pPkthdr->len);
                }
            }            
            break;
        }
        case 2: {
            //第1个mac是DA，第2个mac是BSSID，第3个mac是SA
            pAp = addAp(IDENTITY_AP, pIeee80211Frame->zAddress2, pIeee80211Frame->zAddress2,
                    pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                    wirelessIdentity, pPkthdr, pPacket);
            if (NULL != pAp) {   
                if (NULL != m_pFrameCallback) {
                    m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                            pIeee80211FrameParam, pPacket, pPkthdr->len);
                }
            }
            break;
        }
        case 3: {
            //第1个mac是RA，第2个mac是TA，第3个mac是DA，第4个mac是SA
            pAp = addAp(IDENTITY_BRIDGE, pIeee80211Frame->zAddress2, pIeee80211Frame->zAddress2,
                        pRadiotapData, pIeee80211Frame, pIeee80211FrameParseParam, pIeee80211FrameParam,
                        wirelessIdentity, pPkthdr, pPacket);
            if (NULL != pAp) {   
                if (NULL != m_pFrameCallback) {
                    m_pFrameCallback(m_pObject, &wirelessIdentity, pRadiotapData, pIeee80211Frame, 
                            pIeee80211FrameParam, pPacket, pPkthdr->len);
                }
            }            
            break;
        }
        default: {
            return;
        }
    }    
    DebugPrint("AP: %d, Station: %d, Unknow: %d\r\n", m_mapAp.size(), m_mapStation.size(), m_mapUnknow.size());
}

/**
 * 超时检测函数
 */
void CRealWifiTings::outtimeThreadProc() {
    m_bOutTimeStoped = false;
    int    nOutTime = 0;
    while (!m_bOutTimeStoped) {
        try {
            if (nOutTime >= 10*1000) {//超时检测时间间隔 10秒
                m_mapAp.foreach(NULL, foreachApCallback);
                m_mapStation.foreach(NULL, foreachStationCallback);
//                m_mapUnknow.foreach(NULL, foreachUnknowCallback);
                m_mapFree.foreach(NULL, foreachFreeCallback);
                nOutTime = 0;
            }
            nOutTime += 100;
        } catch (...) {            
        }        
        //睡眠100毫秒
        usleep(100*1000);
    }
    LOGWARNING("outtimeThreadProc", "离线监视线程已退出");
}

/**
* 遍历AP表
* @param pObject--当前对象
* @param nKey--键
* @param pValue--值
*/
void CRealWifiTings::foreachApCallback(void* pObject, uint64_t nKey, void* pValue) {
    
    PWIRELESS_AP pAp = (PWIRELESS_AP)pValue;
    if (NULL == pAp) {
        return;
    }
    if (1 == pAp->wireless.nFree) {
        return;
    }
    uint64_t nNow = getNowMillisecond();
    if (0 == nNow) {
        return;
    }
    if (0 == pAp->wireless.unLasttime ) {
        pAp->wireless.unLasttime = nNow;
    }
    if (nNow > m_nOffLineTime + pAp->wireless.unLasttime) {        
        WIRELESS_IDENTITY wirelessIdentity = { 0 };
        wirelessIdentity.unOnline = 0;
        wirelessIdentity.unIdentity = pAp->wireless.unIdentity;
        memcpy(wirelessIdentity.zMac, pAp->wireless.zMac, 6);
        if (NULL != m_pOffLineCallback && 0 != pAp->wireless.unOnline) {
            m_pOffLineCallback(m_pCallbackObject, &wirelessIdentity, NULL, NULL, NULL, NULL, 0);
        }
        pAp->wireless.unOnline = 0;
        //释放长期不用的对象
        if (nNow >= m_nFreeTime + pAp->wireless.unLasttime ) {     
            DebugPrint04("AP free Long idle object: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    pAp->wireless.zMac[0], pAp->wireless.zMac[1], pAp->wireless.zMac[2], pAp->wireless.zMac[3], 
                    pAp->wireless.zMac[4], pAp->wireless.zMac[5]);
            if (NULL != m_mapAp.removeObject(nKey)) {
                pAp->wireless.nFree = 1;
                if (!m_mapFree.insertObject(nKey, pAp)) {
                    usleep(10*1000);
                    free(pAp);
                }
            }
        }
    }
}

/**
* 遍历Station表
* @param pObject--当前对象
* @param nKey--键
* @param pValue--值
*/
void CRealWifiTings::foreachStationCallback(void* pObject, uint64_t nKey, void* pValue) {
    
    PWIRELESS_STATION pStation = (PWIRELESS_STATION)pValue;
    if (NULL == pStation) {
        return;
    }
    if (1 == pStation->wireless.nFree) {
        return;
    }
    uint64_t nNow = getNowMillisecond();
    if (0 == nNow) {
        return;
    }
    if (0 == pStation->wireless.unLasttime ) {
        pStation->wireless.unLasttime = nNow;
    }
    if (nNow > m_nOffLineTime + pStation->wireless.unLasttime) {        
        pStation->nConnected = 0;
        WIRELESS_IDENTITY wirelessIdentity = { 0 };
        wirelessIdentity.unOnline = 0;
        wirelessIdentity.unIdentity = pStation->wireless.unIdentity;
        memcpy(wirelessIdentity.zMac, pStation->wireless.zMac, 6);
        if (NULL != m_pOffLineCallback && 0 != pStation->wireless.unOnline) {
            m_pOffLineCallback(m_pCallbackObject, &wirelessIdentity, NULL, NULL, NULL, NULL, 0);
        }
        pStation->wireless.unOnline = 0;
        //释放长期不用的对象
        if (nNow >= m_nFreeTime + pStation->wireless.unLasttime) {    
            DebugPrint04("Station free Long idle object: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    pStation->wireless.zMac[0], pStation->wireless.zMac[1], pStation->wireless.zMac[2], pStation->wireless.zMac[3], 
                    pStation->wireless.zMac[4], pStation->wireless.zMac[5]);
            if (NULL != m_mapStation.removeObject(nKey)) {
                pStation->wireless.nFree = 1;
                if (!m_mapFree.insertObject(nKey, pStation)) {
                    usleep(10*1000);
                    free(pStation);
                }
            }
        }
    }
}

/**
* 遍历Unknow表
* @param pObject--当前对象
* @param nKey--键
* @param pValue--值
*/
void CRealWifiTings::foreachUnknowCallback(void* pObject, uint64_t nKey, void* pValue) {
    
    PWIRELESS_DEVICE pUnknow = (PWIRELESS_DEVICE)pValue;
    if (NULL == pUnknow) {
        return;
    }
    if (1 == pUnknow->nFree) {
        return;
    }
    uint64_t nNow = getNowMillisecond();
    if (0 == nNow) {
        return;
    }
    if (0 == pUnknow->unLasttime ) {
        pUnknow->unLasttime = nNow;
    }
    if (nNow > m_nOffLineTime + pUnknow->unLasttime ) {        
        pUnknow->unOnline = 0;
        WIRELESS_IDENTITY wirelessIdentity = { 0 };
        wirelessIdentity.unOnline = 0;
        wirelessIdentity.unIdentity = pUnknow->unIdentity;
        memcpy(wirelessIdentity.zMac, pUnknow->zMac, 6);
        if (NULL != m_pOffLineCallback) {
            m_pOffLineCallback(m_pCallbackObject, &wirelessIdentity, NULL, NULL, NULL, NULL, 0);
        }
        //释放长期不用的对象
        if (nNow >= m_nFreeTime + pUnknow->unLasttime) {   
            DebugPrint04("Unknow free Long idle object: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    pUnknow->zMac[0], pUnknow->zMac[1], pUnknow->zMac[2], pUnknow->zMac[3], 
                    pUnknow->zMac[4], pUnknow->zMac[5]);
            if (NULL != m_mapUnknow.removeObject(nKey)) {
                pUnknow->nFree = 1;
                if (!m_mapFree.insertObject(nKey, pUnknow)) {
                    usleep(10*1000);
                    free(pUnknow);
                }
            }
        }
    }
}

/**
* 遍历Free表
* @param pObject--当前对象
* @param nKey--键
* @param pValue--值
*/
void CRealWifiTings::foreachFreeCallback(void* pObject, uint64_t nKey, void* pValue) {
    
    PWIRELESS_DEVICE pUnknow = (PWIRELESS_DEVICE)pValue;
    if (NULL == pUnknow) {
        return;
    }
    uint64_t nNow = getNowMillisecond();
    if (nNow > WIRELEE_FREE_WAIT_TIME + pUnknow->nLastSignal) {
        if (NULL != m_mapFree.removeObject(nKey)) {
            DebugPrint04("free Long idle object: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    pUnknow->zMac[0], pUnknow->zMac[1], pUnknow->zMac[2], pUnknow->zMac[3], 
                    pUnknow->zMac[4], pUnknow->zMac[5]);
            free(pValue);            
        }
    }    
}
