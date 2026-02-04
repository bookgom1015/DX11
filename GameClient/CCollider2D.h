#pragma once
#include "Component.h"

#include "CScript.h"

class CCollider2D;
typedef void(CScript::* COLLISION_EVENT)(CCollider2D*, CCollider2D*);
typedef void(Component::* COLLISION_EVENT_COMP)(CCollider2D*, CCollider2D*);

struct COLLISION_DELEGATE
{
    CScript*        Inst;
    COLLISION_EVENT MemFunc;
};

struct COLLISION_DELEGATE_COMP
{
    Component* Inst;
    COLLISION_EVENT_COMP MemFunc;
};

class CCollider2D :
    public Component
{
private:
    Vec2        m_Offset;
    Vec2        m_Scale;

    int         m_OverlapCount;
    Matrix      m_matWorld;
       
    vector<COLLISION_DELEGATE> m_vecBeginDel;
    vector<COLLISION_DELEGATE> m_vecOverDel;
    vector<COLLISION_DELEGATE> m_vecEndDel;

    vector<COLLISION_DELEGATE_COMP> m_vecBeginCompDel;
    vector<COLLISION_DELEGATE_COMP> m_vecOverCompDel;
    vector<COLLISION_DELEGATE_COMP> m_vecEndCompDel;

public:
    void AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);    
    void AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);
    void AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);        

    void AddDynamicBeginOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);
    void AddDynamicOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);
    void AddDynamicEndOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);

public:
    GET_SET(Vec2, Offset);
    GET_SET(Vec2, Scale);

    const Matrix& GetWorldMat() { return m_matWorld; }

private:
    void BeginOverlap(Ptr<CCollider2D> _Other);
    void Overlap(Ptr<CCollider2D> _Other);
    void EndOverlap(Ptr<CCollider2D> _Other);


public:
    virtual void FinalTick() override;
    
public:
    CCollider2D();
    virtual ~CCollider2D();
    friend class CollisionMgr;
};

