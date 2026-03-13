#pragma once

class Vignette {
public:
	Vignette();
	virtual ~Vignette();

public:
	int Init();
	void Apply();

private:
	int Compile();

private:
	ComPtr<ID3D11VertexShader> mVignetteVS{};
	ComPtr<ID3D11PixelShader> mVignettePS{};
};