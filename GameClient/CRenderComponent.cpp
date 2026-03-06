#include "pch.h"
#include "CRenderComponent.h"

#include "LevelMgr.h"

CRenderComponent::CRenderComponent(EComponent::Type _Type) : Component(_Type) {}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin) 
	: Component{ _Origin }
	, m_Mesh{ _Origin.m_Mesh }
	, m_SharedMtrl{ _Origin.m_SharedMtrl } {
	if (_Origin.m_Mtrl == _Origin.m_SharedMtrl) 
		m_Mtrl = m_SharedMtrl;
	else if (_Origin.m_DynamicMtrl != nullptr && _Origin.m_Mtrl == _Origin.m_DynamicMtrl) 
		m_Mtrl = CreateDynamicMaterial();
}

CRenderComponent::~CRenderComponent() {}

void CRenderComponent::Init() {
	CreateMaterial();
}

void CRenderComponent::SetMaterial(Ptr<AMaterial> _Mtrl) {
	m_Mtrl = m_SharedMtrl = _Mtrl;
}

Ptr<AMaterial> CRenderComponent::GetSharedMaterial() {
	m_Mtrl = m_SharedMtrl;
	return m_SharedMtrl;
}

Ptr<AMaterial> CRenderComponent::CreateDynamicMaterial() {
	assert(LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing);

	if (m_DynamicMtrl != nullptr) {
		m_Mtrl = m_DynamicMtrl;
		return m_DynamicMtrl;
	}
	else {
		m_Mtrl = m_DynamicMtrl = m_SharedMtrl->Clone();
		return m_DynamicMtrl;
	}	
}