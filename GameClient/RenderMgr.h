#pragma once

#include "GameObject.h"
#include "CCamera.h"
#include "StructuredBuffer.h"
#include "ConstBuffer.h"

class GammaCorrection;
class ToneMapping;
class Shadow;
class Bloom;
class Blur;

namespace ToneMapper {
	enum Type {
		E_ACES = 0,
		E_Horror,
		E_Anime,
		Count
	};
}

static const char* TonemapperTypeNames[] = {
	"ACES",
	"Horror",
	"Anime"
};

class RenderMgr
	: public singleton<RenderMgr>
{
	SINGLE(RenderMgr);
private:
	Ptr<CCamera>			m_MainCam;
	Ptr<GameObject>			m_DbgObj;			// 디버그 렌더링을 위한 Dummy GameObject
	list<DbgInfo>			m_DbgInfoList;		// 디버그 요청 정보

	vector<Ptr<CLight2D>>	m_vecLight2D;		// 레벨 안에있는 모든 광원
	Ptr<StructuredBuffer>	m_Light2DBuffer;	// 광원의 데이터를 입력받을 구조화버퍼

	bool					m_bDebugRender;		// 디버그 렌더 기능 On / Off

public:
	void RegisterCamera(Ptr<CCamera> _Cam) { m_MainCam = _Cam; }
	Ptr<CCamera> GetPOVCamera() { return m_MainCam; }
	void AddDebugInfo(const DbgInfo& _Info)  {  if(m_bDebugRender) m_DbgInfoList.push_back(_Info); }
	void RegisterLight2D(Ptr<CLight2D> _Light2D) { m_vecLight2D.push_back(_Light2D); }

	int GetLightCount() const { return static_cast<int>(m_vecLight2D.size()); }

private:
	void Render_Start();
	void Render_End();
	void Render_Debug();
	void Render_Post();

	void UpdateLightInfos(std::vector<Light2DInfo>& infos);

	void ApplyBloom();
	void ApplyToneMapping();
	void ApplyGammaCorrection();

	void ApplyShadow();

public:
	void Init();
	void Progress();

public:
	static bool GammaEnabled;

	static bool ToneEnabled;
	static ToneMapper::Type ToneType;

	static bool BloomEnabled;

private:
	std::unique_ptr<Shadow> mShadow{};
	std::unique_ptr<GammaCorrection> mGamma{};
	std::unique_ptr<ToneMapping> mTone{};
	std::unique_ptr<Bloom> mBloom{};
	std::unique_ptr<Blur> mBlur{};

	Ptr<ConstBuffer> mCB;
};

