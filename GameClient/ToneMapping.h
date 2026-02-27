#pragma once

class ToneMapping {
public:
	ToneMapping();
	virtual ~ToneMapping();

public:
	int Init();
	void Apply();

private:
	int Compile();
	int BuildPipelines();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mToneMappingVS{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mToneMappingPS{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState{};
	Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState{};
};