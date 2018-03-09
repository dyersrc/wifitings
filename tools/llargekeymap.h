/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   llargekeymap.h
 * Author: dyer
 *
 * Created on 2017年2月8日, 下午8:53
 */

#ifndef LLARGEKEYMAP_H
#define LLARGEKEYMAP_H

#include <stdint.h>
#include <boost/container/map.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

using namespace boost;

typedef boost::container::map<uint64_t, void*> mapObject;

//遍历回调，第一个参数，调用者设置，回调时回传，第二个参数，存储在map中的对象
typedef void (*PFOREACHCALLBACK)(void*, uint64_t, void*);

class CLLargeKeyMap {
    CLLargeKeyMap(const CLLargeKeyMap& orig);
public:
    CLLargeKeyMap();    
    virtual ~CLLargeKeyMap();
    
    //向map中插入一个值
    bool insertObject(uint64_t nKey, void* pValue);

    //在map中删除指定值
    void* removeObject(uint64_t nKey);
    
    //在map中删除第一个指定值
    void* removeObject(void* pValue);	

    //清空map
    void clear();
    
    //清空map
    void clearfree();
    
//    //遍历map
//    void foreach(void* user, PFOREACHCALLBACK pforeach);
    
    //得到map长度
    int size();
    
    //在map中查找指定值
    void* findObject(uint64_t nKey);
        
    //从map弹出一个对象
    void* pop();
    
    //遍历map
    void foreach(void* pUser, PFOREACHCALLBACK pForeach);
    
    //复制map
    bool copyMap(mapObject& mapCopy);
    
protected:
    mapObject                m_mapObject;       
    boost::shared_mutex m_Mutex;  
//    typedef boost::unique_lock<WR_Mutex>   writeLock;  
//    typedef boost::shared_lock<WR_Mutex>   readLock;  
};

#endif /* LLARGEKEYMAP_H */

