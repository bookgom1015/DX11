#pragma once


struct Vertex
{
	Vec3	vPos;	// 3D 공간상에서 정점의 위치
	Vec2	vUV;	// 정점위에 이미지를 띄울 경우, 이미지의 어느 지점을 참고하는지 좌표
	Vec4	vColor; // 정점 자체의 색상
};
typedef Vertex Vtx;


// DebugInfo : 디버그 렌더링 요청사항 정보
struct DbgInfo
{
	DBG_SHAPE	Shape;

	Vec3		Pos;
	Vec3		Scale;
	Vec3		Rotation;

	Matrix		matWorld;

	Vec4		Color;
	float		Age;		
	float		Life;		

	bool		DepthTest;
};

// TaskMgr 가 처리할 작업 단위
struct TaskInfo
{
	TASK_TYPE Type;
	DWORD_PTR Param_0;
	DWORD_PTR Param_1;
	DWORD_PTR Param_2;
};


struct TransformMatrix
{
	Matrix	matWorld;
	Matrix  matView;
	Matrix	matProj;
};
// extern, 전방선언, 유일한 전역변수, 모든파일(cpp) 에서 접근 가능
extern TransformMatrix g_Trans;

struct MtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	mat[2];
};

