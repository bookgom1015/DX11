#pragma once
#include "Component.h"

class CLight2D :
    public Component
{
private:
    Light2DInfo     m_Info;

public:
    virtual void FinalTick() override;    

public:
    const Light2DInfo& GetInfo() { return m_Info; }

    int IsDisabled() { return m_Info.Disabled; }
    void Disable(bool status) { m_Info.Disabled = (int)status; }

    LIGHT_TYPE GetLightType() { return m_Info.Type; }
    void SetLightType(LIGHT_TYPE _Type) { m_Info.Type = _Type; }

    Vec3 GetLightColor() { return m_Info.Color; }
    void SetLightColor(Vec3 _Color) { m_Info.Color = _Color; }

    Vec3 GetLightDir() { return m_Info.LightDir; }
    void SetLightDir(Vec3 _Dir) { m_Info.LightDir = _Dir; }
    
    Vec3 GetAmbient() { return m_Info.Ambient; }
    void SetAmbient(Vec3 _Ambient) { m_Info.Ambient = _Ambient; }

    float GetRadius() { return m_Info.Radius; }
    void SetRadius(float _R) { m_Info.Radius = _R; }

    float GetAngle() { return m_Info.Angle; }
    void SetAngle(float _Angle) { m_Info.Angle = _Angle; }

    float GetLength() { return m_Info.Length; }
    void SetLength(float _Length) { m_Info.Length = _Length; }

    float GetIntensity() { return m_Info.Intensity; }
    void SetIntensity(float _Intensity) { m_Info.Intensity = _Intensity; }

    Matrix GetView(int idx = 0) { return m_Info.Views[idx]; }
    void SetView(Matrix mat, int idx = 0) { m_Info.Views[idx] = mat; }

    Matrix GetProj(int idx = 0) { return m_Info.Projs[idx]; }
    void SetProj(Matrix mat, int idx = 0) { m_Info.Projs[idx] = mat; }

    int GetBaseIndex() { return m_Info.BaseIndex; }
    void SetBaseIndex(int idx) { m_Info.BaseIndex = idx; }

    int GetSize() { return m_Info.Size; }
    void SetSize(int idx) { m_Info.Size = idx; }

public:
    CLight2D();
    virtual ~CLight2D();
};

