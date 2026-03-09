#ifndef __GAMEOBJECT_INL__
#define __GAMEOBJECT_INL__

Ptr<Component> GameObject::GetComponent(EComponent::Type _Type) const { 
    return m_Com[(UINT)_Type]; 
}

Ptr<GameObject> GameObject::GetParent() const { return m_Parent; }

Ptr<GameObject> GameObject::GetChild(int _idx) const { return m_vecChild[_idx]; }

const vector<Ptr<GameObject>>& GameObject::GetChild() const { return m_vecChild; }

bool GameObject::IsDead() const { return m_Dead; }

Ptr<CRenderComponent> GameObject::GetRenderCom() const { return m_RenderCom; }

constexpr int GameObject::GetLayerIndex() const noexcept { return m_LayerIdx; }

bool IsValid(Ptr<GameObject>& _Object);

template<typename T>
Ptr<T> GameObject::GetScript() {
    for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i) {
        T* pScript = dynamic_cast<T*>(m_vecScripts[i].Get());
        if (nullptr == pScript) continue;

        return pScript;
    }

    return nullptr;
}

Ptr<CScript> GameObject::GetScript(SCRIPT_TYPE _Type) {
    for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i) {
        auto script = m_vecScripts[i].Get();
        if (script->GetScriptType() == _Type) return script;
    }

    return nullptr;
}

#endif // __GAMEOBJECT_INL__