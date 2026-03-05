#ifndef __ENGINE_INL__
#define __ENGINE_INL__

HINSTANCE Engine::GetInstance() const { return m_hInst; }

HWND Engine::GetMainWndHwnd() const { return m_hWnd; }

Vec2 Engine::GetResolution() const { return m_Resolution; }

#endif // __ENGINE_INL__