#include "pch.h"
#include "TaskMgr.h"

#include "LevelMgr.h"
#include "GameObject.h"

TaskMgr::TaskMgr()
{
}

TaskMgr::~TaskMgr()
{
}

void TaskMgr::Progress()
{
	// 쓰레기통 수거
	m_Garbage.clear();


	// Task 처리
	for (size_t i = 0; i < m_vecTask.size(); ++i)
	{
		switch (m_vecTask[i].Type)
		{
		case TASK_TYPE::CREATE_OBJECT:
		{
			Ptr<GameObject> pNewObj = (GameObject*)m_vecTask[i].Param_0;

			Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
			pCurLevel->AddObject(m_vecTask[i].Param_1, pNewObj);

			// 레벨에 추가된 오브젝트는, 레벨 시작시점때 Begin 을 호출받지 못하기 때문에,
			// 레벨에 스폰될 때 Begin 을 호출받는다.
			pNewObj->Begin();
		}		
			break;
		case TASK_TYPE::DESTROY_OBJECT:
		{
			Ptr<GameObject> pObj = (GameObject*)m_vecTask[i].Param_0;

			if (false == pObj->m_Dead)
			{
				pObj->m_Dead = true;
				m_Garbage.push_back(pObj);
			}
		}
			break;
		}
	}

	m_vecTask.clear();
}
