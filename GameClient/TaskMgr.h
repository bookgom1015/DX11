#pragma once

#include "GameObject.h"

class TaskMgr : public singleton<TaskMgr> {
	SINGLE(TaskMgr);

public:
	void Progress();

public:
	void AddTask(const TaskInfo& _info) { m_vecTask.push_back(_info); }

private:
	vector<TaskInfo>		m_vecTask;
	vector<Ptr<GameObject>> m_Garbage;
};
