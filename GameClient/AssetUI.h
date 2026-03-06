#pragma once

#include "Asset.h"

#include "EditorUI.h"

class AssetUI : public EditorUI {
public:
    AssetUI(EAsset::Type _Type);
    virtual ~AssetUI();

public:
    __forceinline void SetTargetAsset(Ptr<Asset> _Asset);
    __forceinline Ptr<Asset> GetTargetAsset() const;

protected:
    void OutputTitle();

private:
    const EAsset::Type m_AssetType;
    Ptr<Asset> m_TargetAsset;
};

#include "AssetUI.inl"