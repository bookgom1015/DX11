#ifndef __ATEXTURE_INL__
#define __ATEXTURE_INL__

float ATexture::GetWidth() const { return (float)m_Desc.Width; }
float ATexture::GetHeight() const { return (float)m_Desc.Height; }

ComPtr<ID3D11RenderTargetView> ATexture::GetRTV() const { return m_RTV; }
ComPtr<ID3D11DepthStencilView> ATexture::GetDSV() const { return m_DSV; }
ComPtr<ID3D11ShaderResourceView> ATexture::GetSRV() const { return m_SRV; }

#endif // __ATEXTURE_INL__