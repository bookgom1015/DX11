#pragma once
#include "EditorUI.h"

#include "GameObject.h"
#include "ComponentUI.h"
#include "AssetUI.h"

class Inspector :
    public EditorUI
{
private:
    Ptr<GameObject>     m_TargetObject;
    Ptr<ComponentUI>    m_arrComUI[(UINT)COMPONENT_TYPE::END];

    Ptr<Asset>          m_TargetAsset;
    Ptr<AssetUI>        m_arrAssetUI[(UINT)ASSET_TYPE::END];

public:
    virtual void Tick_UI() override;

public:
    void SetTargetObject(Ptr<GameObject> _Object);
    Ptr<GameObject> GetTargetObject() { return m_TargetObject; }

    void SetTargetAsset(Ptr<Asset> _Asset);

private:
    void CreateChildUI();

public:
    Inspector();
    virtual ~Inspector();
};


