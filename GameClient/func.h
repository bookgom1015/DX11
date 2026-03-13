#pragma once

class Component;

// Task
void CreateObject(class GameObject* _Object, int LayerIdx);

void DestroyObject(class GameObject* _Object);

namespace Util {
	void ChangeLevel(const wstring& _NextLevelName);

    // 64-bit FNV-1a
    constexpr uint64_t HashString(string_view str) {
        uint64_t hash = 14695981039346656037ull; // offset basis
        for (char c : str) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
            hash *= 1099511628211ull; // prime
        }
        return hash;
    }

    // wchar_t 버전
    constexpr uint64_t HashWString(wstring_view str) {
        uint64_t hash = 14695981039346656037ull;
        for (wchar_t c : str) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ull;
        }
        return hash;
    }

    static SimpleRect MakeRect(const Vec2& a, const Vec2& b) {
        SimpleRect r;
        r.Min.x = min(a.x, b.x);
        r.Min.y = min(a.y, b.y);
        r.Max.x = max(a.x, b.x);
        r.Max.y = max(a.y, b.y);
        return r;
    }

    static bool Intersects(const SimpleRect& a, const SimpleRect& b) {
        if (a.Max.x <= b.Min.x || a.Min.x >= b.Max.x) return false;
        if (a.Max.y <= b.Min.y || a.Min.y >= b.Max.y) return false;
        return true;
    }

    static bool Intersects(const Vec2& a, const SimpleRect& b) {
        if (a.x <= b.Min.x || a.x >= b.Max.x) return false;
        if (a.y <= b.Min.y || a.y >= b.Max.y) return false;
        return true;
    }

    static float LengthSq(const Vec2& v) {
        return v.x * v.x + v.y * v.y;
    }

    static bool MouseInRect(Vec2 pos, Vec2 min, Vec2 max) {
        return pos.x >= min.x && pos.x <= max.x && pos.y >= min.y && pos.y <= max.y;
    }
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
    wstring GetComponentTypeName(Type _Type);
    Type GetComponentType(const wstring& _Name);

    Component* GetComponent(Type _Type);
}