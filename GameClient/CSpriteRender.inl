#ifndef __CSPRITERENDER_INL__
#define __CSPRITERENDER_INL__

void CSpriteRender::SetSprite(Ptr<ASprite> _Sprite) { m_Sprite = _Sprite; }

Ptr<ASprite> CSpriteRender::GetSprite() { return m_Sprite; }

void CSpriteRender::SetGlitchEffect(bool status) { m_GlitchEffectEnabled = status; }

void CSpriteRender::SetRelicEffect(bool status) { m_RelicEffectEnabled = status; }

void CSpriteRender::SetEmit(bool status) { m_Emit = status; }

#endif // __CSPRITERENDER_INL__