
#ifndef __STRINGPRT_H__
#define __STRINGPRT_H__

//查找子串
int strpos(const char *pszSrc, int nBytesOfSrc, const char *pszSub);

//去除开头和结尾的空格与换行回车符
bool trimspace(const char* pszSrc, char* pszDest, int nBytesOfDest);

//提取文件扩展名
bool extractSuffix(const char* pszFileName, char* pszSuffix, int nBytesOfSuffix);

//字符串大写
bool strupcase (char* pszStr);

//字符串小写
bool strlower (char* pszStr);

//获取系统当前时间
bool getnow(char* pszNow, int nBytesOfNow);

//判断字符串是否是数字
bool isdigit(char* pszSrc);

//得到第一个单词
int getword(const char* pszSrc, char* pszWord, int nBytesOfWord);

//拆分路径中的最后一个目录
bool splitLastFolder(const char* pszSrc, char* pszSplit1, char* pszSplit2);

//追加字符串
bool appendString(char* pszSrc, int nSizeOfMax,char* pszSplit, char* pszSub);

//删除子串
bool deleteSub(char* pszSrc, int nSizeOfSrc, char* pszSub);

#endif
