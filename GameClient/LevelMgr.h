#pragma once

#include "ALevel.h"

class LevelMgr
	: public singleton<LevelMgr>
{
	SINGLE(LevelMgr)
private:
	Ptr<ALevel>		m_CurLevel;	
	Ptr<GameObject> m_MainCam;

public:
	Ptr<ALevel> GetCurLevel() { return m_CurLevel; }
	Ptr<GameObject> GetMainCam() { return m_MainCam; }
	Ptr<GameObject> FindObjectByName(const wstring& _name);

private:
	void CreateGrounds();
	void CreateLights();
	void CreateEnemies();

public:
	void Init();
	void Progress();
};

