#include "timer_wheel.h"

TimerWheel::TimerWheel()
{
    _Init();
}

TimerWheel::~TimerWheel()
{
    _UnInit();
}

HTIMER TimerWheel::AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile, int32_t nLine)
{
    CTimerNode* pNode = _GetFreeNode();
    if (!pNode)
    {
        return INVALIDE_HTIMER;
    }
    pNode->m_oDelegate = delegate;
    pNode->m_pDelegateArgs = pDelegateArgs;
    pNode->m_pArgs = pArgs;
    pNode->m_nInterval = nInterval;
    pNode->m_nTotalCount = nCount;
    pNode->m_szFile = szFile;
    pNode->m_nLine = nLine;
    // 最大跨度超过了2^32 -1
    if (pNode->m_nInterval > 0xffffffffUL)
    {
        pNode->m_nInterval = 0xffffffffUL;
    }
    pNode->m_nExpireTime = m_nCurTime + pNode->m_nInterval;
    pNode->m_uGuid = _GetNewTimerID();

    _AddTimer(pNode);
    
    return pNode->m_uGuid;
}

void TimerWheel::KillTimer(HTIMER uTimer)
{
    auto it = m_oAllTimers.find(uTimer);
    if (it == m_oAllTimers.end())
    {
        return;
    }
    auto* pNode = it->second;
    if (!pNode)
    {
        return;
    }
    ListRemove(pNode);
    m_oAllTimers.erase(it);
    _AddFreeNode(pNode);
}

void TimerWheel::Update(int32_t nDelta)
{
    int64_t now_time = m_nCurTime + nDelta;
    while (m_nCurTime < now_time)
    {
        int32_t nIdx = m_nCurTime & TVR_MASK;
        // 当前时间低8位如果为0, 则把外层时间轮上的时间节点向内层转移
        if (!nIdx
            && !_CascadeTime(OFFSET(0), INDEX(m_nCurTime, 0))
            && !_CascadeTime(OFFSET(1), INDEX(m_nCurTime, 1))
            && !_CascadeTime(OFFSET(2), INDEX(m_nCurTime, 2)))
        {
            _CascadeTime(OFFSET(3), INDEX(m_nCurTime, 3));
        }

        auto* pNode = &m_oTimerNodes[nIdx];
        while (pNode->m_pNext != pNode)
        {
            auto* pNext = pNode->m_pNext;
            ++pNext->m_nCurCount;
            pNext->m_oDelegate(pNext->m_pDelegateArgs, pNext->m_pArgs);
            ListRemove(pNext);
            if (pNext->m_nTotalCount == -1 || pNext->m_nCurCount < pNext->m_nTotalCount)
            {
                pNext->m_nExpireTime = m_nCurTime + pNext->m_nInterval;
                _AddTimer(pNext);
            }
            else
            {
                m_oAllTimers.erase(pNext->m_uGuid);
                _AddFreeNode(pNext);
            }
        }

        m_nCurTime++;
    }
}

void TimerWheel::Release()
{
    _UnInit();
}

HTIMER TimerWheel::_GetNewTimerID()
{
    return ++m_uNextID;
}

void TimerWheel::_Init()
{
    for (auto& i : m_oTimerNodes)
    {
        CTimerNode* pHead = &i;
        pHead->m_pNext = pHead;
        pHead->m_pPrev = pHead;
    }
}

void TimerWheel::_UnInit()
{
    for (auto& i : m_oTimerNodes)
    {
        CTimerNode* pNode = &i;
        while (pNode->m_pNext != pNode)
        {
            CTimerNode* pNodeToDel = pNode->m_pNext;
            ListRemove(pNode);
            _AddFreeNode(pNode);
        }
    }

    for (auto& p : m_oFreeNodes)
    {
        delete p;
        p = nullptr;
    }

    m_oAllTimers.clear();
    m_oFreeNodes.clear();
}

CTimerNode* TimerWheel::_GetFreeNode()
{
    if (m_oFreeNodes.empty())
    {
        return new CTimerNode();
    }

    auto* pNode = m_oFreeNodes.front();
    m_oFreeNodes.pop_front();
    return pNode;
}

void TimerWheel::_AddFreeNode(CTimerNode* pNode)
{
    if (!pNode)
    {
        return;
    }
    pNode->Reset();
    m_oFreeNodes.emplace_back(pNode);
}

void TimerWheel::_AddTimer(CTimerNode* pNode)
{
    if (!pNode)
    {
        return;
    }
    int32_t nSlotIdx = 0;
    int64_t nDelay = pNode->m_nExpireTime - m_nCurTime;
    if (nDelay < TVR_SIZE)
    {
        // 第一层的索引为低8位的值
        nSlotIdx = pNode->m_nExpireTime & TVR_MASK;
    }
    else if (nDelay < (1 << (TVR_BITS + TVN_BITS)))
    {
        // 第二层索引为偏移量+9-14位值
        nSlotIdx = OFFSET(0) + INDEX(pNode->m_nExpireTime, 0);
    }
    else if (nDelay < (1 << (TVR_BITS + 2 * TVN_BITS)))
    {
        // 第三层的索引为偏移量+15-20位值
        nSlotIdx = OFFSET(1) + INDEX(pNode->m_nExpireTime, 1);
    }
    else if (nDelay < (1 << (TVR_BITS + 3 * TVN_BITS)))
    {
        // 第四层的索引为偏移量+21-26位值
        nSlotIdx = OFFSET(2) + INDEX(pNode->m_nExpireTime, 2);
    }
    else if (nDelay < 0)
    {
        // 已经超时的定时器放入最内层时间轮的随机位置
        nSlotIdx = m_nCurTime & TVR_MASK;
    }
    else
    {
        // 最外层的索引为偏移量+27-32位值
        nSlotIdx = OFFSET(3) + INDEX(pNode->m_nExpireTime, 3);
    }

    auto* pHead = &m_oTimerNodes[nSlotIdx];
    if (m_oAllTimers.emplace(pNode->m_uGuid, pNode).second)
    {
        ListAdd(pHead, pNode);
    }
}

int32_t TimerWheel::_CascadeTime(int32_t nOff, int32_t nIndex)
{
    int32_t nSlotIdx = nOff + nIndex;
    auto* pNode = &m_oTimerNodes[nSlotIdx];
    while (pNode->m_pNext != pNode)
    {
        auto* pNext = pNode->m_pNext;
        ListRemove(pNext);
        _AddTimer(pNext);
    }
    return nIndex;
}
