#include "pch.h"
#include "CMonsterScript.h"

#include "GameObject.h"
#include "CCollider2D.h"

CMonsterScript::CMonsterScript()
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Begin()
{
	ADD_DYNAMIC_BEGIN_OVERLAP(CMonsterScript::BeginOverlap);
	ADD_DYNAMIC_END_OVERLAP(CMonsterScript::EndOverlap);
}

void CMonsterScript::Tick()
{
}

void CMonsterScript::BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	if (nullptr != GetOwner()->GetRenderCom() || nullptr != GetOwner()->GetRenderCom()->GetMaterial())
	{
		// 가리키고 있던 원본재질을 복사생해서 자신만의 재질을 만들고, 그것을 반환
		Ptr<AMaterial> pDynamicMtrl = GetOwner()->GetRenderCom()->CreateDynamicMaterial();		
		pDynamicMtrl->SetScalar(VEC4_0, Vec4(2.f, 1.f, 1.f, 1.f));
	}
}

void CMonsterScript::EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	if (nullptr != GetOwner()->GetRenderCom() || nullptr != GetOwner()->GetRenderCom()->GetMaterial())
	{
		// 현재 사용중인 재질을 원본 재질을 가리키게 함
		GetOwner()->GetRenderCom()->GetSharedMaterial();
	}
}