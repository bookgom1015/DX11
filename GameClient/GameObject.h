#pragma once

#include "Entity.h"
#include "components.h"

#define GET_COMPONENT(COM_NAME, COM_TYPE) Ptr<C##COM_NAME> COM_NAME() { \
    return (C##COM_NAME*)m_Com[EComponent::COM_TYPE].Get();   \
}

class GameObject : public Entity {
    friend class Layer;
    friend class TaskMgr;

public:
    GameObject();
    GameObject(const GameObject& _Origin);
    virtual ~GameObject();

public:
    // 레벨이 처음 시작될때 호출되는 함수
    void Begin();

    // 매 프레임마다 할 일
    void Tick();
    void LateTick();

    // 매 프레임마다 Tick 이후에 뒷 수습작업 수행
    void FinalTick();
    void FinalTick_Editor();

    // 자신을 타겟에 그림
    void Render();

    void SaveToLevelFile(FILE* const _FileStream);
    void LoadFromLevelFile(FILE* const _FileStream);

public:
    void AddComponent(Ptr<Component> _Com);
    __forceinline Ptr<Component> GetComponent(EComponent::Type _Type) const;

    template<typename T>
    __forceinline Ptr<T> GetScript();

    __forceinline Ptr<CScript> GetScript(SCRIPT_TYPE _Type);

    void AddChild(Ptr<GameObject> _Child);
    void DisconnectWithParent();
    void RegisterAsParent();
    void DeregisterAsParent();

    __forceinline Ptr<GameObject> GetParent() const;
    __forceinline Ptr<GameObject> GetChild(int _idx) const;
    __forceinline const vector<Ptr<GameObject>>& GetChild() const;

    __forceinline bool IsDead() const;
    void Destroy();

    __forceinline Ptr<CRenderComponent> GetRenderCom() const;

    __forceinline constexpr int GetLayerIndex() const noexcept;

public:
    CLONE(GameObject);

    // 특정 컴포넌트를 다운캐스팅해서 바로 리턴
    GET_COMPONENT(Light2D, E_Light2D);
    GET_COMPONENT(TileRender, E_TileRender);
    GET_COMPONENT(FlipbookRender, E_FlipbookRender);
    GET_COMPONENT(SpriteRender, E_SpriteRender);
    GET_COMPONENT(Transform, E_Transform);
    GET_COMPONENT(MeshRender, E_MeshRender);
    GET_COMPONENT(BillboardRender, E_BillboardRender);
    GET_COMPONENT(Camera, E_Camera);
    GET_COMPONENT(Collider2D, E_Collider2D);
    GET_COMPONENT(RigidBody, E_Rigidbody);

private:
    void RegisterLayer();

private:
    Ptr<Component> m_Com[EComponent::Type::Count];
    Ptr<CRenderComponent> m_RenderCom;
    vector<Ptr<CScript>> m_vecScripts;

    GameObject* m_Parent;
    vector<Ptr<GameObject>> m_vecChild;

    // GameObject 본인이 속한 Layer Index, 
    // -1 인 경우, 어떤 레이어에도 속하지 않는다 == 레벨안에 있지 않은 오브젝트
    int m_LayerIdx;
    bool m_Dead;
};

#include "GameObject.inl"
