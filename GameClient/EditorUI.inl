#ifndef __EDITORUI_INL__
#define __EDITORUI_INL__

void EditorUI::SetSaperator(bool _Set) { m_Separator = _Set; }

void EditorUI::SetModal(bool _Modal) { m_IsModal = _Modal; }

bool EditorUI::IsActive() const { return m_Active; }

void EditorUI::SetActive(bool _Active) {
    if (m_Active == _Active)
        return;

    m_Active = _Active;

    if (m_Active)
        Activate();
    else
        Deactivate();
}

void EditorUI::AddChildUI(Ptr<EditorUI> _Child) {
    _Child->m_Parent = this;
    m_ChildUI.push_back(_Child);
}

Ptr<EditorUI> EditorUI::GetParentUI() const { return m_Parent; }

#endif // __EDITORUI_INL__