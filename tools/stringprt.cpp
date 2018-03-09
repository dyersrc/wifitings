
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include "time.h"
#include "stringprt.h"


////////////////////////////////////////////////////////////////////////////
//函数名成：strpos
//函数功能：查找子串
//参数说明：pszStr--字符串
//        nBytesOfSrc--字符串字节数
//        pszSub--子串
//返 回 值：成功返回子串位置，失败返回－1
//备    注：
////////////////////////////////////////////////////////////////////////////
int strpos(const char *pszSrc, int nBytesOfSrc, const char *pszSub)
{
	int i = 0;
	int nLen = 0;

	if (NULL == pszSrc || nBytesOfSrc <= 0 || NULL == pszSub)
	{
		return -1;
	}
	nLen = strlen(pszSub);
	while (i < nBytesOfSrc && 0 != pszSrc[i])
	{
		if (pszSrc[i] == pszSub[0] && nBytesOfSrc - i >= nLen)
		{
			if (0 == memcmp(&pszSrc[i], pszSub, nLen))
			{
				return i;
			}
		}
		i++;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：trimspace
//函数功能：去除开头和结尾的空格与换行回车符
//参数说明：pszSrc--原字符串
//                      pszDest--接收缓冲区
//                      nBytesOfDest--接收缓冲区大小
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool trimspace(const char* pszSrc, char* pszDest, int nBytesOfDest)
{
	int nSrcLen = 0;
	int nStart = 0;
	int nEnd = 0;

	if (NULL == pszSrc || NULL == pszDest || nBytesOfDest <= 0)
	{
		return false;
	}
	memset(pszDest, 0x00, nBytesOfDest);
	nSrcLen = strlen(pszSrc);
	if (nSrcLen <= 0)
	{//原字符串为空
		return true;
	}
	//找到第一个不为空格|\r|\n的位置
	while (nStart < nSrcLen)
	{
		if (' ' != pszSrc[nStart] && '\r' !=  pszSrc[nStart] && '\n'  != pszSrc[nStart] && 0 != pszSrc[nStart] && '\t' != pszSrc[nStart])
		{
			break;
		}
		nStart++;
	}
	//找到末尾第一个空格的位置
	nEnd = nSrcLen;
	while (nEnd > nStart)
	{
		if (' ' != pszSrc[nEnd] && '\r' !=  pszSrc[nEnd] && '\n'  != pszSrc[nEnd] && 0 != pszSrc[nEnd] && '\t' != pszSrc[nEnd])
		{
			break;
		}
		nEnd--;
	}
	if (nBytesOfDest <= (nEnd - nStart + 1))
	{//接收缓冲区不足
		return false;
	}
	memcpy(pszDest, &pszSrc[nStart], nEnd - nStart + 1);
	pszDest[nEnd - nStart + 1] = 0;

	return true;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：extractSuffix
//函数功能：提取文件扩展名
//参数说明：pszFileName--
//        pszSuffix--
//        nBytesOfSuffix--
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool extractSuffix(const char* pszFileName, char* pszSuffix, int nBytesOfSuffix)
{
	int      nLen = 0;
	int      i = 0;

	if (NULL == pszFileName || NULL == pszSuffix || nBytesOfSuffix <= 0)
	{
		return false;
	}
	nLen = strlen(pszFileName);
	i = nLen;
	while (i > 0 && (nLen - i) < nBytesOfSuffix)
	{
		if ('.' == pszFileName[i])
		{
			strcpy(pszSuffix, &pszFileName[i]);
			return true;
		}
		i--;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：strupcase
//函数功能：字符串大写
//参数说明：pszStr--字符串
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool strupcase (char* pszStr)
{
	int nLen = 0;

	if (NULL ==  pszStr)
	{
		return false;
	}

	nLen = strlen(pszStr);
	if (0 >= nLen)
	{
		return true;
	}
	for (int i = 0; i < nLen; i++)
	{
		if (pszStr[i] >= 'a' && pszStr[i] <= 'z')
		{
			pszStr[i] = pszStr[i] - 32;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：strlower
//函数功能：字符串小写
//参数说明：pszStr--字符串
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool strlower (char* pszStr)
{
	int nLen = 0;

	if (NULL ==  pszStr)
	{
		return false;
	}

	nLen = strlen(pszStr);
	if (0 >= nLen)
	{
		return true;
	}
	for (int i = 0; i < nLen; i++)
	{
		if (pszStr[i] >= 'A' && pszStr[i] <= 'Z')
		{
			pszStr[i] = pszStr[i] + 32;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：getnow
//函数功能：获取系统当前时间
//参数说明：pszNow--接收时间字符串缓冲区 如2014-01-01@15_12_41
//        nBytesOfNow--接收时间字符串缓冲区大小
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool getnow(char* pszNow, int nBytesOfNow)
{
	char               szNow[260] = { 0 };
	time_t             tNow;
	struct tm*         ptmNow;

	if (NULL == pszNow || 0 >= nBytesOfNow)
	{
		return false;
	}

	time(&tNow);
	ptmNow = localtime(&tNow);
	if (NULL == ptmNow)
	{
		return false;
	}
	sprintf(szNow, "%4d-%02d-%02d@%02d_%02d_%02d",
			1900 + ptmNow->tm_year,1 + ptmNow->tm_mon, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	if (nBytesOfNow <= (int)strlen(szNow))
	{
		return false;
	}
	memset(pszNow, 0x00, nBytesOfNow);
	strcpy(pszNow, szNow);

	return true;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：isdigit
//函数功能：判断字符串是否是数字
//参数说明：pszSrc--数字字符串
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
bool isdigit(char* pszSrc)
{
	if (NULL == pszSrc)
	{
		return false;
	}
	for (int i = 0; i < (int)strlen(pszSrc); i++)
	{
		if (pszSrc[i] < '0' || pszSrc[i] > '9')
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：getword
//函数功能：得到第一个单词
//参数说明：pszSrc--原字符串
//        pszWord--接收单词缓冲区
//        nBytesOfWord--接收单词缓冲区大小
//返  回  值：成功返回true，失败返回false
//备        注：
////////////////////////////////////////////////////////////////////////////
int getword(const char* pszSrc, char* pszWord, int nBytesOfWord)
{
	int          nLen = 0;
	int          nStart = 0;
	int          nEnd = 0;

	if (NULL == pszSrc || NULL == pszWord || 0 >= nBytesOfWord)
	{
		return -1;
	}
	nLen = strlen(pszSrc);
	//得到单词起始位置
	while (nStart < nLen)
	{
		if (0 != pszSrc[nStart] && ' ' != pszSrc[nStart] && '\r' != pszSrc[nStart] && '\n' != pszSrc[nStart] && '\t' != pszSrc[nStart])
		{
			break;
		}
		nStart++;
	}
	if (nStart >= nLen)
	{//已经是字符串结尾
		return -1;
	}
	//得到单词结束位置
	nEnd = nStart;
	while (nEnd < nLen)
	{
		if (0 == pszSrc[nEnd] || ' ' == pszSrc[nEnd] || '\r' == pszSrc[nEnd] || '\n' == pszSrc[nEnd] || '\t' == pszSrc[nEnd])
		{
			break;
		}
		nEnd++;
	}
	if (nBytesOfWord <= (nEnd - nStart + 1))
	{//接收缓冲区不足
		return -1;
	}
	memset(pszWord, 0x00, nBytesOfWord);
	memcpy(pszWord, &pszSrc[nStart], nEnd - nStart);
	pszWord[nEnd - nStart] = 0;

	return nStart;
}

////////////////////////////////////////////////////////////////////////////
//函数名成：SplitLastFolder
//函数功能：拆分路径中的最后一个目录
//参数说明：pszSrc--原路径
//        pszSplit1--拆分的前半
//        pszSplit1--拆分的后半
//返 回 值：成功返回true，失败返回false
//备   注：
////////////////////////////////////////////////////////////////////////////
bool splitLastFolder(const char* pszSrc, char* pszSplit1, char* pszSplit2)
{
	int  nLen = 0;
	int  nPos = 0;
	bool bSplit = false;

	if (NULL == pszSrc || NULL == pszSplit1 || NULL == pszSplit2)
	{
		return false;
	}
	nLen = strlen(pszSrc);
	for (nPos = nLen; nPos > 0; nPos--)
	{
		if ('/' == pszSrc[nPos])
		{
			bSplit = true;
			break;
		}
	}
	if (!bSplit)
	{
		return false;
	}
	memcpy(pszSplit1, pszSrc, nPos + 1);
	strcpy(pszSplit2, &pszSrc[nPos + 1]);

	return true;
}

/**
 * @brief 追加字符串
 * @param pszSrc--源串
 * @param nSizeOfMax--源串最大长度
 * @param pszSplit--分割符
 * @param pszSub--子串
 * @return true：成功，false：失败
 */
bool appendString(char* pszSrc, int nSizeOfMax, char* pszSplit, char* pszSub)
{
    if (NULL == pszSrc || NULL == pszSplit || NULL == pszSub || strlen(pszSub) <= 0 || nSizeOfMax <= 0)
    {
        return false;
    }
    if (strlen(pszSrc) + strlen(pszSub) + strlen(pszSplit) >= nSizeOfMax)
    {
        return false;
    }
    if (strlen(pszSrc) > 0)
    {
        strcat(pszSrc, pszSplit);
        strcat(pszSrc, pszSub);
    }
    else
    {
        strcpy(pszSrc, pszSub);
    }

    return true;
}

/**
 * @brief 删除子串
 * @param pszSrc--源串
 * @param nSizeOfSrc--源串长度
 * @param pszSub--子串
 * @return true：成功，false：失败
 */
bool deleteSub(char* pszSrc, int nSizeOfSrc, char* pszSub)
{
    char szBuf[4096] = { 0 };

    if (nSizeOfSrc >= sizeof(szBuf))
    {
        return false;
    }
    int i = 0;
    int n = 0;
    int nLen = strlen(pszSub);
    while (i < nSizeOfSrc)
    {
        if (pszSrc[i] == pszSub[0])
        {
            if (0 == memcmp(&pszSrc[i] , pszSub, nLen))
            {
                i += nLen;
                continue;
            }
        }
        szBuf[n] = pszSrc[i] ;
        i++;
        n++;
    }
    memset(pszSrc, 0x00, nSizeOfSrc);
    strcpy(pszSrc, szBuf);

    return true;
}
