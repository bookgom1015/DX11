#pragma once

#include "ALevel.h"

class LevelMgr
	: public singleton<LevelMgr>
{
	SINGLE(LevelMgr)
private:
	Ptr<ALevel>		m_CurLevel;
	Ptr<GameObject> mMainCam;

public:
	Ptr<ALevel> GetCurLevel() { return m_CurLevel; }
	Ptr<GameObject> GetMainCam() { return mMainCam; }

public:
	void Init();
	void Progress();
};

