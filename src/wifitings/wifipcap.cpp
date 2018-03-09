/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wifipcap.cpp
 * Author: dyer
 * 
 * Created on 2017年2月6日, 下午5:42
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../../tools/cmd.h"
#include "../../include/DbgPrint.h"
#include "wifipcap.h"

/**
 * 构造函数
 */
CWifiPcap::CWifiPcap() {
    memset(m_szAdapterName, 0x00, sizeof(m_szAdapterName));
    memset(m_szDumpFile, 0x00, sizeof(m_szDumpFile));
    m_pThreadWorker = NULL;
    m_pcapDevice = NULL;
    m_pDumpFile = NULL;
    m_bDumpFile = false;
    m_pCallbackProc = NULL;
    m_pUser = NULL;
    m_itCountsOfPkt = 0;
    m_bStoped = true;
}

/**
 * 析构函数
 */
CWifiPcap::~CWifiPcap() {
    stop();
}

/**
 * 设置驱动
 * @param pszAdapterName--网卡名
 * @return true：成功，false：失败
 */
bool CWifiPcap::setAdapterName(const char* pszAdapterName) {
    if (NULL == pszAdapterName) {
        return false;
    }
    snprintf(m_szAdapterName, sizeof(m_szAdapterName), "%s", pszAdapterName);
    
    return true;
}

/**
 * 得到已抓包数
 * @return 
 */
u_int64_t CWifiPcap::getCountsOfPackage() {
    return m_itCountsOfPkt;
}

/**
* 保存数据包到文件中
* @param pPkthdr--pcap包信息
* @param pPacket--pcap包数
*/
void CWifiPcap::dumpPacket(const struct pcap_pkthdr * pPkthdr, const u_char*pPacket) {
    
    if (NULL == pPkthdr || pPkthdr->len <= 0 || NULL == pPacket) {
        return;
    }
    if (m_bDumpFile && NULL != m_pDumpFile) {
        pcap_dump((u_char *)m_pDumpFile, pPkthdr, pPacket);
        pcap_dump_flush(m_pDumpFile);
    }
}

/**
 * 发送数据包
 * @param pData--数据
 * @param nLen--数据长度
 * @return 0：成功，-1失败
 */
int CWifiPcap::sendpacket(const u_char* pData, int nLen) {
    if (NULL == pData || nLen <= 0 || m_pcapDevice == NULL) {
        return -1;
    }
    return pcap_sendpacket(m_pcapDevice, pData, nLen);
}

/**
 * 设置dump文件
 * @param pszDumpFile--抓包保存文件
 * @return true：成功，false：失败
 */
bool CWifiPcap::setDump(const char *pszDumpFile) {
    if (NULL == pszDumpFile) {
        return false;
    }
    strcpy(m_szDumpFile, pszDumpFile);
    m_bDumpFile = true;
    m_pDumpFile = NULL;
    
    return true;
}

/**
 * 设置回调函数
 * @param pUser--回调时需要传给调用者
 * @param pCallback--函数指针
 * @return true：成功，false：失败
 */
bool CWifiPcap::setCallbackProc(unsigned char* pUser, pcap_handler pCallback) {
    m_pCallbackProc = pCallback;
    m_pUser = pUser;
    
    return true;
}

/**
 * 启动
 * @return true：成功，false：失败
 */
bool CWifiPcap::start() {
    
    if (!m_bStoped) {
        return true;
    }
    
    //设置网卡为监听模式
    if (!setMonitor()) {
        LOGERROR("start","[%s]:设置网卡监视模式失败", m_szAdapterName);
        return false;
    }
    
    char szErrBuf[PCAP_ERRBUF_SIZE] = { 0 };
//    m_pcapDevice = pcap_open_offline("/home/leeyea/svn/WifiSkyNet/Codes/device-client/bin/dbg/wlan1.pcap", szErrBuf);
    m_pcapDevice = pcap_open_live(m_szAdapterName, 65535, 1, 0, szErrBuf);    
    if(NULL == m_pcapDevice)
    {
        DebugPrint("error: pcap_open_live(): %s\n", szErrBuf);
        LOGERROR("start","[%s]:打开抓包对象失败", m_szAdapterName);
        return false;
    }

 //   int nRet = pcap_datalink(m_pcapDevice);
    //设置链路层类型
    if (0 != pcap_set_datalink(m_pcapDevice, DLT_IEEE802_11_RADIO)) {
        char* pError = pcap_geterr(m_pcapDevice);
        DebugPrint("error: pcap_set_datalink(): %s\n", pError);
        LOGERROR("start","[%s]:设置抓包对象失败", m_szAdapterName);
        stop();
        return false;
    }
    
    try {
        m_pThreadWorker = new boost::thread(__worker_proc, this);
        if (NULL == m_pThreadWorker) {
            LOGERROR("start","[%s]:启动抓包对象线程失败", m_szAdapterName);
            return false;
        }
    } catch(boost::exception& e) {
        LOGABORT("start","[%s]:启动抓包对象线程异常", m_szAdapterName);
        return false;
    }
    LOGSUCCESS("start","[%s]:启动抓包对象成功", m_szAdapterName);
    
    return true;
}

/**
 * 停止
 * @return true：成功，false：失败
 */
bool CWifiPcap::stop() {    
    try {
        if (!m_bStoped && NULL != m_pcapDevice) {
            m_bStoped = true;
            
            pcap_breakloop(m_pcapDevice);
        }
        m_bStoped = true;   
        usleep(10000);

        //关闭转包文件
        if (NULL != m_pDumpFile) {
            pcap_dumper_t* pDumpFile = m_pDumpFile;
            m_pDumpFile = NULL;
            pcap_dump_close(pDumpFile);        
        }  
        //关闭pcap抓包对象
        if (NULL != m_pcapDevice) {
            pcap_close(m_pcapDevice);  
            m_pcapDevice = NULL;
        } 
        
        //退出线程
        try {
            if (NULL != m_pThreadWorker) {
//                m_pThreadWorker->timed_join(boost::posix_time::microseconds(500)); 
                m_pThreadWorker->join();
                delete m_pThreadWorker;
                m_pThreadWorker = NULL;
            }        
        } catch (boost::exception& e) {
            LOGABORT("stop","[%s]:停止抓包对象线程异常", m_szAdapterName);
            return false;
        }    

        LOGSUCCESS("stop", "[%s]:停止抓包对象成功", m_szAdapterName);    
        return true;
    } catch (...) {
        LOGABORT("stop","[%s]:停止异常", m_szAdapterName);
    }
    
    return false;
}

/**
* 得到运行状态
* @return true：正在运行，false：已经停止
*/
bool CWifiPcap::getRunningState() {
    if (m_bStoped) {
        return false;
    }
    
    return true;
}

/**
 * 设置过滤器
 * @param pFilter--过滤表达式
 * @return true:成功，false：失败
 */
 bool CWifiPcap::setPcapFilter(const char* pszFilter) {
     
     if (NULL == m_pcapDevice) {
         return false;
     }     
     struct bpf_program filter;
     if (-1 == pcap_compile(m_pcapDevice, &filter, pszFilter, 1, 0) ) {
         LOGERROR("setPcapFilter", "[%s]:编译包过滤条件失败", m_szAdapterName);
         return false;
     }
     if ( - 1 == pcap_setfilter(m_pcapDevice, &filter)) {
         LOGERROR("setPcapFilter", "[%s]:设置包过滤条件失败", m_szAdapterName);
         return false;
     }
     
     return true;
 }
 
 /**
 * 设置监听模式
 * @return true：成功，false：失败
 */
bool CWifiPcap::setMonitor() {
    char szCmd[512] = { 0 };
    
    memset(szCmd, 0x00, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "ifconfig %s down", m_szAdapterName);
    CCmd cmd;
    cmd.open(szCmd, "r");
    cmd.close();
//    std::system(szCmd);
    
    memset(szCmd, 0x00, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "iw dev %s set monitor none", m_szAdapterName);
    cmd.open(szCmd, "r");
    cmd.close();
//    std::system(szCmd);
    
    memset(szCmd, 0x00, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "ifconfig %s up", m_szAdapterName);
    cmd.open(szCmd, "r");
    cmd.close();
//    std::system(szCmd);
    
    return true;
}

/**
 * 线程执行函数
 */
void CWifiPcap::excute()
{
    try {
//        if (0 == strcmp(m_szAdapterName, "wls35u3")) {
//            int nBreak = 0;
//        }
        if (m_bDumpFile) {
            m_pDumpFile = pcap_dump_open(m_pcapDevice, m_szDumpFile);
        }
        while (!m_bStoped) {
            pcap_loop(m_pcapDevice, -1, callback, (u_char*)this);
            DebugPrint("%s: pcap_loop break\r\n", m_szAdapterName);
        }
    } catch (...) {        
        DebugPrint("%s: pcap_loop abnor\r\n", m_szAdapterName);
        LOGABORT("excute", "[%s]:抓包线程异常", m_szAdapterName);
    }
    
    DebugPrint("%s: Capture the end\r\n", m_szAdapterName);
    LOGWARNING("excute", "[%s]:抓包线程已经退出", m_szAdapterName);

//    if (NULL != m_pDumpFile) {
//        pcap_dump_close(m_pDumpFile);
//        m_pDumpFile = NULL;
//    }
//    if (NULL != m_pcapDevice) {
//        pcap_close(m_pcapDevice);  
//        m_pcapDevice = NULL;
//    } 
}

////////////////////////////////////////////////////////////////////////////
//函数名成：__worker_proc
//函数功能：工作线程函数
//参数说明：pThis--线程对象
//返 回 值：
//备   注：
////////////////////////////////////////////////////////////////////////////
void CWifiPcap::__worker_proc(void* pThis)
{
    CWifiPcap* lpThis = (CWifiPcap*)pThis;

    if (NULL == lpThis) {
        return;
    }

    lpThis->m_bStoped = false;
    lpThis->excute();
    lpThis->m_bStoped = true;
}

void CWifiPcap::callback(u_char* pObject, const struct pcap_pkthdr * pPkthdr, const u_char*pPacket) {
    CWifiPcap* pThis = (CWifiPcap*)pObject;
    if (NULL == pThis || NULL == pPkthdr || NULL == pPacket) {
        return;
    }
    try {
//        if (0 == strcmp(pThis->m_szAdapterName, "wls35u3")) {
//            int nBreak = 0;
//        }
        if (pThis->m_bStoped) {
            if (NULL != pThis->m_pcapDevice) {
                pcap_breakloop(pThis->m_pcapDevice);
            }
            return;
        }

        pThis->m_itCountsOfPkt++;

        TmpDebugPrint("callback: caplen: %o, len:%o\r\n", pPkthdr->caplen, pPkthdr->len);
        if (NULL != pThis->m_pCallbackProc) {
            pThis->m_pCallbackProc(pThis->m_pUser, pPkthdr, pPacket);
        }
    } catch (...) {        
        DebugPrint("%s: Capture abnor\r\n", pThis->m_szAdapterName);
    }
}