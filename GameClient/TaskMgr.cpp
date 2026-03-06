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
		case ETask::E_CreateObject: {
			Ptr<GameObject> pNewObj = (GameObject*)m_vecTask[i].Param_0;

			Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
			pCurLevel->AddObject(static_cast<int>(m_vecTask[i].Param_1), pNewObj);
			pCurLevel->SetChanged();

			if (LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing)
				pNewObj->Begin();
		}		
			break;
		case ETask::E_DestroyObject: {
			Ptr<GameObject> pObj = (GameObject*)m_vecTask[i].Param_0;

			if (!pObj->m_Dead) {
				pObj->m_Dead = true;
				m_Garbage.push_back(pObj);
				Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
				pCurLevel->SetChanged();
			}
		}
			break;
		case ETask::E_ChangeLevel: {
			const wchar_t* pLevelName = (const wchar_t*)m_vecTask[i].Param_0;
			Ptr<ALevel> pLevel = AssetMgr::GetInst()->Find<ALevel>(pLevelName);
			LevelMgr::GetInst()->ChangeLevel(pLevel);
		}
		break;
		case ETask::E_ChangeLevelState: {
			ELevelState::Type NextState = (ELevelState::Type)m_vecTask[i].Param_0;
			LevelMgr::GetInst()->ChangeLevelState(NextState);
		}
		break;
		case ETask::E_DeferredProcessing: {
			auto* func = reinterpret_cast<std::function<void()>*>(m_vecTask[i].Param_0);
			(*func)();
			delete func;
		}
		break;
		}
	}

	m_vecTask.clear();
}
