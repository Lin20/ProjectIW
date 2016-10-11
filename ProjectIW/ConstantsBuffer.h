#pragma once

#include <d3d11.h>
#include "Engine.h"

class ConstantsBuffer
{
public:

	ConstantsBuffer();
	~ConstantsBuffer();

	HRESULT Create(UINT size, void* init_data = 0);
	void SetData(void* data, UINT offsetInBytes, UINT length);
	void BindToShader(unsigned int index);
	void BindToPSShader(unsigned int index);

	inline ID3D11Buffer* GetUnderlyingBuffer() { return underlying_buffer; }

private:
	ID3D11Buffer* underlying_buffer;
	unsigned int size;
};

