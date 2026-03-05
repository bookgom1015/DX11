#ifndef __DEVICE_INL__
#define __DEVICE_INL__

Vec2 Device::GetRenderResolution() const { return m_RenderResol; }
ComPtr<ID3D11Device> Device::GetDevice() const { return m_Device; }
ComPtr<ID3D11DeviceContext>	Device::GetContext() const { return	m_Context; }
Ptr<ConstBuffer> Device::GetCB(CB_TYPE _Type) const { return m_CB[(UINT)_Type]; }

ComPtr<ID3D11RasterizerState> Device::GetRSState(ERasterizerState::Type _Type) const { return m_RSState[_Type]; }
ComPtr<ID3D11DepthStencilState> Device::GetDSState(EDepthStencilState::Type _Type) const { return m_DSState[(UINT)_Type]; }
ComPtr<ID3D11BlendState> Device::GetBSState(EBlendState::Type _Type) const { return m_BSState[(UINT)_Type]; }

ID3D11RenderTargetView* Device::GetRenderTargetView() const { return m_RTV.Get(); }

ID3D11Texture2D* Device::GetScene() const { return m_Scene.Get(); }
ID3D11RenderTargetView* Device::GetScene_Rtv() const { return m_Scene_Rtv.Get(); }
ID3D11ShaderResourceView* Device::GetScene_Srv() const { return m_Scene_Srv.Get(); }

ID3D11Texture2D* Device::GetHDR() const { return m_HDR.Get(); }
ID3D11RenderTargetView* Device::GetHDR_Rtv() const { return m_HDR_Rtv.Get(); }
ID3D11ShaderResourceView* Device::GetHDR_Srv() const { return m_HDR_Srv.Get(); }

ID3D11Texture2D* Device::GetHDR_Copy() const { return m_HDR_Copy.Get(); }
ID3D11RenderTargetView* Device::GetHDR_Copy_Rtv() const { return m_HDR_Copy_Rtv.Get(); }
ID3D11ShaderResourceView* Device::GetHDR_Copy_Srv() const { return m_HDR_Copy_Srv.Get(); }

ID3D11Texture2D* Device::GetRenderTarget() const { return m_RenderTarget.Get(); }

ID3D11Texture2D* Device::GetRenderTargetCopy() const { return m_RenderTargetCopy.Get(); }
ID3D11ShaderResourceView* Device::GetRenderTargetCopySrv() const { return m_RenderTargetCopySrv.Get(); }

ID3D11DepthStencilView* Device::GetDSV() const { return m_DSV.Get(); }

#endif // __DEVICE_INL__