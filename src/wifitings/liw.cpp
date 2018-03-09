/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CLiw.cpp
 * Author: dyer
 * 
 * Created on 2016年6月24日, 上午10:09
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "../../include/DbgPrint.h"
#include "liw.h"

CLiw::CLiw() {
    m_bInited = false;
    m_nlState.nl80211_id = -1;
    m_nlState.nl_sock = NULL;
}

CLiw::~CLiw() {
    cleanup();
}

/**
 * 初始化
 * @return true：成功，false：失败
 */
 bool CLiw::init() {

     if (m_bInited) {
         return true;
     }
     //申请nl80211 socket
     m_nlState.nl_sock =  nl_handle_alloc();
     if (!m_nlState.nl_sock) {
         TmpDebugPrint("Failed to allocate netlink socket.\r\n");
         return false;
     }

    if (genl_connect((struct nl_handle *)m_nlState.nl_sock)) {
        TmpDebugPrint("Failed to connect to generic netlink.\r\n");
        nl_handle_destroy(m_nlState.nl_sock);
        m_nlState.nl_sock = NULL;
        m_nlState.nl80211_id = -1;
        return false;
    }
     
     nl_set_buffer_size(m_nlState.nl_sock, 8192, 8192);
     m_nlState.nl80211_id = genl_ctrl_resolve(
             m_nlState.nl_sock
             , "nl80211");
     if (m_nlState.nl80211_id < 0) {
         TmpDebugPrint("nl80211 not found.\r\n");
         nl_handle_destroy(m_nlState.nl_sock);
         m_nlState.nl_sock = NULL;
         return false;
    }
     m_bInited = true;

    return true;
 }
 
 /**
 * 清理
 */
void CLiw::cleanup() {
    if (NULL == m_nlState.nl_sock) {
        nl_handle_destroy(m_nlState.nl_sock);
    }
    m_nlState.nl_sock = NULL;
    m_nlState.nl80211_id = -1;
}

/**
 * 设置信道
 * @param pszDevice--网卡名，如wlan0
 * @param nChannle--信道
 * @return true：成功，false：失败
 */
bool CLiw::setChannel(const char* pszDevice,  int nChannle) {
    
    int  nErr;
    struct nl_cb* cb;
    struct nl_cb* s_cb;
    struct nl_msg* nlmsg;
    signed long long devidx = 0;
    
    if (NULL == pszDevice) {
        TmpDebugPrint("setChannel: param error.\r\n");
        return false;
    }
    
    if (NULL == m_nlState.nl_sock) {
        TmpDebugPrint("setChannel: not inited.\r\n");
        return false;
    }

    devidx = if_nametoindex(pszDevice);
    if (0 == devidx) {
        TmpDebugPrint("setChannel: Failed to if_nametoindex.\r\n");
        return false;
    }
    nlmsg = nlmsg_alloc();
    if (!nlmsg) {
        TmpDebugPrint("setChannel: Failed to nlmsg_alloc.\r\n");
        return false;
    }
    cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (!cb ) {
        nlmsg_free(nlmsg);
        TmpDebugPrint("setChannel: Failed to nl_cb_alloc 1.\r\n");
        return false;
    }
    s_cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (!s_cb) {
        nl_cb_put(cb);
        nlmsg_free(nlmsg);
        TmpDebugPrint("setChannel: Failed to nl_cb_alloc 2.\r\n");
        return false;
    }
    //genlmsg_put(nlmsg, 0, 0, nlstate.nl80211_id, 0, 0, NL80211_CMD_SET_CHANNEL, 0);
    genlmsg_put(nlmsg, 0, 0, m_nlState.nl80211_id, 0, 0, NL80211_CMD_SET_WIPHY, 0);
    NLA_PUT_U32(nlmsg, NL80211_ATTR_IFINDEX, devidx);
        
    enum nl80211_band nlBand;
    unsigned int nfreq;
    nfreq = (unsigned int)nChannle;
    nlBand = nfreq <= 14 ? NL80211_BAND_2GHZ : NL80211_BAND_5GHZ;
    nfreq = ieee80211_channel_to_frequency(nfreq, nlBand);
    NLA_PUT_U32(nlmsg, NL80211_ATTR_WIPHY_FREQ, nfreq);
    
    nl_socket_set_cb(m_nlState.nl_sock, s_cb);

    nErr = nl_send_auto_complete(m_nlState.nl_sock, nlmsg);
    if (nErr < 0) {
        nl_cb_put(cb);
        nl_cb_put(s_cb);
        nlmsg_free(nlmsg);
        TmpDebugPrint("setChannel: Failed to nl_send_auto_complete.\r\n");
        return false;
    }

    nErr = 1;
    
    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &nErr);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &nErr);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &nErr);
    
    while (nErr > 0) {
        nl_recvmsgs(m_nlState.nl_sock, cb);
    }
    
    nl_cb_put(cb);
    nl_cb_put(s_cb);
    nlmsg_free(nlmsg);
    
    if (0 == nErr) {
        TmpDebugPrint("setChannel: success.\n");
        return true;
    } else {
        TmpDebugPrint("setChannel: falure.\n");
        return false;
    } 
    
 nla_put_failure:
    nl_cb_put(cb);
    nl_cb_put(s_cb);
    nlmsg_free(nlmsg);
    TmpDebugPrint("setChannel: building message failed.\n");
    return false;
}

/**
 * 错误处理函数
 * @param nla
 * @param err
 * @param arg
 * @return 
 */
int CLiw::error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) {
    int *ret =  (int*)arg;
    *ret = err->error;
    return NL_STOP;
}

/**
 * 完成处理函数
 * @param msg
 * @param arg
 * @return 
 */
int CLiw::finish_handler(struct nl_msg *msg, void *arg) {
    int *ret =  (int*)arg;
    *ret = 0;
    return NL_SKIP;
}

/**
 * 应答处理函数
 * @param msg
 * @param arg
 * @return 
 */
int CLiw::ack_handler(struct nl_msg *msg, void *arg) {
    int *ret = (int*)arg;
    *ret = 0;
    return NL_STOP;
}

/**
 * 十进制转80211
 * @param chan
 * @param band
 * @return 
 */
int CLiw::ieee80211_channel_to_frequency(int chan, enum nl80211_band band) {
    /* see 802.11 17.3.8.3.2 and Annex J
     * there are overlapping channel numbers in 5GHz and 2GHz bands */
    if (chan <= 0)
            return 0; /* not supported */
    switch (band) {
    case NL80211_BAND_2GHZ:
            if (chan == 14)
                    return 2484;
            else if (chan < 14)
                    return 2407 + chan * 5;
            break;
    case NL80211_BAND_5GHZ:
            if (chan >= 182 && chan <= 196)
                    return 4000 + chan * 5;
            else
                    return 5000 + chan * 5;
            break;
    case NL80211_BAND_60GHZ:
            if (chan < 5)
                    return 56160 + chan * 2160;
            break;
    default:
            ;
    }
    return 0; /* not supported */
}
