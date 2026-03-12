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

namespace Util {
    void ChangeLevel(const wstring& _NextLevelName) {
        TaskInfo info{};

        wcscpy_s(Buff, 255, _NextLevelName.c_str());

        info.Type = ETask::E_ChangeLevel;
        info.Param_0 = (DWORD_PTR)Buff;

        TaskMgr::GetInst()->AddTask(info);
    }
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

wstring ELevelLayer::GetLevelLayerName(Type type) {
    switch (type) {
    case ELevelLayer::E_Default: return L"Default";
    case ELevelLayer::E_Player: return L"Player";
    case ELevelLayer::E_Light: return L"Light";
    case ELevelLayer::E_Enemy: return L"Enemy";
    case ELevelLayer::E_Ground: return L"Ground";
    case ELevelLayer::E_Projectile: return L"Projectile";
    case ELevelLayer::E_Particle: return L"Particle";
    case ELevelLayer::E_Background: return L"Background";
    default: assert(false && "Undefined Level Layer");
    }
}

namespace EComponent {
    wstring GetComponentTypeName(Type _Type) {
        switch (_Type) {
        case E_Transform: return L"CTransform";
        case E_Camera: return L"CCamera";
        case E_Collider2D: return L"CCollider2D";
        case E_Collider3D: return L"CCollider3D";
        case E_Light2D: return L"CLight2D";
        case E_Light3D: return L"CLight3D";
        case E_MeshRender: return L"CMeshRender";
        case E_BillboardRender: return L"CBillboardRender";
        case E_SpriteRender: return L"CSpriteRender";
        case E_FlipbookRender: return L"CFlipbookRender";
        case E_ParticleRender: return L"CParticleRender";
        case E_TileRender: return L"CTileRender";
        case E_Rigidbody: return L"CRigidbody";
        default: assert(false && "Undefined Component Type");
        }
    }

    Type GetComponentType(const wstring& _Name) {
        switch (Util::HashWString(_Name)) {
        case Util::HashWString(L"CTransform"): return EComponent::E_Transform;
        case Util::HashWString(L"CCamera"): return EComponent::E_Camera;
        case Util::HashWString(L"CCollider2D"): return EComponent::E_Collider2D;
        case Util::HashWString(L"CCollider3D"): return EComponent::E_Collider3D;
        case Util::HashWString(L"CLight2D"): return EComponent::E_Light2D;
        case Util::HashWString(L"CLight3D"): return EComponent::E_Light3D;
        case Util::HashWString(L"CMeshRender"): return EComponent::E_MeshRender;
        case Util::HashWString(L"CBillboardRender"): return EComponent::E_BillboardRender;
        case Util::HashWString(L"CSpriteRender"): return EComponent::E_SpriteRender;
        case Util::HashWString(L"CFlipbookRender"): return EComponent::E_FlipbookRender;
        case Util::HashWString(L"CParticleRender"): return EComponent::E_ParticleRender;
        case Util::HashWString(L"CTileRender"): return EComponent::E_TileRender;
        case Util::HashWString(L"CRigidbody"): return EComponent::E_Rigidbody;
        default: assert(false && "Undefined Component Type");
        }
    }

    Component* GetComponent(Type _Type) {
        switch (_Type) {
        case E_Transform: return NEW CTransform;
        case E_Camera: return NEW CCamera;
        case E_Collider2D: return NEW CCollider2D;
        case E_Collider3D: assert(false && "Not Implemented Component");
        case E_Light2D: return NEW CLight2D;
        case E_Light3D: assert(false && "Not Implemented Component");
        case E_MeshRender: return NEW CMeshRender;
        case E_BillboardRender: return NEW CBillboardRender;
        case E_SpriteRender: return NEW CSpriteRender;
        case E_FlipbookRender: return NEW CFlipbookRender;
        case E_ParticleRender: assert(false && "Not Implemented Component");
        case E_TileRender: return NEW CTileRender;
        case E_Rigidbody: return NEW CRigidBody;
        default: assert(false && "Undefined Component Type");
        }
    }
}