#pragma once

#include "EditorUI.h"

#include "GameObject.h"

#include "ComponentUI.h"
#include "AssetUI.h"
#include "ScriptUI.h"

class Inspector : public EditorUI {
public:
    Inspector();
    virtual ~Inspector();

public:
    virtual void Tick_UI() override;

public:
    void SetTargetObject(Ptr<GameObject> _Object);
    __forceinline Ptr<GameObject> GetTargetObject() const;

    void SetTargetAsset(Ptr<Asset> _Asset);

    void NeedToResetTarget();

private:
    void CreateChildUI();

private:
    Ptr<GameObject> m_TargetObject;
    Ptr<ComponentUI> m_arrComUI[EComponent::Count];
    Ptr<ComponentUI> m_AddCompBtn;

    Ptr<Asset> m_TargetAsset;
    Ptr<AssetUI> m_arrAssetUI[EAsset::Count];

    Ptr<ScriptUI> m_arrScriptUI[SCRIPT_TYPE::Count];
};

#include "Inspector.inl"