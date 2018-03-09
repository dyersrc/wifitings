/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wifitingsinterface.h
 * Author: dyer
 *
 * Created on 2017年2月4日, 上午11:23
 */

#ifndef WIFITINGSINTERFACE_H
#define WIFITINGSINTERFACE_H

#ifdef WIFITINGS_EXPORTS
#define WIFITINGS_API __attribute__((visibility("default")))
#else
#define WIFITINGS_API 
#endif

#include "wifitings.h"

#ifdef __cplusplus/*定义调用约定*/
extern "C"
{
#endif

/**
* 获取无线网卡听包对象指针
* @return != NULL: 成功，== NULL:失败
*/
WIFITINGS_API CWifiTings* getWifiTingsInstance();

/**
* 释放无线网卡听包服务对象
* @return -1：失败，0：成功
*/
WIFITINGS_API int releaseWifiTingsInstance(CWifiTings* pWifiTings);

/**
 * 注册离线监视器回调函数
 * @param pObject--调用对象，将作为pFrameCallback的第一个参数
 * @param pOfflineCallback--回调函数
 * @return true：成功，false：失败
 */
WIFITINGS_API bool registeOfflineCallback(void* pObject, PFRAME_CALLBACK pOfflineCallback);

/**
 * 启动离线监视器
 * @return true：成功，false：失败
 */
WIFITINGS_API bool startOffineMonitor();

/**
 * 停止离线监视器
 * @return true：成功，false：失败
 */
WIFITINGS_API bool stopOffineMonitor();

/**
 * 得到离线监视器的状态
 * @return true：正在运行，false：已停止
 */
WIFITINGS_API bool getOffineMonitorState();

#ifdef __cplusplus/*定义调用约定*/
};
#endif

#endif /* WIFITINGSINTERFACE_H */

