/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   llargekeymap.cpp
 * Author: dyer
 * 
 * Created on 2017年2月8日, 下午8:53
 */

#include "llargekeymap.h"

CLLargeKeyMap::CLLargeKeyMap() {
}

CLLargeKeyMap::~CLLargeKeyMap() {
}

/**
 * 向map中插入一个值
 * @param nKey--键
 * @param pValue--值
 * @return true：成功，false：失败
 */
bool CLLargeKeyMap::insertObject(uint64_t nKey, void* pValue) {

    std::pair<mapObject::iterator, bool>   pairRet;

    if (NULL == pValue || 0 == nKey) {
        return false;
    }
    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex); 
        pairRet = m_mapObject.insert(mapObject::value_type(nKey, pValue));
        if (pairRet.second) {
            return true;
        }
    } catch (...) {
       
    }

    return false;
}

/**
 * 在map中删除指定值
 * @param nKey--键
 * @return NULL：失败，成功返回已出的值
 */
void* CLLargeKeyMap::removeObject(uint64_t nKey) {
    
    mapObject::iterator iterRemove;
    void*                    pValue;

    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex);        
        if (m_mapObject.size() <= 0) {
            return NULL;
        }

        iterRemove = m_mapObject.find(nKey);
        if (iterRemove == m_mapObject.end()) {
            return NULL;
        }
        pValue = (void*)iterRemove->second;
        m_mapObject.erase(iterRemove);

        return pValue;
    }
    catch (...) {
    }

    return NULL;
}

/**
 * 在map中删除第一个指定值
 * @param pValue--键
 * @return NULL：失败，成功返回已出的值
 */
void* CLLargeKeyMap::removeObject(void* pValue) {
    
    mapObject::iterator iterRemove;
    void*                    pRemove;
    
    if (NULL == pValue) {
        return NULL;
    }
    
    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex); 
        if (m_mapObject.size() <= 0) {
            return NULL;
        }
        
        for (iterRemove = m_mapObject.begin(); iterRemove != m_mapObject.end(); iterRemove++) {
            pRemove = (void*)iterRemove->second;
            if (pRemove == pValue)
            {
                m_mapObject.erase(iterRemove);
                return pRemove;
            }
        }
    }
    catch (...) {
    }

    return NULL;
}

/**
 * 清空map
 */
void CLLargeKeyMap::clear() {

    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex);      
        m_mapObject.clear();
    } catch (...) {
    }
}

/**
 * 清空map
 */
void CLLargeKeyMap::clearfree() {
    
    mapObject::iterator iterRemove;
    void*                    pDelete;
        
    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex);
        if (m_mapObject.size() <= 0) {
            return;
        }
        
        for (iterRemove = m_mapObject.begin(); iterRemove != m_mapObject.end(); iterRemove++) {
            pDelete = (void*)iterRemove->second;
            if (NULL != pDelete) {
                free (pDelete);
            }
        }
        m_mapObject.clear();        
    }
    catch (...) {
    }
}

/**
 * 得到map长度
 * @return -1:失败
 */
int CLLargeKeyMap::size() {
    int nSize = 0;
    try {
        boost::shared_lock<boost::shared_mutex> readLock(m_Mutex);     
        nSize = (int)m_mapObject.size();
    } catch (...) {
        return -1;
    }
    
    return nSize;
}

/**
 * 在map中查找指定值
 * @param nKey--键
 * @return NULL:失败
 */
void* CLLargeKeyMap::findObject(uint64_t nKey) {
    
    mapObject::iterator      iterFind;
    void*                         pValue;

    try {
        boost::shared_lock<boost::shared_mutex> readLock(m_Mutex);           
        if (m_mapObject.size() <= 0) {
            return NULL;
        }

        iterFind = m_mapObject.find(nKey);
        if (iterFind == m_mapObject.end()) {
            return NULL;
        }

        pValue = (void*)iterFind->second;          

        return pValue;
    }
    catch (...) {
    }
    
    return NULL;
}

/**
 * 从map弹出一个对象
 * @return NULL:失败或map已经为空
 */
void* CLLargeKeyMap::pop() {
    
    mapObject::iterator      iterPop;
    void*                         pValue;

    try {
        boost::unique_lock<boost::shared_mutex> writeLock(m_Mutex);
        if (m_mapObject.size() <= 0) {
            return NULL;
        }

        iterPop = m_mapObject.begin();
        if (iterPop == m_mapObject.end()) {
            return NULL;
        }
        pValue = (void*)iterPop->second;
        m_mapObject.erase(iterPop);
        if (NULL == pValue) {
            return NULL;
        } 
        
        return pValue;   
    } catch (...) {
    }
    
    return NULL;
}

/**
 * 复制map
 * @param mapCopy
 * @return true：成功，false：失败
 */
bool CLLargeKeyMap::copyMap(mapObject& mapCopy) {
    
    mapObject::iterator iterItem;

    try {
        boost::shared_lock<boost::shared_mutex> readLock(m_Mutex);    
        if (m_mapObject.size() <= 0) {
            return false;
        }
        for (iterItem = m_mapObject.begin(); iterItem != m_mapObject.end(); iterItem++) {			
            mapCopy.insert(mapObject::value_type(iterItem->first, iterItem->second));
        }
        return true;
    } catch (...) {
    }

    return false;
}

/**
 * 遍历map
 * @param user
 * @param pforeach
 */
void CLLargeKeyMap::foreach(void* pUuser, PFOREACHCALLBACK pForeach) {
    
    mapObject::iterator iterItem;
    
    if (NULL == pForeach) {
        return;
    }
    mapObject mapCopy;
    if (!copyMap(mapCopy)) {
        return;
    }
    for (iterItem = mapCopy.begin(); iterItem != mapCopy.end(); iterItem++) {
        pForeach(pUuser,  iterItem->first,(void*)iterItem->second);
    }
}