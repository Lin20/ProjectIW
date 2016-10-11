#pragma once

#include <d3d11.h>
#include <vector>
#include "General.h"
#include "Window.h"

class IBO
{
public:

	IBO(void* data, unsigned int size, bool modifiable);
	~IBO();

	void SetData(void* data, UINT offset, UINT length);
	void Bind();
	void Dispose();

	inline ID3D11Buffer* GetBuffer() { return buffer; }
	inline UINT GetBufferSize() { return buffer_size; }

private:
	UINT buffer_size;
	ID3D11Buffer* buffer = 0;

	bool CreateBuffer(void*, bool modifyable);
};

