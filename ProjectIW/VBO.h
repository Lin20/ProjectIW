#pragma once

#include <d3d11.h>
#include <vector>
#include "General.h"
#include "Window.h"

class VBO
{
public:

	VBO(void* data, unsigned int size, unsigned int stride, bool modifiable);
	~VBO();

	UINT GetBufferSize();
	void SetData(void* data, UINT offset, UINT length);
	void Bind(unsigned int slot = 0);
	void Dispose();

	inline ID3D11Buffer* GetBuffer() { return buffer; }

private:
	UINT buffer_size;
	UINT stride;
	UINT numElements;
	ID3D11Buffer* buffer = 0;

	bool CreateBuffer(void*, bool modifyable);
};

