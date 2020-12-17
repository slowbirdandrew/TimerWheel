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

HTIMER CTimerManager::AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile /*= ""*/, int32_t nLine /*= 0*/)
{
    return m_pTimer->AddTimer(delegate, pDelegateArgs, pArgs, nInterval, nCount, szFile, nLine);
}

HTIMER CTimerManager::AddTimer(const std::function<bool(CArgs*, void*)>& callback, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile /*= ""*/, int32_t nLine /*= 0*/)
{
    if (!callback)
    {
        return INVALIDE_HTIMER;
    }
    auto* pDelegateMethod = new DelegateMethod;
    pDelegateMethod->m_pFunc = callback;
    pDelegateMethod->m_pData = pArgs;
    return AddTimer(DELEGATE_COMBINATION(CTimerManager, ExecuteDelegateMethod, this), nullptr, reinterpret_cast<void*>(pDelegateMethod), nInterval, nCount, szFile, nLine);
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

bool CTimerManager::ExecuteDelegateMethod(CArgs* pArgs, void* pData)
{
    if (!pData)
    {
        return false;
    }
    auto* pDelegateMethod = reinterpret_cast<DelegateMethod*>(pData);
    bool bRes = pDelegateMethod->m_pFunc(pArgs, pData);
    delete pDelegateMethod;
    return bRes;
}

