#pragma once

class Pixelization {
public:
	Pixelization();
	virtual ~Pixelization();

public:
	int Init();
	void Apply();

private:
	int Compile();

private:
	ComPtr<ID3D11VertexShader> mPixelizationVS{};
	ComPtr<ID3D11PixelShader> mPixelizationPS{};
};