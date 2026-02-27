#include "pch.h"
#include "Blur.h"

#include "Device.h"
#include "AGraphicShader.h"

Blur::Blur() {}

Blur::~Blur() {}

int Blur::Init() {
	if (FAILED(Compile())) return E_FAIL;

	if (!mBlurCB.Initialize(1)) return E_FAIL;

	return S_OK;
}

void Blur::Apply(
		ID3D11ShaderResourceView* pInputSrv,
		ID3D11UnorderedAccessView* pOutputUav,
		UINT width, UINT height) {
	auto device = Device::GetInst();
	auto context = device->GetContext();

	context->CSSetShader(mBlurCS.Get(), nullptr, 0);

	context->CSSetUnorderedAccessViews(0, 1, &pOutputUav, nullptr);
	context->CSSetShaderResources(0, 1, &pInputSrv);

	BlurCB cb{};
	cb.gBlurTexDim[0] = width;
	cb.gBlurTexDim[1] = height;

	mBlurCB.SetData(cb);

	context->CSSetConstantBuffers(0, 1, mBlurCB.CBAddress());

	context->Dispatch(
		CeilDivide(width, 8),
		CeilDivide(height, 8),
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
}

int Blur::Compile() {
	if (FAILED(AGraphicShader::CreateComputeShader(
		mBlurCS, L"Shader\\blur.fx", "CS_Blur")))
		return E_FAIL;

	return S_OK;
}