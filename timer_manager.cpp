#include "timer_manager.h"

void CTimerManager::_Init()
{
    m_pTimer = new TimerWheel();
}

void CTimerManager::_UnInit()
{
    if (m_pTimer)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

HTIMER CTimerManager::AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile /*= ""*/, int32_t uLine /*= 0*/)
{
    return m_pTimer->AddTimer(delegate, pDelegateArgs, pArgs, nInterval, nCount, szFile, uLine);
}

void CTimerManager::KillTimer(HTIMER uTimer)
{
    m_pTimer->KillTimer(uTimer);
}

void CTimerManager::KillAllTimer()
{
    m_pTimer->Release();
}

void CTimerManager::Update(int32_t nDelta)
{
    m_pTimer->Update(nDelta);
}

