#pragma once

// Task
void CreateObject(class GameObject* _Object, int LayerIdx);
namespace Util {
	void ChangeLevel(const wstring& _NextLevelName);
}
void ChangeLevelState(ELevelState::Type _NextState);


// DebugRender
void DrawDebugRect(Vec3 _Pos, Vec3 _Scale, Vec3 _Rot, Vec4 _Color, float _Duration, bool _DepthTest = false);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, float _Duration, bool _DepthTest = false);
void DrawDebugCircle(Vec3 _Pos, float _Radius, Vec4 _Color, float _Duration, bool _DepthTest = false);

void SaveWString(FILE* _File, const wstring& _String);
wstring LoadWString(FILE* _File);

void SaveAssetRef(FILE* _File, class Asset* _Asset);

// 0 ~ 1 로 제한
float Saturate(float _Data);

decltype(auto) GetTImeStamp();

wstring MakeUniqueName(const wstring& name);

UINT CeilDivide(UINT value, UINT divisor);

inline std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();

    int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
    std::wstring result(size, 0);

    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], size);
    return result;
}
#ifndef StrToWStr
#define StrToWStr(x) StringToWString(x)
#endif // StrToWStr

inline std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);

    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &result[0], size, nullptr, nullptr);
    return result;
}
#ifndef WStrToStr
#define WStrToStr(x) WStringToString(x)
#endif // WStrToStr

namespace ELevelLayer {
    wstring GetLevelLayerName(ELevelLayer::Type type);
}

namespace EComponent {
    wstring GetComponentTypeName(Type type);
}