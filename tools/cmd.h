//////////////////////////////////////////////////////////////////////////
// Copyright@2010, XXXXXXXXXXXXXXXXX
// All rights reserved.
// 文件名称：cmd.h
// 文件标识：
// 摘    要：popen管道类定义
//
// 当前版本：1.0.0
// 作    者：dyer
// 完成时间：2015年7月22日
//
// 取代版本：
// 原作者：
// 完成日期：
//////////////////////////////////////////////////////////////////////////

#ifndef CMD_H
#define	CMD_H

#include <stdio.h>

class CCmd
{
private:
    CCmd(const CCmd& orig);
public:
    CCmd();
    virtual ~CCmd();
    
    //打开命令管道
    bool open(const char* pszCmd, const  char* pszType);
    
    //输出命令执行结果,
    bool out(char* pszLine, int nBytesOfLine, int nWaitTime = 0);
    
    //输入一行命令
    bool in(const char* pszLine);
    
    //关闭命令管道
    bool close();

protected:
    //线程执行函数
    void* excute();

private:
    FILE*           m_pFStream;
    int               m_nType;
};

#endif	/* CMD_H */

