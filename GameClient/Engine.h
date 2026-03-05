#pragma once

// 게임 프로그램의 최상위 관리자
class Engine : public singleton<Engine> {
	SINGLE(Engine)

public:
	__forceinline HINSTANCE GetInstance() const;
	__forceinline HWND GetMainWndHwnd() const;
	__forceinline Vec2 GetResolution() const;

public:
	int Init(HINSTANCE _hInst, UINT _Width, UINT _Height, bool _EditorMode);
	int Progress();

private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	Vec2		m_Resolution;

	bool		m_EditorMode;
};

#include "Engine.inl"