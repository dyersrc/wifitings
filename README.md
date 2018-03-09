  通过libpcap直接抓取无线网卡上采集到的数据，对ieee80211协议进行解析，实现周边WIFI信号环境还原，能够扫描出
周边所有的WIFI网卡的MAC地址，以及终端与各个AP之间的连接关系。模块之中自行维护一张实时映射表，当采集到MAC信
息后会将MAC的信号、信道、身份（AP或终端）、连接AP的MAC等信息通过接口吐出。支持2.4G和5.8G，同时最大支持8块
网卡。

依赖库：

libpcap 
libnl 
boost 

编译：

make #编译debug版本 

make all #编译debug和release版本

使用：

  需要导入头文件./src/include/wifitings.h和./src/include/wifitingsinterface.h

  
    CWifiTings* pWifiTings = getWifiTingsInstance();

    if (NULL = pWifiTings) {
        return;
    }

    if (!pWifiTings->setAdapterName(pszAdapterName) || 
            !pWifiTings->setRootDir(m_szRootDir)) {
        releaseWifiTingsInstance(pAdapterItem->pWifiTings);
        return;
    }

    if (2 == nType) {//2.4G网卡
        pWifiTings->setFrequencyChanne(2);
    } else if (5 == nType) {//5.8G网卡
        pWifiTings->setFrequencyChanne(5);
    } else {
        releaseWifiTingsInstance(pWifiTings);
        return;
    }

    //设置信息回调函数
    pWifiTings->registeFrameCallback((void*)this, frameCallback);  
  
    //设置网卡自定义绑定ID号，上吐数据时一并吐出
    pWifiTings->setAdapterId(nAdapterId);

    //2.4G网卡设置需要扫描的信道，可以在后期实时设置
    int nLenOfList = 13;
    int8_t lstChannel[13] = {
        (int8_t)1, (int8_t)2, (int8_t)3, (int8_t)4, (int8_t)5, (int8_t)6, (int8_t)7, (int8_t)8,
        (int8_t)9, (int8_t)10, (int8_t)11, (int8_t)12, (int8_t)13
    };
    pWifiTings->setChannel(lstChannel, nLenOfList);
    
    //5.8G网卡设置需要扫描的信道，可以在后期实时设
    //中国支持的信道 149,153,157,161,165
    //int nLenOfList = 16;
    //int8_t lstChannel[16] = {
    //    (int8_t)36, (int8_t)38, (int8_t)40, (int8_t)42, (int8_t)44, (int8_t)46, (int8_t)48, (int8_t)52,
    //    (int8_t)56, (int8_t)60, (int8_t)64, (int8_t)149, (int8_t)153, (int8_t)157, (int8_t)161, (int8_t)165
    //};
    //pWifiTings->setChannel(lstChannel, nLenOfList);
    
    
    //启动
    pWifiTings->start()
    
    //启动下线监听器，当MAC离线时主动吐出离线信息
    registeOfflineCallback((void*)this, frameCallback);
    if (!startOffineMonitor()) {
        return;
    }
  
    //pWifiTings->stop()

    //releaseWifiTingsInstance(pWifiTings)


