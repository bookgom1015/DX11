#pragma once
#include "Entity.h"

#include "components.h"

#define GET_COMPONENT(COM_NAME, COM_TYPE)     Ptr<C##COM_NAME> COM_NAME() { return (C##COM_NAME*)m_Com[(UINT)COMPONENT_TYPE::COM_TYPE].Get(); }


class GameObject :
    public Entity
{
private:
    Ptr<Component>          m_Com[(UINT)COMPONENT_TYPE::END];
    Ptr<CRenderComponent>   m_RenderCom;
    vector<Ptr<CScript>>    m_vecScripts;

    GameObject*             m_Parent;
    vector<Ptr<GameObject>> m_vecChild;

    // GameObject 본인이 속한 Layer Index, 
    // -1 인 경우, 어떤 레이어에도 속하지 않는다 == 레벨안에 있지 않은 오브젝트
    int                     m_LayerIdx; 
    bool                    m_Dead;

public:
    // 레벨이 처음 시작될때 호출되는 함수
    void Begin();

    // 매 프레임마다 할 일
    void Tick();

    // 매 프레임마다 Tick 이후에 뒷 수습작업 수행
    void FinalTick();

    // 자신을 타겟에 그림
    void Render();

public:
    void AddComponent(Ptr<Component> _Com);
    Ptr<Component> GetComponent(COMPONENT_TYPE _Type) { return m_Com[(UINT)_Type]; }

    void AddChild(Ptr<GameObject> _Child) { m_vecChild.push_back(_Child); _Child->m_Parent = this; }

    Ptr<GameObject> GetParent() { return m_Parent; }
    Ptr<GameObject> GetChild(int _idx) { return m_vecChild[_idx]; }

    bool IsDead() { return m_Dead; }
    void Destroy();

    // GET_COMPONENT
    GET_COMPONENT(SpriteRender, SPRITE_RENDER);
    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(BillboardRender, BILLBOARD_RENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    GET_COMPONENT(RigidBody, RIGIDBODY);

    Ptr<CRenderComponent> GetRenderCom() { return m_RenderCom; }

    constexpr int GetLayerIndex() const noexcept { return m_LayerIdx; }

private:
    void RegisterLayer();

public:
    GameObject();
    virtual ~GameObject();

    friend class Layer;
    friend class TaskMgr;
};


bool IsValid(Ptr<GameObject>& _Object);

