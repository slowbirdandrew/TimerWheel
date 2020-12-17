#include <chrono>
#include <time.h>
#include "timer_manager.h"

static int64_t s_DeltaTime = 0;
static int64_t s_Frame = 50;
static uint64_t s_FrameTime = 0;
static uint64_t s_LastFrameTime = 0;

uint64_t GetMilliSecond()
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    return ms.count();
}

int64_t GetDeltaTimeMillis()
{
    return s_FrameTime - s_LastFrameTime;
}

void UpdateFrameTime()
{
    s_LastFrameTime = s_FrameTime;
    s_FrameTime = GetMilliSecond();
}

void ProcessUpdate()
{
    int64_t nDelta = GetDeltaTimeMillis();
    s_DeltaTime += nDelta;
    if (s_DeltaTime >= s_Frame)
    {
        s_DeltaTime = nDelta < s_Frame ? s_DeltaTime - s_Frame : 0;
        CTimerManager::Instance().Update(nDelta);
    }
}

int main(int argc, char** argv)
{
    while (true)
    {
        UpdateFrameTime();
        ProcessUpdate();
    }
    return 0;
}