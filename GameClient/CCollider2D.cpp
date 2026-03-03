#include "pch.h"
#include "CCollider2D.h"

#include "RenderMgr.h"
#include "CScript.h"

CCollider2D::CCollider2D() : Component(COMPONENT_TYPE::COLLIDER2D)
	, m_Scale(Vec2(1.f, 1.f))
	, m_OverlapCount(0) {}

CCollider2D::~CCollider2D() {}

void CCollider2D::FinalTick() {
	Matrix matTran = XMMatrixTranslation(m_Offset.x, m_Offset.y, 0.f);
	Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, 0.f);

	m_matWorld = matScale * matTran;
	m_matWorld *= Transform()->GetWorldMat();

	if (m_OverlapCount > 0)
		DrawDebugRect(m_matWorld, Vec4(1.f, 0.f, 0.f, 1.f), 0.f);
	else
		DrawDebugRect(m_matWorld, Vec4(0.f, 1.f, 0.f, 1.f), 0.f);
}


void CCollider2D::BeginOverlap(CollisionData _Other) {
	++m_OverlapCount;

	for (size_t i = 0, end = m_vecBeginDel.size(); i < end; ++i)
		(m_vecBeginDel[i].Inst->*m_vecBeginDel[i].MemFunc)({this, Vec3()}, _Other);

	for (size_t i = 0, end = m_vecBeginCompDel.size(); i < end; ++i)
		(m_vecBeginCompDel[i].Inst->*m_vecBeginCompDel[i].MemFunc)({ this, Vec3() }, _Other);
}

void CCollider2D::Overlap(CollisionData _Other) {
	for (size_t i = 0, end = m_vecOverDel.size(); i < end; ++i)
		(m_vecOverDel[i].Inst->*m_vecOverDel[i].MemFunc)({ this, Vec3() }, _Other);
	for (size_t i = 0, end = m_vecOverCompDel.size(); i < end; ++i)
		(m_vecOverCompDel[i].Inst->*m_vecOverCompDel[i].MemFunc)({ this, Vec3() }, _Other);
}

void CCollider2D::EndOverlap(CollisionData _Other) {
	--m_OverlapCount;

	for (size_t i = 0, end = m_vecEndDel.size(); i < end; ++i)
		(m_vecEndDel[i].Inst->*m_vecEndDel[i].MemFunc)({ this, Vec3() }, _Other);
	for (size_t i = 0, end = m_vecEndCompDel.size(); i < end; ++i)
		(m_vecEndCompDel[i].Inst->*m_vecEndCompDel[i].MemFunc)({ this, Vec3() }, _Other);
}


void CCollider2D::AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) {
	m_vecBeginDel.push_back(COLLISION_DELEGATE{ _Inst , _MemFunc });
}

void CCollider2D::AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) {
	m_vecOverDel.push_back(COLLISION_DELEGATE{ _Inst , _MemFunc });
}

void CCollider2D::AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) {
	m_vecEndDel.push_back(COLLISION_DELEGATE{ _Inst , _MemFunc });
}

void CCollider2D::AddDynamicBeginOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc) {
	m_vecBeginCompDel.push_back(COLLISION_DELEGATE_COMP{ _Inst , _MemFunc });
}

void CCollider2D::AddDynamicOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc) {
	m_vecOverCompDel.push_back(COLLISION_DELEGATE_COMP{ _Inst , _MemFunc });
}

void CCollider2D::AddDynamicEndOverlap(Component* _Inst, COLLISION_EVENT_COMP _MemFunc) {
	m_vecEndCompDel.push_back(COLLISION_DELEGATE_COMP{ _Inst , _MemFunc });
}