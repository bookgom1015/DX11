#pragma once

#include "CLight2D.h"

class Shadow {
public:
	static const UINT ArraySize = 32;

public:
	Shadow();
	virtual ~Shadow();

public:
	int Init();
	void Apply(const vector<Ptr<CLight2D>>& lights);

public:
	ID3D11ShaderResourceView* GetSrv() { return mSrv.Get(); }

private:
	int Compile();
	int BuildResources();
	int BuildPipelines();

	void CalcSpotLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs);
	void CalcLineLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs);
	void CalcPointLight(Ptr<CLight2D> light, const std::vector<Ptr<GameObject>>& objs);

public:
	static Shadow* sShadow;

private:
	ComPtr<ID3D11VertexShader> mShadowVS{};
	ComPtr<ID3D11GeometryShader> mShadowGS{};
	ComPtr<ID3D11PixelShader> mShadowPS{};
	ComPtr<ID3D11InputLayout> mLayout{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState{};
	Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState{};

	ComPtr<ID3D11Texture2D> mDepthMapArray{};
	std::array<ComPtr<ID3D11DepthStencilView>, ArraySize> mDsvs{};
	ComPtr<ID3D11ShaderResourceView> mSrv{};

	D3D11_VIEWPORT mViewport{};
};