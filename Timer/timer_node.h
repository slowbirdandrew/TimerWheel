#ifndef __TIMER_NODE_H__
#define __TIMER_NODE_H__

#include <time.h>
#include <stdint.h>
#include "delegate.hpp"

typedef uint64_t HTIMER;
#define INVALIDE_HTIMER 0

struct CTimerNode
{
    CTimerNode* m_pPrev{ nullptr };
    CTimerNode* m_pNext{ nullptr };
    
    CDelegate m_oDelegate;
    CArgs* m_pDelegateArgs{ nullptr };
    void* m_pArgs{ nullptr };
    int64_t m_nExpireTime{ 0 };
    HTIMER m_uGuid{ 0 };
    int32_t m_nInterval{ 0 };
    int32_t m_nTotalCount{ 0 };
    int32_t m_nCurCount{ 0 };

    const char* m_szFile{ "" };
    int32_t m_nLine{ 0 };

    ~CTimerNode()
    {
        Reset();
    }

    void Reset()
    {
        if (m_pDelegateArgs)
        {
            delete m_pDelegateArgs;
            m_pDelegateArgs = nullptr;
        }
        m_pPrev = nullptr;
        m_pNext = nullptr;
        m_pArgs = nullptr;
        m_uGuid = 0;
        m_nInterval = 0;
        m_nTotalCount = 0;
        m_nCurCount = 0;
        m_szFile = "";
        m_nLine = 0;
        m_nExpireTime = 0;
    }
};

/**
 * @brief 初始化链表
 * @param pNode 哨兵节点
 * @return void
 */
inline void ListInit(CTimerNode* pNode)
{
    if (!pNode) return;
    pNode->m_pPrev = nullptr;
    pNode->m_pNext = nullptr;
}

/**
 * @brief 添加节点
 * @param pCurNode 添加的节点位置
 * @param pNewNode 待添加的节点
 * @return void
 */
inline void ListAdd(CTimerNode* pCurNode, CTimerNode* pNewNode)
{
    if (!pCurNode || !pNewNode) return;
    pNewNode->m_pNext = pCurNode->m_pNext;
    pNewNode->m_pPrev = pCurNode;
    pCurNode->m_pNext->m_pPrev = pNewNode;
    pCurNode->m_pNext = pNewNode;
}

/**
 * @brief 删除节点
 * @param pNode
 * @return void
 */
inline void ListRemove(CTimerNode* pNode)
{
    if (!pNode) return;
    pNode->m_pNext->m_pPrev = pNode->m_pPrev;
    pNode->m_pPrev->m_pNext = pNode->m_pNext;
}

#endif
