#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: Component(_Type)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::Init()
{
	CreateMaterial();
}

void CRenderComponent::SetMaterial(Ptr<AMaterial> _Mtrl)
{
	m_Mtrl = m_SharedMtrl = _Mtrl;
}

Ptr<AMaterial> CRenderComponent::GetSharedMaterial()
{
	m_Mtrl = m_SharedMtrl;
	return m_SharedMtrl;
}

Ptr<AMaterial> CRenderComponent::CreateDynamicMaterial()
{
	if (nullptr != m_DynamicMtrl)
	{
		m_Mtrl = m_DynamicMtrl;
		return m_DynamicMtrl;
	}
	else
	{
		m_Mtrl = m_DynamicMtrl = m_SharedMtrl->Clone();
		return m_DynamicMtrl;
	}	
}