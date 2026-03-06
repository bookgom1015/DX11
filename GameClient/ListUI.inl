#ifndef __LISTUI_INL__
#define __LISTUI_INL__

void ListUI::AddString(const string& _Str) { 
    m_vecList.push_back(_Str); 
}

void ListUI::AddString(const wstring& _WStr) {
    m_vecList.push_back(string(_WStr.begin(), _WStr.end())); 
}

void ListUI::AddString(const vector<string>& _vecStr) { 
    m_vecList.insert(m_vecList.end(), _vecStr.begin(), _vecStr.end()); 
}

void ListUI::AddString(const vector<wstring>& _vecWStr) {
    for (size_t i = 0; i < _vecWStr.size(); ++i)
        AddString(_vecWStr[i]);
}

void ListUI::AddDelegate(EditorUI* _Inst, DELEGATE_1 _MemFunc) { 
    m_Inst = _Inst; m_MemFunc = _MemFunc; 
}

const string& ListUI::GetSelectedString() const { return m_SelectedString; }

#endif // __LISTUI_INL__