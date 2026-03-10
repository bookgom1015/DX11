#ifndef __CLIGHT2D_INL__
#define __CLIGHT2D_INL__

Light2DInfo CLight2D::GetInfo() const { return m_Info; }

int CLight2D::IsDisabled()const { return m_Info.Disabled; }
void CLight2D::Disable(bool status) { m_Info.Disabled = (int)status; }

ELight::Type CLight2D::GetLightType()const { return m_Info.Type; }
void CLight2D::SetLightType(ELight::Type _Type) { m_Info.Type = _Type; }

Vec3 CLight2D::GetLightColor() const { return m_Info.Color; }
void CLight2D::SetLightColor(Vec3 _Color) { m_Info.Color = _Color; }

void CLight2D::SetLightDir(Vec3 _Dir) { m_Info.LightDir = _Dir; }
Vec3 CLight2D::GetLightDir() const { return m_Info.LightDir; }

Vec3 CLight2D::GetAmbient() const { return m_Info.Ambient; }
void CLight2D::SetAmbient(Vec3 _Ambient) { m_Info.Ambient = _Ambient; }

float CLight2D::GetRadius() const { return m_Info.Radius; }
void CLight2D::SetRadius(float _R) { m_Info.Radius = _R; }

float CLight2D::GetAngle() const { return m_Info.Angle; }
void CLight2D::SetAngle(float _Angle) { m_Info.Angle = _Angle; }

float CLight2D::GetLength() const { return m_Info.Length; }
void CLight2D::SetLength(float _Length) { m_Info.Length = _Length; }

float CLight2D::GetIntensity() const { return m_Info.Intensity; }
void CLight2D::SetIntensity(float _Intensity) { m_Info.Intensity = _Intensity; }

Matrix CLight2D::GetView(int idx) const { return m_Info.Views[idx]; }
void CLight2D::SetView(Matrix mat, int idx) { m_Info.Views[idx] = mat; }

Matrix CLight2D::GetProj(int idx) const { return m_Info.Projs[idx]; }
void CLight2D::SetProj(Matrix mat, int idx) { m_Info.Projs[idx] = mat; }

int CLight2D::GetBaseIndex() const { return m_Info.BaseIndex; }
void CLight2D::SetBaseIndex(int idx) { m_Info.BaseIndex = idx; }

int CLight2D::GetSize() const { return m_Info.Size; }
void CLight2D::SetSize(int idx) { m_Info.Size = idx; }

#endif // __CLIGHT2D_INL__