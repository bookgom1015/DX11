#include "pch.h"
#include "CLight2D.h"

#include "RenderMgr.h"

#include "CTransform.h"

CLight2D::CLight2D() : Component(EComponent::E_Light2D) {}

CLight2D::~CLight2D() {}

void CLight2D::FinalTick() {
	m_Info.WorldPos = Transform()->GetWorldPos();

	RenderMgr::GetInst()->RegisterLight2D(this);
}

void CLight2D::SaveToLevelFile(FILE* const _FileStream) {
	fwrite(&m_Info, sizeof(Light2DInfo), 1, _FileStream);
}

void CLight2D::LoadFromLevelFile(FILE* const _FileStream) {
	fread(&m_Info, sizeof(Light2DInfo), 1, _FileStream);
}