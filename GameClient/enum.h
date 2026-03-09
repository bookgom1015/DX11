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

namespace ETask {
	enum Type {
		E_CreateObject,
		E_DestroyObject,
		E_ChangeLevel,
		E_ChangeLevelState,
		E_DeferredProcessing,
		Count
	};
}

namespace ELight {
	enum Type {
		E_Directional,
		E_Point,
		E_Spot,
		E_Line,
		E_Rect,
		Count
	};
}

namespace ERenderDomain {
	enum Type {
		E_Opaque,
		E_Masked,
		E_Transparent,
		E_PostProcess,
		E_Debug,
		E_None,
		Count
	};
}

namespace EDebugShape {
	enum Type {
		E_Rect,
		E_Circle,
		E_Cube,
		E_Sphere,
		Count
	};
}

namespace EProjection {
	enum Type {
		E_Orthographic,
		E_Perspective,
		Count
	};
}

namespace EAsset {
	enum Type {
		E_Mesh,
		E_Material,
		E_Texture,
		E_Sound,
		E_GraphicShader,
		E_ComputeShader,
		E_Level,
		E_Sprite,
		E_Flipbook,
		E_TileMap,
		E_Prefab,
		Count
	};
}

namespace EComponent {
	enum Type {
		E_None = -1,
		E_Transform = 0,
		E_Camera,
		E_Collider2D,
		E_Collider3D,
		E_Light2D,
		E_Light3D,
		E_MeshRender,
		E_BillboardRender,
		E_SpriteRender,
		E_FlipbookRender,
		E_ParticleRender,
		E_TileRender,
		E_Rigidbody,
		Count,
		E_Script
	};
}

namespace EGizmoState {
	enum Type {
		E_Trans,
		E_Rotate,
		E_Scale,
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

enum class CB_TYPE {
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2

	END,
};

enum class DIR {
	RIGHT,
	UP,
	FRONT,

	END,
};

enum TEX_PARAM {
	TEX_0, // t0
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_END,
};

enum SCALAR_PARAM {
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