/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CLiw.h
 * Author: dyer
 *
 * Created on 2016年6月24日, 上午10:09
 */

#ifndef CLIW_H
#define CLIW_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/if_ether.h>
#include "../netlink/genl/genl.h"
#include "../netlink/genl/family.h"
#include "../netlink/genl/ctrl.h"
#include "../netlink/msg.h"
#include "../netlink/attr.h"
#include "../netlink/socket.h"
#include "nl80211.h"

struct nl80211_state {
    struct nl_handle* nl_sock;
    int nl80211_id;
};

class CLiw {
    CLiw(const CLiw& orig);
public:
    CLiw();
    ~CLiw();
    
    //初始化
    bool init();
    
    //设置信道
    bool setChannel(const char* pszDevices,  int nChannle);
    
protected:
    //清理
    void cleanup();
        
    static int error_handler(sockaddr_nl *nla, struct nlmsgerr *err,  void *arg);

    //完成处理函数
    static int finish_handler(nl_msg *msg, void *arg);

    //应答处理函数
    static int ack_handler(nl_msg *msg, void *arg);
    
    //十进制转80211
    int ieee80211_channel_to_frequency(int chan, enum nl80211_band band);
        
private:
    struct nl80211_state      m_nlState;
    bool                            m_bInited;
};

#endif /* CLIW_H */

