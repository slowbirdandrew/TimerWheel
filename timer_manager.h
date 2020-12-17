#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include "timer_wheel.h"

class CTimerManager
{
public:
    static CTimerManager& Instance()
    {
        static CTimerManager* pInstance = new CTimerManager();
        return *pInstance;
    }

private:
    ~CTimerManager() {};

    /**
     * @brief 初始化定时器
     * @return void
     */
    void _Init();
    /**
     * @brief 逆初始化定时器
     * @return void
     */
    void _UnInit();

public:
    /**
     * @brief 添加一个定时器任务
     * @param delegate 定时任务执行对象
     * @param pDelegateArgs 定时任务参数
     * @param pArgs 额外参数
     * @param nInterval 执行间隔
     * @param nCount 执行次数
     * @param szFile 定时器添加的文件
     * @param uLine 定时器添加的代码行
     * @return HTIMER 定时器句柄
     */
    HTIMER AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile = "", int32_t uLine = 0);
    /**
     * @brief 删除一个定时器任务
     * @param uTimer 定时器句柄
     * @return void
     */
    void KillTimer(HTIMER uTimer);
    /**
     * @brief 删除所有定时器
     * @return void
     */
    void KillAllTimer();
    /**
     * @brief 定时器轮询
     * @param nDelta
     * @return void
     */
    void Update(int32_t nDelta);

private:
    TimerWheel* m_pTimer{ nullptr };
};

#endif // !__TIMER_MANAGER_H__
