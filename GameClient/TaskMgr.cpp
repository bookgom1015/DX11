#include "pch.h"
#include "TaskMgr.h"

#include "LevelMgr.h"
#include "AssetMgr.h"
#include "GameObject.h"

TaskMgr::TaskMgr() {}

TaskMgr::~TaskMgr() {}

void TaskMgr::Progress() {
	// 쓰레기통 수거
	m_Garbage.clear();	

	// Task 처리
	for (size_t i = 0; i < m_vecTask.size(); ++i) {
		switch (m_vecTask[i].Type) {
		case TASK_TYPE::CREATE_OBJECT: {
			Ptr<GameObject> pNewObj = (GameObject*)m_vecTask[i].Param_0;

			Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
			pCurLevel->AddObject(m_vecTask[i].Param_1, pNewObj);
			pCurLevel->SetChanged();

			if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing)
				pNewObj->Begin();
		}		
			break;
		case TASK_TYPE::DESTROY_OBJECT: {
			Ptr<GameObject> pObj = (GameObject*)m_vecTask[i].Param_0;

			if (!pObj->m_Dead) {
				pObj->m_Dead = true;
				m_Garbage.push_back(pObj);
				Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
				pCurLevel->SetChanged();
			}
		}
			break;
		case TASK_TYPE::CHANGE_LEVEL:
		{
			const wchar_t* pLevelName = (const wchar_t*)m_vecTask[i].Param_0;
			Ptr<ALevel> pLevel = AssetMgr::GetInst()->Find<ALevel>(pLevelName);
			LevelMgr::GetInst()->ChangeLevel(pLevel);
		}
		break;
		case TASK_TYPE::CHANGE_LEVEL_STATE:
		{
			ELevelState::Type NextState = (ELevelState::Type)m_vecTask[i].Param_0;
			LevelMgr::GetInst()->ChangeLevelState(NextState);
		}
		break;
		}
	}

	m_vecTask.clear();
}
