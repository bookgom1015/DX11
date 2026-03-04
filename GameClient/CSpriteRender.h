#pragma once

#include "CRenderComponent.h"
#include "ASprite.h"

class CSpriteRender : public CRenderComponent {
public:
    CSpriteRender();
    virtual ~CSpriteRender();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;

public:
    CLONE(CSpriteRender);

    GET_SET(Vec4, Albedo);

    __forceinline void SetSprite(Ptr<ASprite> _Sprite);
    __forceinline Ptr<ASprite> GetSprite();

    __forceinline void SetGlitchEffect(bool status);
    __forceinline void SetRelicEffect(bool status);
    __forceinline void SetEmit(bool status);

private:
    Ptr<ASprite> m_Sprite;
    Vec4 m_Albedo;
    bool m_GlitchEffectEnabled{};
    bool m_RelicEffectEnabled{};
    bool m_Emit{};
};

#include "CSpriteRender.inl"