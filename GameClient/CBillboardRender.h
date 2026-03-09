#pragma once

#include "CRenderComponent.h"

class CBillboardRender : public CRenderComponent {
public:
    CBillboardRender();
    virtual ~CBillboardRender();

public:
    CLONE(CBillboardRender);

    virtual void SaveToLevelFile(FILE* const _FileStream) override;
    virtual void LoadFromLevelFile(FILE* const _FileStream) override;

    GET_SET(Vec2, BillboardScale);
    GET_SET(Vec3, Albedo);

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;

private:
    Vec2 m_BillboardScale;
    Ptr<ATexture> m_Tex;
    Vec3 m_Albedo;
};

