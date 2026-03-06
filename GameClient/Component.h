#pragma once

#include "Entity.h"

#define GET_OTHER_COMPONENT(COM_NAME) class C##COM_NAME* COM_NAME();

class GameObject;

class Component : public Entity {    
    friend class GameObject;

public:
    Component(EComponent::Type _Type);
    Component(const Component& _Origin);
    virtual ~Component();

public:
    virtual void Init() {}
    virtual void Begin() {}
    virtual void LateTick() {}
    virtual void FinalTick() = 0;

    virtual Component* Clone() = 0;

public:
    __forceinline EComponent::Type GetType() const;
    __forceinline GameObject* GetOwner() const;

    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(TileRender);
    GET_OTHER_COMPONENT(FlipbookRender);
    GET_OTHER_COMPONENT(SpriteRender);

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(BillboardRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
    GET_OTHER_COMPONENT(RigidBody);

private:
    const EComponent::Type m_Type;    // 컴포넌트 타입
    GameObject* m_Owner;            // 컴포넌트를 소유한 게임오브젝트
};

#include "Component.inl"