#pragma once


#include "GameObject.h"
#include "CCamera.h"

class RenderMgr
	: public singleton<RenderMgr>
{
	SINGLE(RenderMgr);
private:
	Ptr<CCamera>		m_MainCam;
	Ptr<GameObject>		m_DbgObj;		// 디버그 렌더링을 위한 Dummy GameObject
	list<DbgInfo>		m_DbgInfoList;	// 디버그 요청 정보



public:
	void RegisterCamera(Ptr<CCamera> _Cam) { m_MainCam = _Cam; }
	Ptr<CCamera> GetPOVCamera() { return m_MainCam; }
	void AddDebugInfo(const DbgInfo& _Info) { m_DbgInfoList.push_back(_Info); }

private:
	void Render_Debug();

public:
	void Init();
	void Progress();
};

