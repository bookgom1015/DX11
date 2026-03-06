#pragma once

#include "EditorUI.h"

class ListUI : public EditorUI {
public:
    ListUI();
    virtual ~ListUI();

public:
    virtual void Tick_UI() override;
    virtual void Activate() override;
    virtual void Deactivate() override;

public:
    __forceinline void AddString(const string& _Str);
    __forceinline void AddString(const wstring& _WStr);
    __forceinline void AddString(const vector<string>& _vecStr);
    __forceinline void AddString(const vector<wstring>& _vecWStr);

    __forceinline void AddDelegate(EditorUI* _Inst, DELEGATE_1 _MemFunc);

    __forceinline const string& GetSelectedString() const;

private:
    vector<string> m_vecList;
    int m_SelectedIdx;
    string m_SelectedString;

    EditorUI* m_Inst;
    DELEGATE_1 m_MemFunc;
};

#include "ListUI.inl"