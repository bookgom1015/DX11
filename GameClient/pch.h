#pragma once

// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#ifdef _DEBUG
#define CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#endif // _DEBUG

// Directx11 객체들을 가리킬 수 있는 스마트포인터
#include "wrl.h"
using namespace Microsoft::WRL;

// Directx11 라이브러리 (정적, 동적)
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#include "SimpleMath.h"
typedef SimpleMath::Vector2		Vec2;
typedef SimpleMath::Vector3		Vec3;
typedef SimpleMath::Vector4		Vec4;
typedef SimpleMath::Matrix		Matrix;

inline Vec3 Reflect(const Vec3& v, const Vec3& normal) {
    // normal은 반드시 normalize 되어 있어야 함
    return v - normal * (2.f * v.Dot(normal));
}

// DLL(동적 라이브러리)
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

// DirectxTex Library
#include <DirectxTex\\DirectXTex.h>
#ifdef _DEBUG
#pragma comment(lib, "DirectxTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectxTex\\DirectXTex.lib")
#endif

// C++ 타입 정보기능
#include <typeinfo>

// STL 자료구조
#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>

#include <string>
#include <format>
using namespace std;

#include <functional>

#include <chrono>

// 파일 시스템
#include <filesystem>

// 상대경로
#include "resource.h"
#include "global.h"
#include "Ptr.h"
#include "Random.h"




