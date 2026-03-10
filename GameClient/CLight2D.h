#pragma once
#include "Component.h"

class CLight2D : public Component {
public:
    CLight2D();
    virtual ~CLight2D();

public:
    virtual void FinalTick() override;    

public:
    CLONE(CLight2D);

    virtual void SaveToLevelFile(FILE* const _FileStream) override;
    virtual void LoadFromLevelFile(FILE* const _FileStream) override;

public:
    __forceinline Light2DInfo GetInfo() const;

    __forceinline int IsDisabled() const;
    __forceinline void Disable(bool status);

    __forceinline ELight::Type GetLightType() const;
    __forceinline void SetLightType(ELight::Type _Type);

    __forceinline Vec3 GetLightColor() const;
    __forceinline void SetLightColor(Vec3 _Color);

    __forceinline Vec3 GetLightDir() const;
    __forceinline void SetLightDir(Vec3 _Dir);
    
    __forceinline Vec3 GetAmbient() const;
    __forceinline void SetAmbient(Vec3 _Ambient);

    __forceinline float GetRadius() const;
    __forceinline void SetRadius(float _R);

    __forceinline float GetAngle() const;
    __forceinline void SetAngle(float _Angle);

    __forceinline float GetLength() const;
    __forceinline void SetLength(float _Length);

    __forceinline void SetIntensity(float _Intensity);
    __forceinline float GetIntensity() const;

    __forceinline Matrix GetView(int idx = 0) const;
    __forceinline void SetView(Matrix mat, int idx = 0);

    __forceinline Matrix GetProj(int idx = 0) const;
    __forceinline void SetProj(Matrix mat, int idx = 0);

    __forceinline int GetBaseIndex() const;
    __forceinline void SetBaseIndex(int idx);

    __forceinline int GetSize() const;
    __forceinline void SetSize(int idx);

private:
    Light2DInfo m_Info;
};

#include "CLight2D.inl"