#include "pch.h"
#include "CMissileScript.h"

#include "GameObject.h"
#include "TimeMgr.h"
#include "CCollider2D.h"

CMissileScript::CMissileScript()
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::Begin()
{	
	Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&CMissileScript::BeginOverlap);
	Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&CMissileScript::Overlap);
	Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&CMissileScript::EndOverlap);
}

void CMissileScript::Tick()
{
	// 추적 대상이 있다면
	if (IsValid(m_Target))
	{		
		Vec3 vTargetPos = m_Target->Transform()->GetWorldPos();
		Vec3 vPos = Transform()->GetRelativePos();

		static Vec3 vDir = Vec3(0.f, 0.f, 0.f);
		if (vDir == Vec3(0.f, 0.f, 0.f))
		{
			vDir = vTargetPos - vPos;
			vDir.Normalize();			
		}	
		vPos += vDir * 200.f * DT;

		//Vec3 vDirToTarget = vTargetPos - vPos;
		//vDirToTarget.Normalize();
		//vPos += vDirToTarget * 200.f * DT;

		Transform()->SetRelativePos(vPos);
	}
}

void CMissileScript::BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
}

void CMissileScript::Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
}

void CMissileScript::EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
}
