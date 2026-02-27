#pragma once

#include "Device.h"
#include "struct.h"

#ifndef ConstantBufferSize
#define ConstantBufferSize(__type) ((sizeof(__type) + 255) & ~255)
#endif

template <typename T>
class UploadBuffer {
public:
	UploadBuffer() = default;
	virtual ~UploadBuffer();

public:
	bool Initialize(UINT numElements);
	void CleanUp();

	void CopyData(const T& data, UINT index = 0);
	__forceinline ID3D11Buffer** CBAddress();

	bool BeginFrame();
	void EndFrame();

	bool SetData(const T& data);

	__forceinline constexpr UINT FirstConstant(UINT index = 0) const noexcept;
	__forceinline constexpr UINT NumConstants() const noexcept;

protected:
	ComPtr<ID3D11Buffer> mUploadBuffer{};
	BYTE* mpMappedData{};

	UINT mElementByteSize{};
};

#include "UploadBuffer.inl"