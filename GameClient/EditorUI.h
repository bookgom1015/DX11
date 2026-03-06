#pragma once

#include "Entity.h"

#include "ImGui/imgui.h"

class EditorUI : public Entity {
public:
    EditorUI(const string& _Name);
    virtual ~EditorUI();

public:
    virtual void Tick();
    virtual void Tick_UI() = 0;
    virtual void Activate() {}
    virtual void Deactivate() {}

private:
    void CheckFocus();

public:
    GET_SET(Vec2, SizeAsChild);
    GET_SET(string, UIName);
    
    __forceinline void SetSaperator(bool _Set);
    __forceinline void SetModal(bool _Modal);

    __forceinline bool IsActive() const;
    __forceinline void SetActive(bool _Active);

    __forceinline void AddChildUI(Ptr<EditorUI> _Child);

    __forceinline Ptr<EditorUI> GetParentUI() const;

protected:
    void SetUIKey(const string& _Key) { m_UIKey = _Key; }    

private:
    string m_UIName;
    string m_UIKey;

    bool m_IsModal;
    bool m_Separator;
    bool m_Active;

    EditorUI* m_Parent;
    vector<Ptr<EditorUI>> m_ChildUI;

    Vec2 m_SizeAsChild; // 자식 UI 인 경우, 담당하는 영역 크기
};

typedef void(EditorUI::* DELEGATE_0)(void);
typedef void(EditorUI::* DELEGATE_1)(DWORD_PTR);
typedef void(EditorUI::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);

#include "EditorUI.inl"