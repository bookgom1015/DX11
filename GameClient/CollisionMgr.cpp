#include "pch.h"
#include "CollisionMgr.h"

#include "AssetMgr.h"

CollisionMgr::CollisionMgr() {}

CollisionMgr::~CollisionMgr() {}

void CollisionMgr::Progress(Ptr<ALevel> _Level) {
	UINT* pMatrix = _Level->GetCollisionMatrix();

	for (UINT Row = 0; Row < MAX_LAYER; ++Row) {
		for (UINT Col = Row; Col < MAX_LAYER; ++Col) {
			if (false == (pMatrix[Row] & (1 << Col))) continue;

			CollisionBtwLayer(_Level->GetLayer(Row), _Level->GetLayer(Col));
		}
	}
}

bool CollisionMgr::IsCollision(Ptr<CCollider2D> collider, Vec3 pos) {
	Ptr<AMesh> pRectMesh = FIND(AMesh, L"RectMesh");
	const Vtx* pVtx = pRectMesh->GetVtxSysMem();

	const Matrix& W = collider->GetWorldMat();

	// RectMesh의 로컬 기준에서 "중심(0)"과 "두 축 방향 벡터(엣지)"를 월드로 만든다.
	// (0 -> 1), (0 -> 3) 이 두 엣지가 직사각형의 두 축.
	const Vec3 p0 = XMVector3TransformCoord(pVtx[0].vPos, W);
	const Vec3 p1 = XMVector3TransformCoord(pVtx[1].vPos, W);
	const Vec3 p3 = XMVector3TransformCoord(pVtx[3].vPos, W);

	Vec3 axisX = p1 - p0;   // 월드 공간에서의 로컬 X축 방향(스케일 포함)
	Vec3 axisY = p3 - p0;   // 월드 공간에서의 로컬 Y축 방향(스케일 포함)

	float lenX = axisX.Length();
	float lenY = axisY.Length();

	// 축 길이가 0에 가깝면(스케일 0 등) 충돌 판정 불가 -> 안전하게 false
	if (lenX < 1e-6f || lenY < 1e-6f)
		return false;

	// 정규화된 축(투영 축)
	Vec3 uX = axisX / lenX;
	Vec3 uY = axisY / lenY;

	// 중심은 월드행렬로 원점 변환 (네 코드 스타일 그대로)
	Vec3 center = XMVector3TransformCoord(Vec3(0.f), W);

	// 점을 콜라이더 로컬(축 기준)으로 투영: center->pos 벡터를 각 축으로 dot
	Vec3 d = pos - center;

	float distX = fabs(d.Dot(uX));
	float distY = fabs(d.Dot(uY));

	// RectMesh의 (0->1), (0->3) 엣지 길이는 "전체 폭/높이"에 해당.
	// 반-폭(half extent)은 그 절반.
	float halfX = lenX * 0.5f;
	float halfY = lenY * 0.5f;

	// 분리축 테스트(2개 축만)
	if (distX > halfX) return false;
	if (distY > halfY) return false;

	return true;
}

void CollisionMgr::CollisionBtwLayer(Layer* _Left, Layer* _Right) {
	const vector<Ptr<GameObject>>& vecLeft = _Left->GetAllObjects();
	const vector<Ptr<GameObject>>& vecRight = _Right->GetAllObjects();

	for (size_t i = 0, endL = vecLeft.size(); i < endL; ++i) {
		if (nullptr == vecLeft[i]->Collider2D()) continue;

		for (size_t j = 0, endR = vecRight.size(); j < endR; ++j) {
			if (nullptr == vecRight[j]->Collider2D()) continue;

			// 두 충돌체의 고유 ID 로 조합을한 키값 생성
			COL_ID colid;
			colid.LeftID = vecLeft[i]->Collider2D()->GetID();
			colid.RightID = vecRight[j]->Collider2D()->GetID();

			auto iter = m_mapColID.find(colid.ID);

			if (iter == m_mapColID.end()) {
				m_mapColID.insert(make_pair(colid.ID, false));
				iter = m_mapColID.find(colid.ID);
			}

			CollisionData data{};
			bool isDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();
			
			if (IsCollision(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D(), data)) {
				if (isDead) {
					vecLeft[i]->Collider2D()->EndOverlap({
						vecRight[j]->Collider2D().Get(), data.HitNormal });
					vecRight[j]->Collider2D()->EndOverlap({
						vecLeft[i]->Collider2D().Get(), data.HitNormal });
				}
				else if (iter->second) {
					vecLeft[i]->Collider2D()->Overlap({ 
						vecRight[j]->Collider2D().Get(), data.HitNormal});
					vecRight[j]->Collider2D()->Overlap({ 
						vecLeft[i]->Collider2D().Get(), data.HitNormal });
				}
				else {
					data.HitNormal = -1.f * data.HitNormal;
					vecLeft[i]->Collider2D()->BeginOverlap({ 
						vecRight[j]->Collider2D().Get(), data.HitNormal });
					vecRight[j]->Collider2D()->BeginOverlap({ 
						vecLeft[i]->Collider2D().Get(), data.HitNormal });
				}

				iter->second = true;
			}
			else {
				if (iter->second)
				{
					vecLeft[i]->Collider2D()->EndOverlap({ 
						vecRight[j]->Collider2D().Get(), data.HitNormal });
					vecRight[j]->Collider2D()->EndOverlap({ 
						vecLeft[i]->Collider2D().Get(), data.HitNormal });
				}

				iter->second = false;
			}
		}
	}
}

bool CollisionMgr::IsCollision(
		Ptr<CCollider2D> _LeftCol, 
		Ptr<CCollider2D> _RightCol, 
		CollisionData& data) {
	Ptr<AMesh> pRectMesh = FIND(AMesh, L"RectMesh");

	const Vtx* pVtx = pRectMesh->GetVtxSysMem();

	const Matrix& matWorldLeft = _LeftCol->GetWorldMat();
	const Matrix& matWorldRight = _RightCol->GetWorldMat();

	// 월드 공간상에서 충돌을 검사하기 위해서, RectMesh 모델을 각 충돌체의 월드행렬을 곱해서 정점을 충돌체 꼭지점에 배치시킨다.
	// 각 꼭지점끼리 빼서 두 충돌체의 표면 방향벡터를 각 충돌체로부터 2개씩 구한다.
	Vec3 Axis[4]{};
	Axis[0] = XMVector3TransformCoord(pVtx[1].vPos, matWorldLeft) 
		- XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
	Axis[1] = XMVector3TransformCoord(pVtx[3].vPos, matWorldLeft) 
		- XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
	Axis[2] = XMVector3TransformCoord(pVtx[1].vPos, matWorldRight) 
		- XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
	Axis[3] = XMVector3TransformCoord(pVtx[3].vPos, matWorldRight) 
		- XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
		
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f), matWorldRight) 
		- XMVector3TransformCoord(Vec3(0.f), matWorldLeft);

	float minPenetration = FLT_MAX;
	Vec3 bestAxis{};

	for (int i = 0; i < 4; ++i) {	
		// 4 개의 축 중에서, 하나를 투영 목적지로 정함
		// 원본값을 훼손하면 나중에 투영할때 문제가 생기기 때문에, 정규화한 벡터를 따로 지역변수로 둠
		Vec3 vProjAxis = Axis[i];
		vProjAxis.Normalize();

		// 투영축으로 4개의 벡터를 투영시켜서 얻은 면적의 절반 길이를 구함
		float Dot = 0.f;
		for (int j = 0; j < 4; ++j) 
			Dot += fabs(vProjAxis.Dot(Axis[j]));
		Dot /= 2.f;

		// 두 충돌체의 중심끼리 이은 벡터도 투영시킴
		float fCenter = fabs(vCenter.Dot(vProjAxis));

		// 중심끼리 이은 벡터의 면적이 더크다면, 두 충돌체를 나눌 수 있는 분리축이 존재함
		if (fCenter > Dot) return false;

		float penetration = Dot - fCenter;

		if (penetration < minPenetration) {
			minPenetration = penetration;

			bestAxis = vProjAxis;
			if (vCenter.Dot(bestAxis) < 0.f) bestAxis = -bestAxis;
		}
	}

	data.HitNormal = bestAxis;

	return true;
}
