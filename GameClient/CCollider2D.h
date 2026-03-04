#pragma once
#include "Component.h"

#include "CScript.h"

struct CollisionData {
    CCollider2D* Collider;
    Vec3 HitNormal;
};

class CCollider2D;
typedef void(CScript::* COLLISION_EVENT)(CollisionData, CollisionData);
typedef void(Component::* COLLISION_EVENT_COMP)(CollisionData, CollisionData);

struct COLLISION_DELEGATE {
    CScript*        Inst;
    COLLISION_EVENT MemFunc;
};

struct COLLISION_DELEGATE_COMP {
    Component* Inst;
    COLLISION_EVENT_COMP MemFunc;
};

class CCollider2D : public Component {
    friend class CollisionMgr;

public:
    CCollider2D();
    CCollider2D(const CCollider2D& _Origin);
    virtual ~CCollider2D();

public:
    virtual void FinalTick() override;

private:
    void BeginOverlap(CollisionData _Other);
    void Overlap(CollisionData _Other);
    void EndOverlap(CollisionData _Other);

public:
    void AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);    
    void AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);
    void AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc);        

    void AddDynamicBeginOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);
    void AddDynamicOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);
    void AddDynamicEndOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc);

public:
    CLONE(CCollider2D);

    GET_SET(Vec2, Offset);
    GET_SET(Vec2, Scale);

    const Matrix& GetWorldMat() { return m_matWorld; }

private:
    Vec2 m_Offset;
    Vec2 m_Scale;

    int m_OverlapCount;
    Matrix m_matWorld;

    vector<COLLISION_DELEGATE> m_vecBeginDel;
    vector<COLLISION_DELEGATE> m_vecOverDel;
    vector<COLLISION_DELEGATE> m_vecEndDel;

    vector<COLLISION_DELEGATE_COMP> m_vecBeginCompDel;
    vector<COLLISION_DELEGATE_COMP> m_vecOverCompDel;
    vector<COLLISION_DELEGATE_COMP> m_vecEndCompDel;
};

