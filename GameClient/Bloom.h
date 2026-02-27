#pragma once

#include "UploadBuffer.h"

class Blur;

class Bloom {
public:
	enum Resource {
		E_4thRes = 0,
		E_16thRes,
		E_64thRes,
		E_256thRes,
		Count
	};

public:
	Bloom();
	virtual ~Bloom();

public:
	int Init();
	void Apply(
		Blur* pBlur,
		ID3D11Texture2D* pBackBuffer,
		ID3D11RenderTargetView* pBackBufferRtv,
		ID3D11Texture2D* pBackBufferCopy,
		ID3D11ShaderResourceView* pBackBufferCopySrv,
		ID3D11ShaderResourceView* pInputSrv);

private:
	int Compile();
	int BuildResources();

	void DownScale(ID3D11ShaderResourceView* pInputSrv);
	void Blend(Blur* pBlur);
	void Apply(
		ID3D11Texture2D* pBackBuffer,
		ID3D11RenderTargetView* pBackBufferRtv,
		ID3D11Texture2D* pBackBufferCopy,
		ID3D11ShaderResourceView* pBackBufferCopySrv);

private:
	ComPtr<ID3D11ComputeShader> mDownScaleCS{};
	ComPtr<ID3D11ComputeShader> mBlendBloomCS{};
	ComPtr<ID3D11VertexShader> mApplyBloomVS{};
	ComPtr<ID3D11PixelShader> mApplyBloomPS{};

	std::array<ComPtr<ID3D11Texture2D>, Resource::Count> mDownScaleMaps{};
	std::array<ComPtr<ID3D11ShaderResourceView>, Resource::Count> mDowScaleMapSrvs{};
	std::array<ComPtr<ID3D11UnorderedAccessView>, Resource::Count> mDowScaleMapUavs{};

	std::array<ComPtr<ID3D11Texture2D>, Resource::Count> mResultMaps{};
	std::array<ComPtr<ID3D11ShaderResourceView>, Resource::Count> mResultMapSrvs{};
	std::array<ComPtr<ID3D11UnorderedAccessView>, Resource::Count> mResultMapUavs{};

	UploadBuffer<DownScaleCB> mDownScaleCB{};
	UploadBuffer<BlendBloomCB> mBlendBloomCB{};
};