#include "pch.h"
#include "CScript.h"

#include "TaskMgr.h"

CScript::CScript(int _ScriptType) 
	: Component(EComponent::E_Script), m_ScriptType(_ScriptType) {}

CScript::~CScript() {}

void CScript::Destroy() {
	if (GetOwner()->IsDead()) return;

	TaskInfo info{};
	info.Type = ETask::E_DestroyObject;
	info.Param_0 = (DWORD_PTR)GetOwner();

	TaskMgr::GetInst()->AddTask(info);
}
