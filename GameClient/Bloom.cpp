#include "pch.h"
#include "Bloom.h"

#include "Device.h"
#include "AGraphicShader.h"

#include "Blur.h"

Bloom::Bloom() {}

Bloom::~Bloom() {}

int Bloom::Init() {
	if (FAILED(Compile())) return E_FAIL;
	if (FAILED(BuildResources())) return E_FAIL;

	if (!mDownScaleCB.Initialize(1)) return E_FAIL;
	if (!mBlendBloomCB.Initialize(1)) return E_FAIL;

	return S_OK;
}

void Bloom::Apply(
		Blur* pBlur,
		ID3D11Texture2D* pBackBuffer,
		ID3D11RenderTargetView* pBackBufferRtv,
		ID3D11Texture2D* pBackBufferCopy,
		ID3D11ShaderResourceView* pBackBufferCopySrv,
		ID3D11ShaderResourceView* pInputSrv) {
	DownScale(pInputSrv);
	Blend(pBlur);
	Apply(pBackBuffer, pBackBufferRtv, pBackBufferCopy, pBackBufferCopySrv);
}

int Bloom::Compile() {
	if (FAILED(AGraphicShader::CreateComputeShader(
		mDownScaleCS, L"Shader\\downscale.fx", "CS_DownScale")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreateComputeShader(
		mBlendBloomCS, L"Shader\\blend_bloom.fx", "CS_BlendBloom")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreateVertexShader(
		mApplyBloomVS, L"Shader\\apply_bloom.fx", "VS_Bloom")))
		return E_FAIL;
	if (FAILED(AGraphicShader::CreatePixelShader(
		mApplyBloomPS, L"Shader\\apply_bloom.fx", "PS_Bloom")))
		return E_FAIL;

	return S_OK;
}

int Bloom::BuildResources() {
	D3D11_TEXTURE2D_DESC desc{};
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC sr{};
	sr.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sr.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sr.Texture2D.MipLevels = 1;
	sr.Texture2D.MostDetailedMip = 0;

	D3D11_UNORDERED_ACCESS_VIEW_DESC ua{};
	ua.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	ua.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	ua.Texture2D.MipSlice = 0;

	XMUINT2 texDim = { ClientWidth, ClientHeight };

	for (size_t i = 0; i < Resource::Count; ++i) {
		texDim.x >>= 1;
		texDim.y >>= 1;

		desc.Width = texDim.x;
		desc.Height = texDim.y;

		if (FAILED(DEVICE->CreateTexture2D(
			&desc, nullptr, mDownScaleMaps[i].GetAddressOf())))
			return E_FAIL;
		if (FAILED(DEVICE->CreateShaderResourceView(
			mDownScaleMaps[i].Get(), &sr, mDowScaleMapSrvs[i].GetAddressOf())))
			return E_FAIL;
		if (FAILED(DEVICE->CreateUnorderedAccessView(
			mDownScaleMaps[i].Get(), &ua, mDowScaleMapUavs[i].GetAddressOf())))
			return E_FAIL;

		if (FAILED(DEVICE->CreateTexture2D(
			&desc, nullptr, mResultMaps[i].GetAddressOf())))
			return E_FAIL;
		if (FAILED(DEVICE->CreateShaderResourceView(
			mResultMaps[i].Get(), &sr, mResultMapSrvs[i].GetAddressOf())))
			return E_FAIL;
		if (FAILED(DEVICE->CreateUnorderedAccessView(
			mResultMaps[i].Get(), &ua, mResultMapUavs[i].GetAddressOf())))
			return E_FAIL;
	}

	return S_OK;
}

void Bloom::DownScale(ID3D11ShaderResourceView* pInputSrv) {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CSSetShader(mDownScaleCS.Get(), nullptr, 0);

	XMUINT2 texDim{ ClientWidth, ClientHeight };
	for (UINT i = 0; i < Resource::Count; ++i) {
		ID3D11UnorderedAccessView* uavs[] = { mDowScaleMapUavs[i].Get() };
		context->CSSetUnorderedAccessViews(0, _countof(uavs), uavs, nullptr);

		ID3D11ShaderResourceView* srv{};
		if (i == 0) srv = pInputSrv;
		else srv = mDowScaleMapSrvs[i - 1].Get();
		context->CSSetShaderResources(0, 1, &srv);

		DownScaleCB cb{};
		cb.gSrcTexDim[0] = texDim.x;
		cb.gSrcTexDim[1] = texDim.y;

		texDim.x >>= 1;
		texDim.y >>= 1;

		cb.gDstTexDim[0] = texDim.x;
		cb.gDstTexDim[1] = texDim.y;

		mDownScaleCB.SetData(cb);

		context->CSSetConstantBuffers(0, 1, mDownScaleCB.CBAddress());

		context->Dispatch(
			CeilDivide(cb.gSrcTexDim[0], 8),
			CeilDivide(cb.gSrcTexDim[1], 8),
			1);

		// Clear
		{
			ID3D11ShaderResourceView* nullSrvs[1] = {};
			context->CSSetShaderResources(0, _countof(nullSrvs), nullSrvs);

			ID3D11UnorderedAccessView* nullUavs[1] = {};
			context->CSSetUnorderedAccessViews(0, _countof(nullUavs), nullUavs, nullptr);

			context->CSSetConstantBuffers(0, 0, nullptr);
		}
	}

	// Clear
	{
		context->CSSetShader(nullptr, nullptr, 0);
	}
}

void Bloom::Blend(Blur* pBlur) {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	XMUINT2 texDim{ ClientWidth >> 3, ClientHeight >> 3 };

	pBlur->Apply(
		mDowScaleMapSrvs[Resource::E_256thRes].Get(),
		mResultMapUavs[Resource::E_256thRes].Get(),
		texDim.x, texDim.y);	

	auto highSampIndex = Resource::E_64thRes;
	auto lowSampIndex = Resource::E_256thRes;

	for (UINT i = 0, end = Resource::Count - 1; i < end; ++i) {
		ID3D11UnorderedAccessView* uavs[] = { mDowScaleMapUavs[highSampIndex].Get() };
		context->CSSetUnorderedAccessViews(0, _countof(uavs), uavs, nullptr);

		ID3D11ShaderResourceView* srv = mResultMapSrvs[lowSampIndex].Get();
		context->CSSetShaderResources(0, 1, &srv);

		BlendBloomCB cb{};
		cb.gInvTexDim = Vec2(1.f / texDim.x, 1.f / texDim.y);

		mBlendBloomCB.SetData(cb);

		context->CSSetConstantBuffers(0, 1, mBlendBloomCB.CBAddress());
		context->CSSetShader(mBlendBloomCS.Get(), nullptr, 0);

		context->Dispatch(
			CeilDivide(texDim.x, 8),
			CeilDivide(texDim.y, 8),
			1);

		// Clear
		{
			ID3D11ShaderResourceView* nullSrvs[1] = {};
			context->CSSetShaderResources(0, _countof(nullSrvs), nullSrvs);

			ID3D11UnorderedAccessView* nullUavs[1] = {};
			context->CSSetUnorderedAccessViews(0, _countof(nullUavs), nullUavs, nullptr);

			context->CSSetConstantBuffers(0, 0, nullptr);
			context->CSSetShader(nullptr, nullptr, 0);
		}

		pBlur->Apply(
			mDowScaleMapSrvs[highSampIndex].Get(),
			mResultMapUavs[highSampIndex].Get(),
			texDim.x, texDim.y);

		highSampIndex = (Resource)max<UINT>(static_cast<UINT>(highSampIndex) - 1, 0);
		lowSampIndex = (Resource)max<UINT>(static_cast<UINT>(lowSampIndex) - 1, 0);

		texDim.x <<= 1;
		texDim.y <<= 1;
	}
}

void Bloom::Apply(
		ID3D11Texture2D* pBackBuffer,
		ID3D11RenderTargetView* pBackBufferRtv,
		ID3D11Texture2D* pBackBufferCopy,
		ID3D11ShaderResourceView* pBackBufferCopySrv) {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CopyResource(pBackBufferCopy, pBackBuffer);

	context->OMSetRenderTargets(1, &pBackBufferRtv, nullptr);

	ID3D11ShaderResourceView* srvs[] = { 
		pBackBufferCopySrv,
		mDowScaleMapSrvs[Resource::E_4thRes].Get()	
	};
	context->PSSetShaderResources(0, _countof(srvs), srvs);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	context->VSSetShader(mApplyBloomVS.Get(), nullptr, 0);
	context->PSSetShader(mApplyBloomPS.Get(), nullptr, 0);

	context->Draw(6, 0);

	// Clear
	{
		context->OMSetRenderTargets(0, nullptr, nullptr);

		ID3D11ShaderResourceView* nullSrvs[2] = {};
		context->PSSetShaderResources(0, _countof(nullSrvs), nullSrvs);

		context->VSSetShader(nullptr, nullptr, 0);
		context->GSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);
	}
}