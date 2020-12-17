#ifndef __TIMER_WHEEL_H__
#define __TIMER_WHEEL_H__

#include <deque>
#include <array>
#include <unordered_map>
#include <list>
#include "timer_node.h"


/**
 * 5层时间轮, 由一个链表的数组维护
 * 数组中含有N个slot(槽位), N为数组大小, 每个slot维护一个定时器节点的链表
 * 时间轮的精度和轮询数组的interval(时间间隔)有关, interval越小, 精度越高, 当前实现按照1ms跳到下一个slot
 * 最里面的一层时间轮取低8位二进制作为slot的索引(0-255), 其它N层时间轮依次向左取6位二进制作为slot的索引(0-63)
 * 那么5层时间轮可以管理的时间范围是(2^32-1 )* interval
 * 这里我用一个数组保存5层时间轮的所有链表, 通过计算偏移量找到定时器节点所在的链表的数组位置, OFFSET(N) + INDEX(V, N)即是定时器节点最终在数组的slot位置
 */

#define TVN_BITS 6                                                          //>> 6位二进制
#define TVR_BITS 8                                                          //>> 8位二进制
#define TVN_SIZE (1 << TVN_BITS)                                            //>> 64 外层时间轮的slot数目
#define TVR_SIZE (1 << TVR_BITS)                                            //>> 256 一层时间轮的slot数目
#define TVN_MASK (TVN_SIZE - 1)                                             //>> 63-->00111111 外层时间轮slot的最大索引值
#define TVR_MASK (TVR_SIZE - 1)                                             //>> 255-->11111111 一层时间轮slot的最大索引值
#define OFFSET(N) (TVR_SIZE + (N) * TVN_SIZE)                               //>> 第N+1层时间轮在数组中的起始位置 N>=0
#define INDEX(V, N) ((V >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)         //>> 定时器节点在当前层级的数组位置 N>=0
#define MAX_SLOT 256 + 4 * 64                                               //>> 数组大小

class TimerWheel
{
public:
    TimerWheel();
    ~TimerWheel();

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
    virtual HTIMER AddTimer(const CDelegate& delegate, CArgs* pDelegateArgs, void* pArgs, int32_t nInterval, int32_t nCount, const char* szFile, int32_t uLine);
    /**
     * @brief 删除一个定时器节点
     * @param uTimer 定时器句柄
     * @return void
     */
    virtual void KillTimer(HTIMER uTimer);

    /**
     * @brief 轮询
     * @param nDelta 帧间隔时间
     * @return void
     */
    virtual void Update(int32_t nDelta);
    /**
     * @brief 释放所有定时器
     * @return void
     */
    virtual void Release();
    /**
     * @brief 获取当前定时器数量
     * @return uint32_t 返回数量
     */
    uint32_t GetTimerNum() const
    {
        return static_cast<uint32_t>(m_oAllTimers.size());
    }

private:
    using CTimersMap = std::unordered_map<HTIMER, CTimerNode*>;
    using CFreeNode =  std::deque<CTimerNode*>;
    using CTimersArray = std::array<CTimerNode, MAX_SLOT>;
    /**
     * @brief 初始化时间轮
     * @return void
     */
    void _Init();
    /**
     * @brief 释放时间轮
     * @return void
     */
    void _UnInit();
    /**
     * @brief 获取一个新的句柄
     * @return HTIMER
     */
    HTIMER _GetNewTimerID();
    /**
     * @brief 获取一个定时器节点
     * @return CTimerNode* 节点指针
     */
    CTimerNode* _GetFreeNode();
    /**
     * @brief 回收节点
     * @param pNode 节点指针
     * @return void
     */
    void _AddFreeNode(CTimerNode* pNode);
    /**
     * @brief 添加定时器节点
     * @param pNode 定时器节点
     * @return void
     */
    void _AddTimer(CTimerNode* pNode);
    /**
     * @brief 时间节点重新层级变化
     * @param nOff 偏移量
     * @param nIndex 当前层上的位置
     * @return int32_t 在数组中的位置
     */
    int32_t _CascadeTime(int32_t nOff, int32_t nIndex);

private:
    // 可以通过句柄查找所有定时器节点的容器
    CTimersMap m_oAllTimers{};
    // 维护5层时间轮的数组
    CTimersArray m_oTimerNodes{};
    // 已经删除的定时器节点队列
    CFreeNode m_oFreeNodes{};
    // 下一个可以使用的句柄
    HTIMER m_uNextID{ 0 };
    // 时间轮当前时间
    int64_t m_nCurTime{ 0 };
};

#endif
