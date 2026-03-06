#pragma once

#include "ALevel.h"

class LevelMgr : public singleton<LevelMgr> {
	SINGLE(LevelMgr)

public:
	void Init();
	void Progress();

public:
	Ptr<ALevel> GetCurLevel() { return m_CurLevel; }
	Ptr<GameObject> FindObjectByName(const wstring& _name);

	ELevelState::Type GetLevelState() const { return m_LevelState; }

public:
	void ChangeLevel(Ptr<ALevel> level);
	void ChangeLevelState(ELevelState::Type state);

private:
	void CreatePlayer(Ptr<ALevel> level);
	void CreateGrounds(Ptr<ALevel> level);
	void CreateLights(Ptr<ALevel> level);
	void CreateEnemies(Ptr<ALevel> level);

private:
	Ptr<ALevel> m_CurLevel;
	Ptr<ALevel> m_SharedLevel;

	ELevelState::Type m_LevelState;

	bool m_LevelResetRequested;
};

