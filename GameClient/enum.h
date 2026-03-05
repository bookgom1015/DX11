#pragma once

namespace ELevelLayer {
	enum Type {
		E_Default = 0,
		E_Light,
		E_Player,
		E_Enemy,
		E_Ground,
		E_Projectile,
		E_Particle,
		E_Background,
		Count
	};

	static string GetName(ELevelLayer::Type type) {
		switch (type) {
		case ELevelLayer::E_Default: return "Default";
		case ELevelLayer::E_Player: return "Player";
		case ELevelLayer::E_Light: return "Light";
		case ELevelLayer::E_Enemy: return "Enemy";
		case ELevelLayer::E_Ground: return "Ground";
		case ELevelLayer::E_Projectile: return "Projectile";
		case ELevelLayer::E_Particle: return "Particle";
		case ELevelLayer::E_Background: return "Background";
		default: return "Undefined";
		}
	}
}

namespace EToneMapper {
	static const char* TypeNames[] = {
		"ACES",
		"Horror",
		"Anime"
	};

	enum Type {
		E_ACES = 0,
		E_Horror,
		E_Anime,
		Count
	};
}

namespace ELevelState {
	enum Type {
		E_Playing = 0,
		E_Paused,
		E_Stopped,
		Count
	};
}

namespace EGizmoAxis {
	enum Type { 
		None, 
		MoveXY, 
		MoveX, 
		MoveY 
	};
}

namespace ERasterizerState {
	enum Type {
		E_CullBack,	
		E_CullFront,
		E_CullNone,  
		E_Wireframe, 
		Count,
	};
}

namespace EDepthStencilState {
	enum Type {
		E_Less,
		E_LessEqual,
		E_Never,
		E_NeverWrite,
		Count
	};
}

namespace EBlendState {
	enum Type {
		E_Default,
		E_AlphaBlend,
		E_Additive,
		Count
	};
}

enum class SHADER_PARAM {
	INT,
	FLOAT,
	VEC2,
	VEC4,
	MAT,
	TEX,
};

enum class CB_TYPE
{
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2

	END,
};

enum class ASSET_TYPE
{
	MESH,
	MATERIAL,
	TEXTURE,
	SOUND,
	GRAPHICSHADER,
	COMPUTESHADER,
	LEVEL,
	SPRITE,
	FLIPBOOK,
	TILEMAP,
	PREFAB,

	END,
};



enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,	
	COLLIDER2D,
	COLLIDER3D,
	LIGHT2D,
	LIGHT3D,

	MESHRENDER,
	BILLBOARD_RENDER,
	SPRITE_RENDER,
	FLIPBOOK_RENDER,
	PARTICLE_RENDER,	
	TILE_RENDER,

	RIGIDBODY,

	END,

	SCRIPT,	 
};

enum class DIR
{
	RIGHT,
	UP,
	FRONT,

	END,
};


enum TEX_PARAM
{
	TEX_0, // t0
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_END,
};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
};


enum class DBG_SHAPE
{
	RECT,
	CIRCLE,
	CUBE,
	SPHERE,
};


enum class TASK_TYPE
{
	CREATE_OBJECT,
	DESTROY_OBJECT,
	CHANGE_LEVEL,
	CHANGE_LEVEL_STATE
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,	// 방향성 - 멀리서부터 오는 광원에게 설정, 주로 전역광원(빛이 월드로 오는 방향이 모두 같다고 본다)
	POINT,			// 점광원 - 기본적인 광원, 한 점으로부터 주변으로 빛이 뻗어나감
	SPOT,			// 스포트 - 특수한 광원, 빛을 특정 방향으로 모아서 보내는 것으로 봄
	LINE,
	RECT
};

// 재질이 렌더링 되는 시점
enum class RENDER_DOMAIN
{
	DOMAIN_OPAQUE,		// 불투명
	DOMAIN_MASKED,		// 불투명 + 완전 투명
	DOMAIN_TRANSPARENT, // 반투명
	DOMAIN_POSTPROCESS, // 후처리 - 가장 마지막에 동작, 기존에 그려진 장면을 재 가공
	DOMAIN_DEBUG,		// 디버그 렌더링에 사용하는 재질
	DOMAIN_NONE,		// 미지정
};

static string GetRenderDomainName(RENDER_DOMAIN domain) {
	switch (domain) {
	case RENDER_DOMAIN::DOMAIN_OPAQUE: return "Opaque";
	case RENDER_DOMAIN::DOMAIN_MASKED: return "Masked";
	case RENDER_DOMAIN::DOMAIN_TRANSPARENT: return "Transparent";
	case RENDER_DOMAIN::DOMAIN_POSTPROCESS: return "Post Process";
	case RENDER_DOMAIN::DOMAIN_DEBUG: return "Debug";
	case RENDER_DOMAIN::DOMAIN_NONE: return "None";
	default: return "Unknown";
	}
}

enum class PROJ_TYPE {
	ORTHOGRAPHIC,   // 직교투영
	PERSPECTIVE,    // 원근투영
};