/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <string.h>
#include<sys/time.h>
#include "time_tools.h"

/**
 * 得到当前时间，单位毫秒
 * @return 0:失败
 */
uint64_t getNowMillisecond() {
    
    uint64_t          itNow = 0;
    struct timeval    tNow = { 0 };
    if (0 != gettimeofday(&tNow, NULL)) {
        return 0;
    }
    
    itNow = (tNow.tv_sec * 1000) + (tNow.tv_usec / 1000);
    
    return itNow;
}
