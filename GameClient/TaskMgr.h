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

#define MAKE_DEFERRED_TASK(__task, ...) \
	NEW function<void()>([__VA_ARGS__]() __task)

#define DWORD_PTR_DEFERRED_TASK(__task, ...) \
	reinterpret_cast<DWORD_PTR>(MAKE_DEFERRED_TASK(__task, __VA_ARGS__));