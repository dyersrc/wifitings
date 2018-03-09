/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wifipcap.h
 * Author: dyer
 *
 * Created on 2017年2月6日, 下午5:42
 */

#ifndef WIFIPCAP_H
#define WIFIPCAP_H

#define BOOST_DATE_TIME_SOURCE  
#define BOOST_THREAD_NO_LIB 

#include <pcap.h>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../../include/leelog.h"

using namespace boost;
using namespace std;

class CWifiPcap {
    CWifiPcap(const CWifiPcap& orig);
public:
    CWifiPcap();    
    virtual ~CWifiPcap();
    
    /**
    * 设置驱动
    * @param pszAdapterName--网卡名
    * @return true：成功，false：失败
    */
    bool setAdapterName(const char* pszAdapterName);   
    
    /**
    * 设置dump文件
    * @param pszDumpFile--抓包保存文件
    * @return true：成功，false：失败
    */
    bool setDump(const char *pszDumpFile);
    
    /**
    * 设置回调函数
    * @param pUser--回调时需要传给调用者
    * @param pCallback--函数指针
    * @return true：成功，false：失败
    */
    bool setCallbackProc(unsigned char* pUser, pcap_handler pCallback);
       
    /**
    * 启动
    * @return true：成功，false：失败
    */
    bool start();

    /**
    * 停止
    * @return true：成功，false：失败
    */
    bool stop();
    
    /**
     * 得到运行状态
     * @return true：正在运行，false：已经停止
     */
    bool getRunningState();
    
    /**
    * 设置过滤器
    * @param pFilter--过滤表达式
    * @return true:成功，false：失败
    */
    bool setPcapFilter(const char* pszFilter);
    
    /**
    * 得到已抓包数
    * @return 
    */
    u_int64_t getCountsOfPackage();
    
    /**
    * 发送数据包
    * @param pData--数据
    * @param nLen--数据长度
    * @return 0：成功，-1失败
    */
    int sendpacket(const  unsigned char* pData, int nLen);
    
    /**
     * 保存数据包到文件中
     * @param pPkthdr--pcap包信息
     * @param pPacket--pcap包数
     */
    void dumpPacket(const struct pcap_pkthdr * pPkthdr, const unsigned char* pPacket);
    
protected:   
     /**
    * 设置监听模式
    * @return true：成功，false：失败
    */
    bool setMonitor();
    
    /**
    * 线程执行函数
    */
    void excute();
    
    //工作线程函数
    static void __worker_proc(void* pThis);
    
    //
    static void callback(u_char* pObject, const struct pcap_pkthdr * pPkthdr, const u_char*pPacket);
    
protected:
    char            m_szDumpFile[512];             //dump文件
    pcap_dumper_t* m_pDumpFile;             //dump文件
    bool            m_bDumpFile;                     //dump标志
    
    char            m_szAdapterName[256];       //网卡名
    pcap_t*       m_pcapDevice;                   //网卡抓包对象
    
    pcap_handler m_pCallbackProc;               //保存setCallbackProc的第二个参数
    unsigned char*       m_pUser;                          //保存setCallbackProc的第一个参数
    
    u_int64_t    m_itCountsOfPkt;
    
    boost::thread* m_pThreadWorker;            //检测前端状态维护线程
    volatile bool     m_bStoped;                        //线程被外部停止标志    
};

#endif /* WIFIPCAP_H */

