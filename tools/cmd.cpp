//////////////////////////////////////////////////////////////////////////
// Copyright@2010, XXXXXXXXXXXXXXXXX
// All rights reserved.
// 文件名称：cmd.cpp
// 文件标识：
// 摘    要：popen管道类实现
//
// 当前版本：1.0.0
// 作    者：dyer
// 完成时间：2015年7月22日
//
// 取代版本：
// 原作者：
// 完成日期：
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <unistd.h>
#include "cmd.h"
#include "../include/DbgPrint.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////
//函数名成：CCmd
//函数功能：构造函数
//参数说明：
//返  回  值：
//备        注：
////////////////////////////////////////////////////////////////////////////
CCmd::CCmd() 
{
    m_pFStream = NULL;
    m_nType = 0;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：CCmd
//函数功能：构造拷贝函数
//参数说明：
//返  回  值：
//备        注：
////////////////////////////////////////////////////////////////////////////
CCmd::CCmd(const CCmd& orig)
{
    m_nType = 0;
    m_pFStream = NULL;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：~CCmd
//函数功能：析构函数
//参数说明：
//返  回  值：
//备        注：
////////////////////////////////////////////////////////////////////////////
CCmd::~CCmd() 
{
    close();
}

////////////////////////////////////////////////////////////////////////////
//函数名成：open
//函数功能：打开命令管道
//参数说明：pszCmd--需要在命令行中执行的命令
//                   pszType--管道类型r读，w写
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
 bool CCmd::open(const char* pszCmd, const  char* pszType)
 {
     if (NULL ==  pszCmd) {
         return false;
     }
     close();     
     if (0 == strcmp(pszType, "r") ||
         0 == strcmp(pszType, "R")) {
         m_nType = 1;
     }
     else if (0 == strcmp(pszType, "w") ||
                 0 == strcmp(pszType, "W")) {
         m_nType = 2;
     } else {
         return false;
     }
     switch (m_nType) {
         case 1: {//读{
             m_pFStream = popen(pszCmd, "r");
             if (NULL == m_pFStream) {
                 return false;
             }
             break;
         }
         case 2: {//写
             m_pFStream = popen(pszCmd, "w");
              if (NULL == m_pFStream) {
                  return false;
              }
             break;
         }
         default: {
             return false;
         }
     }
     
     return true;
 }
 
////////////////////////////////////////////////////////////////////////////
//函数名成：close
//函数功能：关闭命令管道
//参数说明：
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
 bool CCmd::close()
 {
     if (NULL != m_pFStream) {
         pclose(m_pFStream);
         m_pFStream = NULL;
     }
     
     return true;
 }
 
 ////////////////////////////////////////////////////////////////////////////
//函数名成：out
//函数功能：输出一行命令执行结果
//参数说明：pszLine--接收缓冲区
//                   nBytesOfLine--接收缓冲区大小
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
 bool CCmd::out(char* pszLine, int nBytesOfLine, int nWaitTime)
 {
    if (NULL == m_pFStream || 1 != m_nType) {
        return false;
    }
    if (nWaitTime > 0) {
        int nOutTime = 0;
        int bRet = true;
        while (NULL == fgets(pszLine,  nBytesOfLine, m_pFStream)) {
                if (nOutTime > nWaitTime) {
                        bRet = false;
                        break;
                }
                nOutTime += 500;
                usleep(500*1000);
        }
        return bRet;
   }
   else
   {
        if (NULL != fgets(pszLine,  nBytesOfLine, m_pFStream)) {
            return true;
         }
        return false;
   }

    return false;
 }
 
 ////////////////////////////////////////////////////////////////////////////
//函数名成：in
//函数功能：输入一行命令
//参数说明：pszLine--输入命令
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
 bool CCmd::in(const char* pszLine)
 {
     if (NULL == m_pFStream || 2 != m_nType) {
         return false;
     }
     if (EOF == fputs(pszLine,  m_pFStream)) {
         return false;
     }
     
     return true;
 }
