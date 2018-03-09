/*
 * DbgPrint.h
 *
 *  Created on: 2015年2月26日
 *      Author: root
 */

#ifndef DBGPRINT_H_
#define DBGPRINT_H_



#define __DEBUG_PRINT_
#ifdef __DEBUG_PRINT_
#define DebugPrint(format,args...)  printf(format, ##args)
#else
#define DebugPrint(format,args...)
#endif

#define __DEBUG_PRINT_01
#ifdef __DEBUG_PRINT_01
#define DebugPrint01(format,args...)  printf(format, ##args)
#else
#define DebugPrint01(format,args...)
#endif

//#define __DEBUG_PRINT_02
#ifdef __DEBUG_PRINT_02
#define DebugPrint02(format,args...)  printf(format, ##args)
#else
#define DebugPrint02(format,args...)
#endif

//#define __DEBUG_PRINT_03
#ifdef __DEBUG_PRINT_03
#define DebugPrint03(format,args...)  printf(format, ##args)
#else
#define DebugPrint03(format,args...)
#endif

//#define __DEBUG_PRINT_04
#ifdef __DEBUG_PRINT_04
#define DebugPrint04(format,args...)  printf(format, ##args)
#else
#define DebugPrint04(format,args...)
#endif

//#define __DEBUG_PRINT_DEUBGGING
#ifdef __DEBUG_PRINT_DEUBGGING
#define DebugPrintDebugging(format,args...)  printf(format, ##args)
#else
#define DebugPrintDebugging(format,args...)
#endif

//#define __TMP_DEBUG_PRINT_
#ifdef __TMP_DEBUG_PRINT_
#define TmpDebugPrint(format,args...)  printf(format, ##args)
#else
#define TmpDebugPrint(format,args...)
#endif


#endif /* DBGPRINT_H_ */
