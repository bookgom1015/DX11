#pragma once

#include "CRenderComponent.h"

#include "assets.h"

class CMeshRender : public CRenderComponent {
public:
    CMeshRender();
    virtual ~CMeshRender();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override {}

public:
    CLONE(CMeshRender);

    GET_SET(Vec4, Albedo);

private:
    Vec4 m_Albedo;
};

