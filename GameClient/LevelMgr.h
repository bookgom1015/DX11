#pragma once

#include "ALevel.h"

class LevelMgr
	: public singleton<LevelMgr>
{
	SINGLE(LevelMgr)
private:
	Ptr<ALevel>		m_CurLevel;

public:
	Ptr<ALevel> GetCurLevel() { return m_CurLevel; }

public:
	void Init();
	void Progress();
};

