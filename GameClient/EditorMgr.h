#pragma once

#include "EditorUI.h"
#include "SceneUI.h"
#include "LogUI.h"

class EditorMgr
	: public singleton<EditorMgr>
{
	SINGLE(EditorMgr);
private:
	map<string, Ptr<EditorUI>>	m_mapUI;
	bool						m_ShowDemo;

	Ptr<EditorUI>				m_FocusedUI;
	Ptr<SceneUI>				m_SceneUI;
	Ptr<LogUI>					m_LogUI;

	vector<Ptr<GameObject>>		m_EditorObject;

public:
	bool IsShowDemo() { return m_ShowDemo; }
	void ShowDemo(bool _Show) { m_ShowDemo = _Show; }

	void AddUI(const string& _UIName, Ptr<EditorUI> _UI);
	Ptr<EditorUI> FindUI(const string& _UIName);

	void RegisterFocusedUI(Ptr<EditorUI> _UI) { m_FocusedUI = _UI; }

	void AddLog(const LogEntry& entry);
	void AddInfoLog(const std::string& msg);

	HWND GetSceneHWND() { return m_SceneUI->GetSceneHWND(); }

	void SetDpi() { SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); }

public:
	void Init();
	void Progress();

private:
	void Tick();
	void Render();

private:
	void CreateEditorUI();
	void CreateEditorObject();
};

