/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wireless.h
 * Author: dyer
 *
 * Created on 2017年2月4日, 下午12:24
 */

#ifndef WIRELESSBASE_H
#define WIRELESSBASE_H

#include <stdint.h>

#define WIRELEE_BRIDGE_OUTTIME     120000        //120s
#define WIRELEE_OUTTIME                 60000        //60s
#define WIRELEE_OUTTIME_FREE        120000        //300s
#define WIRELEE_FREE_WAIT_TIME    10000          //10s

#pragma pack(push, 1)

typedef struct __tag_Wireless_Device {
    uint8_t             unIdentity;              //1:AP, 2:station,3:bridge，4：bridge下的station，5：bridge下的AP，6：bridge未知，7：未知    
    int                    nFree;                   //释放标志，1：需要释放，0：不需要释放
    uint64_t            unLasttime;           //最后活动时间
    uint8_t             unOnline;                //1：在线，0：离线
    uint8_t             zMac[6];                //无线设备MAC地址        
    int8_t              nLastChannel;      //无线设备最后捕获信道
    int8_t               nLastSignal;          //最后捕获到的信号值
}WIRELESS_DEVICE, *PWIRELESS_DEVICE;

typedef struct __tag_Wireless_AP {
    WIRELESS_DEVICE   wireless;
    int8_t              nConfirm;           //身份参数确认标志，1：已确认，0：未确认
    uint8_t            unBridge;               //1:是bridge，0：不是是bridge
    uint64_t          unLasttimeOfBridge;//桥数据最后确认时间
    int8_t              nPrivacy;              //保护方式 -1：未确认，0x00:不加密,0x01:WEP,0x02:WPA,0x03:WEP|WPA,0x04:WPA2,0x05:WEP|WPA2,0x06:WPA|WPA2,0x07:WEP|WPA|WPA2
    int8_t              nEncrypt;             //加密算法 -1：未确认，0:不加密,1:WEP40,2:WEP104,3:WEP40|WEP104,4:TKIP,8:CCMP,12:TKIP|CCMP
    int8_t              nAuth;                //认证方式 -1：未确认，0:不认证,1:PSK,2:RADIUS
    int8_t              nWPS;                 //-1：未确认，1：开启WPS，0：不开启WPS
    int8_t              nChannel;             //无线设备实际信道，-1：未确认
    int8_t              nHideSSID;           //隐藏SSID,1:隐藏，0：不隐藏，-1：未确认
    char                 szSSID[66];          //SSID    
}WIRELESS_AP, *PWIRELESS_AP;

typedef struct __tag_Wireless_Station {
    WIRELESS_DEVICE   wireless;
    int8_t              nConnected;        //连接AP标志,1:连接，0：未连接
    int8_t              nSet;                  //1:连接状态被AP设置
    int8_t              nChannel;            //无线设备实际信道,-1：未确认
    uint8_t            zMacOfAP[6];         //无线设备连接AP的MAC地址 
}WIRELESS_STATION, *PWIRELESS_STATION;

#pragma pack(pop)

#endif /* WIRELESSBASE_H */

