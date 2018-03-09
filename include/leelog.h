/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   leelog.h
 * Author: user
 *
 * Created on 2016年12月14日, 下午5:28
 */

#ifndef LEELOG_H
#define LEELOG_H


#ifdef  _WINDOWS

#ifdef LEELOG_EXPORTS
#define LEELOG_API __declspec(dllexport)  
#else
#define LEELOG_API __declspec(dllimport)
#endif

#else

#ifdef LEELOG_EXPORTS
#define LEELOG_API __attribute__((visibility("default")))
#else
#define LEELOG_API 
#endif

#endif 


#ifndef __LEVEL__

#define  LEVEL_ZERO             0x00         //零级别，不输出任何日志
#define  LEVEL_ERROR           0x01         //错误 
#define  LEVEL_ABORT           0x02         //异常
#define  LEVEL_SUCCESS       0x03         //成功
#define  LEVEL_WARNINGS     0x04         //警告
#define  LEVEL_INFORMATION 0x05         //信息
#define  LEVEL_DEBUG          0x06         //调式

#define  LEVEL_ZERO_SWITCH             //零级别，不输出任何日志,编译开关
#define  LEVEL_ERROR_SWITCH           //错误,编译开关
#define  LEVEL_SUCCESS_SWITCH        //成功,编译开关
#define  LEVEL_WARNINGS_SWITCH      //警告,编译开关
#define  LEVEL_INFORMATION_SWITCH  //信息,编译开关
#define  LEVEL_ABORT_SWITCH           //异常,编译开关
#define  LEVEL_DEBUG_SWITCH           //调式,编译开关

#endif

#include <errno.h>

#ifdef LEVEL_ZERO_SWITCH

#define INITLOG(pszLogFile,nMaxSize,nMaxNum, nLevel) InitLog(pszLogFile,nMaxSize,nMaxNum,-1, nLevel)

#define INITSYNLOG(pszLogFile, nLevel) InitLog(pszLogFile,1024*1024*5,5,-1, nLevel)

#define INITASYNLOG(pszLogFile, nLevel) InitLog(pszLogFile,1024*1024*5,5,100, nLevel)

#define CLOSELOG() CloseLog()

#ifdef LEVEL_ERROR_SWITCH
#define LOGERROR(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_ERROR,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else
#define LOGERROR(pszMoudle,pszDiscription,...) 
#endif

#ifdef LEVEL_ABORT_SWITCH
#define LOGABORT(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_ABORT,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else
#define LOGABORT(pszMoudle,pszDiscription,...)
#endif

#ifdef LEVEL_SUCCESS_SWITCH
#define LOGSUCCESS(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_SUCCESS,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else
#define LOGSUCCESS(pszMoudle,pszDiscription,...) 
#endif

#ifdef LEVEL_WARNINGS_SWITCH
#define LOGWARNING(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_WARNINGS,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else
#define LOGWARNING(pszMoudle,pszDiscription,...)
#endif

#ifdef LEVEL_INFORMATION_SWITCH
#define LOGINFO(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_INFORMATION,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else 
#define LOGINFO(pszMoudle,pszDiscription,...)
#endif

#ifdef LEVEL_DEBUG_SWITCH
#define LOGDEBUG(pszMoudle,pszDiscription,...) \
	WriteLog(LEVEL_DEBUG,errno,pszMoudle,__FILE__,__LINE__,pszDiscription,##__VA_ARGS__)
#else
#define LOGDEBUG(pszMoudle,pszDiscription,...) 
#endif

#else

#define INITLOG(pszLogFile,nMaxSize,nMaxNum, nLevel)
#define INITSYNLOG(pszLogFile, nLevel)
#define INITASYNLOG(pszLogFile, nLevel)
#define CLOSELOG()
#define LOGERROR(pszMoudle,pszDiscription,...)
#define LOGABORT(pszMoudle,pszDiscription,...)
#define LOGSUCCESS(pszMoudle,pszDiscription,...) 
#define LOGWARNING(pszMoudle,pszDiscription,...)
#define LOGINFO(pszMoudle,pszDiscription,...)
#define LOGDEBUG(pszMoudle,pszDiscription,...)

#endif

#ifdef __cplusplus/*定义调用约定*/
extern "C"
{
#endif

/**
 * 初始化函数
 * @param pszLogFile--日志文件路径
 * @param nMaxSize--文件最大大小
 * @param nMaxNum--日志最大大小
 * @param nWriteTs--最大日志文件数
 * @param nLevel--输出最大日志级别
 * @return 0：成功返回0，失败返回1：参数错误 2：创建文件失败 3：分配错误 4：创建线程失败 5：为初始化文件
 *               6：数据处理错误 7：异常
 */
LEELOG_API int InitLog(const char* pszLogFile, int nMaxSize, int nMaxNum, int nWriteTs, int nLevel);

/**
 * 写日志
 * @param nLevel--日志级别
 * @param nLastErr--错误码
 * @param pszMoudle--模块名称
 * @param pszFile--日志记录文件
 * @param nLine--日志记录行数
 * @param pszDiscription--日志记录描述
 * @param ...
 * @return 成功返回0，失败返回1：参数错误 2：创建文件失败 3：分配错误 4：创建线程失败 5：初始化文件错误
 *              6：数据处理错误 7：异常 8:日志级别大于设定值
 */
LEELOG_API int WriteLog(int nLevel, int nLastErr, const char* pszMoudle, const char* pszFile,
        int nLine, const char* pszDiscription, ...);

/**
 * 关闭日志
 * @return 0：成功，1：失败
 */
LEELOG_API int CloseLog();

#ifdef __cplusplus/*定义调用约定*/
};
#endif

#endif /* LEELOG_H */

