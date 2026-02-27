#pragma once

#include "UploadBuffer.h"

class Blur {
public:
	Blur();
	virtual ~Blur();

public:
	int Init();
	void Apply(
		ID3D11ShaderResourceView* pInputSrv,
		ID3D11UnorderedAccessView* pOutputUav,
		UINT width, UINT height);

private:
	int Compile();

private:
	ComPtr<ID3D11ComputeShader> mBlurCS{};

	UploadBuffer<BlurCB> mBlurCB{};
};