#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <functional>
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
    /**
     * @brief 包装非类成员函数的结构体
    */
    struct DelegateMethod
    {
        // 定时任务回调函数
        std::function<bool(CArgs*, void*)> m_pFunc;
        // 定时任务数据
        void* m_pData{ nullptr };
    };

public:
    /**
     * @brief 添加一个定时器任务
     * @param delegate 定时任务执行对象
     * @param pDelegateArgs 定时任务执行对象参数
     * @param pArgs 定时任务参数
     * @param nInterval 执行间隔
     * @param nCount 执行次数
     * @param szFile 定时器添加的文件
     * @param nLine 定时器添加的代码行
     * @return HTIMER 定时器句柄
     */
    HTIMER AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile = "", int32_t nLine = 0);
    /**
     * @brief 添加一个定时器任务
     * @param callback 定时任务回调
     * @param pDelegateArgs 定时任务执行对象参数
     * @param pArgs 定时任务参数
     * @param nInterval 执行间隔
     * @param nCount 执行次数
     * @param szFile 添加定时器的文件
     * @param nLine 添加定时器的代码行数
     * @return HTIMER 定时器句柄
     */
    HTIMER AddTimer(const std::function<bool(CArgs*, void*)>& callback, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile = "", int32_t nLine = 0);
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
    /**
     * @brief 非类成员函数结构体的执行函数
     * @param pArgs 定时任务对象参数
     * @param pData 定时任务数据
     * @return bool 返回值
     */
    bool ExecuteDelegateMethod(CArgs* pArgs, void* pData);

private:
    TimerWheel* m_pTimer{ nullptr };
};

#endif // !__TIMER_MANAGER_H__
