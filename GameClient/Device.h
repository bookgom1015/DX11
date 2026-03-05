#pragma once

#include "ConstBuffer.h"

// Directx11 라이브러리 함수를 이용해서 
// GPU 를 제어할 수 있는 클래스
class Device : public singleton<Device> {
	SINGLE(Device);

public:
	int Init(HWND _hwnd, Vec2 _Resolution);
	void ClearTarget();
	void OMSetTarget();
	void ResetTarget();
	void Present();

public:
	__forceinline Vec2 GetRenderResolution() const;
	__forceinline ComPtr<ID3D11Device> GetDevice() const;
	__forceinline ComPtr<ID3D11DeviceContext> GetContext() const;
	__forceinline Ptr<ConstBuffer> GetCB(CB_TYPE _Type) const;

	__forceinline ComPtr<ID3D11RasterizerState> GetRSState(ERasterizerState::Type _Type)  const;
	__forceinline ComPtr<ID3D11DepthStencilState> GetDSState(EDepthStencilState::Type _Type) const;
	__forceinline ComPtr<ID3D11BlendState> GetBSState(EBlendState::Type _Type) const;

	__forceinline ID3D11RenderTargetView* GetRenderTargetView() const;

	__forceinline ID3D11Texture2D* GetScene() const;
	__forceinline ID3D11RenderTargetView* GetScene_Rtv() const;
	__forceinline ID3D11ShaderResourceView* GetScene_Srv() const;

	__forceinline ID3D11Texture2D* GetHDR() const;
	__forceinline ID3D11RenderTargetView* GetHDR_Rtv() const;
	__forceinline ID3D11ShaderResourceView* GetHDR_Srv() const;

	__forceinline ID3D11Texture2D* GetHDR_Copy() const;
	__forceinline ID3D11RenderTargetView* GetHDR_Copy_Rtv() const;
	__forceinline ID3D11ShaderResourceView* GetHDR_Copy_Srv() const;

	__forceinline ID3D11Texture2D* GetRenderTarget() const;

	__forceinline ID3D11Texture2D* GetRenderTargetCopy() const;
	__forceinline ID3D11ShaderResourceView* GetRenderTargetCopySrv() const;

	__forceinline ID3D11DepthStencilView* GetDSV() const;

private:
	int CreateSwapChain();
	int CreateBuffer();
	int CreateSampler();
	int CreateRasterizerState();
	int CreateDepthStencilState();
	int CreateBlendState();
	void CreateConstBuffer();

private:
	ComPtr<ID3D11Device>				m_Device;			// GPU 메모리 관리
	ComPtr<ID3D11DeviceContext>			m_Context;			// GPU 제어 및 렌더링, GPGPU(General Purpose) - ComputeShader 명령

	ComPtr<IDXGISwapChain>				m_SwapChain;
	ComPtr<ID3D11Texture2D>				m_RenderTarget;
	ComPtr<ID3D11Texture2D>				m_DepthStencilTarget;

	ComPtr<ID3D11Texture2D>				m_Scene;
	ComPtr<ID3D11Texture2D>				m_RenderTargetCopy;

	ComPtr<ID3D11Texture2D>				m_HDR;
	ComPtr<ID3D11Texture2D>				m_HDR_Copy;

	ComPtr<ID3D11RenderTargetView>		m_RTV;
	ComPtr<ID3D11DepthStencilView>		m_DSV;

	ComPtr<ID3D11ShaderResourceView>	m_RenderTargetCopySrv;

	ComPtr<ID3D11RenderTargetView>		m_Scene_Rtv;
	ComPtr<ID3D11ShaderResourceView>	m_Scene_Srv;

	ComPtr<ID3D11RenderTargetView>		m_HDR_Rtv;
	ComPtr<ID3D11ShaderResourceView>	m_HDR_Srv;

	ComPtr<ID3D11RenderTargetView>		m_HDR_Copy_Rtv;
	ComPtr<ID3D11ShaderResourceView>	m_HDR_Copy_Srv;

	ComPtr<ID3D11SamplerState>			m_arrSam[2];

	ComPtr<ID3D11RasterizerState>		m_RSState[ERasterizerState::Count];
	ComPtr<ID3D11DepthStencilState>		m_DSState[EDepthStencilState::Count];
	ComPtr<ID3D11BlendState>			m_BSState[EBlendState::Count];

	HWND								m_hWnd;		   // 출력 윈도우
	Vec2								m_RenderResol; // 렌더타겟 해상도

	Ptr<ConstBuffer>					m_CB[(UINT)CB_TYPE::END];
};

#define DEVICE	Device::GetInst()->GetDevice().Get()
#define CONTEXT Device::GetInst()->GetContext().Get()

#include "Device.inl"