#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#define DELEGATE_COMBINATION(T_, Func_, Instance_) (CDelegate::RegisterMethod<T_, &T_::Func_>(Instance_))

class CArgs
{
public:
    virtual ~CArgs() {}
};

class CDelegate
{
public:
    CDelegate()
        : m_pObj(nullptr)
        , m_pFunStub(nullptr)
    {}
    ~CDelegate() {}

public:
    template<class T, bool (T::*TMethod)(CArgs*, void*)>
    static CDelegate RegisterMethod(T* pObj)
    {
        CDelegate d;
        d.m_pObj = pObj;
        d.m_pFunStub = &MethodStub<T, TMethod>;
        return d;
    }
    
    bool operator() (CArgs* pArgs, void* pArg) const
    {
        if (!m_pObj)
        {
            return false;
        }
        return m_pFunStub(m_pObj, pArgs, pArg);
    }

private:
    template<class T, bool (T::*TMethod)(CArgs*, void*)>
    static bool MethodStub(void* pObj, CArgs* pArgs, void* pArg)
    {
        T* ptr = static_cast<T*>(pObj);
        return (ptr->*TMethod)(pArgs, pArg);
    }

private:
    typedef bool (*stub_type)(void*, CArgs*, void*);
    void* m_pObj;
    stub_type m_pFunStub;
};

#endif
