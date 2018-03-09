/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string.h>
#include "realwifitings.h"
#include "../include/wifitingsinterface.h"


/**
* 获取无线网卡听包对象指针
* @return != NULL: 成功，== NULL:失败
*/
CWifiTings* getWifiTingsInstance() {
    try {
        CRealWifiTings* pObject = new CRealWifiTings();
        if (NULL == pObject) {
            return NULL;
        }
        
        return pObject;
    } catch (...) {
        LOGABORT("getInstance", "创建WifiTings实例异常");
    }

    return NULL;
}

/**
* 释放无线网卡听包服务对象
* @return -1：失败，0：成功
*/
int releaseWifiTingsInstance(CWifiTings* pWifiTings) {
    try {
        if (NULL != pWifiTings) {
            delete pWifiTings;
            return 0;
        }
    } catch (...) {
        LOGABORT("releaseInstance", "释放WifiTings实例异常");
    }
    
    return -1;
}

/**
 * 注册离线监视器回调函数
 * @param pObject--调用对象，将作为pFrameCallback的第一个参数
 * @param pOfflineCallback--回调函数
 * @return true：成功，false：失败
 */
bool registeOfflineCallback(void* pObject, PFRAME_CALLBACK pOfflineCallback) {
    CRealWifiTings::m_pCallbackObject = pObject;
    CRealWifiTings::m_pOffLineCallback = pOfflineCallback;
    return true;
}

/**
 * 启动离线监视器
 * @return true：成功，false：失败
 */
bool startOffineMonitor() {
    try {
        if (!CRealWifiTings::m_bOutTimeStoped) {
            LOGWARNING("startOffineMonitor", "离线监视器已经启动");
            return true;
        }
        CRealWifiTings::m_pOutTimeThreadWorker = new boost::thread(CRealWifiTings::outtimeThreadProc);
        if (NULL == CRealWifiTings::m_pOutTimeThreadWorker) {
            LOGERROR("startOffineMonitor", "启动离线监视器线程失败");
            return false;
        }
        LOGSUCCESS("startOffineMonitor", "启动离线监视器线程成功");
    } catch(boost::exception& e) {
        LOGABORT("startOffineMonitor", "启动离线监视器线程异常");
        return false;
    } catch(...)  {
        LOGABORT("startOffineMonitor", "启动离线监视器线程异常");
        return false;
    }
    
    return true;
}

/**
 * 停止离线监视器
 * @return true：成功，false：失败
 */
bool stopOffineMonitor() {
    CRealWifiTings::m_bOutTimeStoped = true;
    //退出线程
    try {
        if (NULL != CRealWifiTings::m_pOutTimeThreadWorker) {
            CRealWifiTings::m_pOutTimeThreadWorker->join();
            delete CRealWifiTings::m_pOutTimeThreadWorker;
            CRealWifiTings::m_pOutTimeThreadWorker = NULL;
        }     
        LOGSUCCESS("stopOffineMonitor", "停止离线监视器线程成功");
    } catch (boost::exception& e) {
        LOGABORT("stopOffineMonitor", "停止离线监视器线程异常");
        return false;
    }
    
    return true;
}

/**
 * 得到离线监视器的状态
 * @return true：正在运行，false：已停止
 */
bool getOffineMonitorState() {
    
    if (CRealWifiTings::m_bOutTimeStoped) {
        return false;
    }
    
    return true;
}

/**
 * 设置离线超时时间
 * @nOutTime--离线超时时间，单位毫秒
 * @return true：成功，false：失败
 */
bool setOffineTime(uint64_t nOutTime) {
    CRealWifiTings::m_nOffLineTime = nOutTime;
    CRealWifiTings::m_nFreeTime = 2*nOutTime;
    
    return true;
}