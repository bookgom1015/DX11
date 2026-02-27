#pragma once

class GammaCorrection {
public:
	GammaCorrection();
	virtual ~GammaCorrection();

public:
	int Init();
	void Apply();

private:
	int Compile();

private:
	ComPtr<ID3D11VertexShader> mGammaCorrectionVS{};
	ComPtr<ID3D11PixelShader> mGammaCorrectionPS{};
};