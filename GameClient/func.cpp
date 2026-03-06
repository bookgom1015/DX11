#include "pch.h"

#include "RenderMgr.h"
#include "TaskMgr.h"

namespace {
	wchar_t Buff[255]{};
}

void CreateObject(GameObject* _Object, int LayerIdx) {
    TaskInfo info{};

    info.Type = ETask::E_CreateObject;
    info.Param_0 = (DWORD_PTR)_Object;
    info.Param_1 = LayerIdx;

    TaskMgr::GetInst()->AddTask(info);
}

void Util::ChangeLevel(const wstring& _NextLevelName) {
	TaskInfo info{};

	wcscpy_s(Buff, 255, _NextLevelName.c_str());
	
	info.Type = ETask::E_ChangeLevel;
	info.Param_0 = (DWORD_PTR)Buff;	

	TaskMgr::GetInst()->AddTask(info);
}

void ChangeLevelState(ELevelState::Type _NextState) {
    TaskInfo info{};

	info.Type = ETask::E_ChangeLevelState;
	info.Param_0 = (DWORD_PTR)_NextState;

	TaskMgr::GetInst()->AddTask(info);
}

void DrawDebugRect(
    Vec3 _Pos, Vec3 _Scale, Vec3 _Rot, Vec4 _Color
    , float _Duration, bool _DepthTest) {
    DbgInfo info{};

    info.Shape = EDebugShape::E_Rect;

    info.Pos = _Pos;
    info.Scale = _Scale;
    info.Rotation = _Rot;

    info.matWorld = XMMatrixIdentity();

    info.Color = _Color;
    info.Age = 0.f;
    info.Life = _Duration;

    info.DepthTest = _DepthTest;
    
    RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugRect(
    const Matrix& _matWorld
    , Vec4 _Color
    , float _Duration, bool _DepthTest) {
    DbgInfo info{};

    info.Shape = EDebugShape::E_Rect;

    info.matWorld = _matWorld;

    info.Color = _Color;
    info.Age = 0.f;
    info.Life = _Duration;

    info.DepthTest = _DepthTest;

    RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugCircle(
    Vec3 _Pos
    , float _Radius, Vec4 _Color
    , float _Duration, bool _DepthTest) {
    DbgInfo info{};
    info.Shape = EDebugShape::E_Circle;
    info.Pos = _Pos;
    info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, 0.f);
    info.Rotation = Vec3(0.f, 0.f, 0.f);
    info.Color = _Color;
    info.Age = 0.f;
    info.Life = _Duration;
    info.DepthTest = _DepthTest;

    RenderMgr::GetInst()->AddDebugInfo(info);
}

void SaveWString(FILE* _File, const wstring& _String) {   
    int Len = static_cast<int>(_String.length());
    fwrite(&Len, sizeof(int), 1, _File);        
    fwrite(_String.data(), sizeof(wchar_t), Len, _File);
}

wstring LoadWString(FILE* _File) {
    int Len = 0;
    fread(&Len, sizeof(int), 1, _File);

    wchar_t buff[255] = {};
    fread(buff, sizeof(wchar_t), Len, _File);

    return buff;
}

void SaveAssetRef(FILE* _File, Asset* _Asset) {
    // Asset 이 Null 인지 아닌지 저장
    bool IsNull = _Asset;
    fwrite(&IsNull, sizeof(bool), 1, _File);
     
    // Asset 의 Key, RelativePath 저장
    if (nullptr != _Asset) {
        SaveWString(_File, _Asset->GetKey());
        SaveWString(_File, _Asset->GetRelativePath());
    }
}

bool IsValid(Ptr<GameObject>& _Object) {
    if (nullptr == _Object || _Object->IsDead()) {
        _Object = nullptr;
        return false;
    }     
        
    return true;
}

float Saturate(float _Data) {
    if (1.f < _Data) return 1.f;
    else if (_Data < 0.f) return 0.f;
    else return _Data;
}

decltype(auto) GetTImeStamp() {
    return chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now().time_since_epoch()).count();
}

wstring MakeUniqueName(const wstring& name) {
    return std::format(L"{}##{}", name, GetTImeStamp());
}

UINT CeilDivide(UINT value, UINT divisor) {
    return (value + divisor - 1) / divisor;
}