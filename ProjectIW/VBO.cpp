#include "VBO.h"

VBO::VBO(void* data, unsigned int size, unsigned int stride, bool modifiable)
{
	this->buffer = 0;
	this->buffer_size = size;
	this->stride = stride;
	this->numElements = numElements;
	CreateBuffer(data, modifiable);
}

VBO::~VBO()
{
	Dispose();
}

void VBO::Dispose()
{
	if (buffer)
	{
		buffer->Release();
		buffer = 0;
	}
}

bool VBO::CreateBuffer(void* data, bool modifyable)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = (!modifyable ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC);
	desc.ByteWidth = this->buffer_size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (!modifyable ? 0 : D3D11_CPU_ACCESS_WRITE);
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA resource;
	ZeroMemory(&resource, sizeof(D3D11_SUBRESOURCE_DATA));
	resource.pSysMem = data;

	HRESULT hr = engine->rendering->Device->CreateBuffer(&desc, &resource, &buffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Failed to create vertex buffer.", "Error", MB_ICONERROR);
		return false;
	}

	return true;
}

void VBO::SetData(void* data, UINT offset, UINT length)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	engine->rendering->ImmediateContext->Map(buffer, 0, D3D11_MAP_WRITE, 0, &resource);
	memcpy((void*)((char*)resource.pData + offset), data, length);
	engine->rendering->ImmediateContext->Unmap(buffer, 0);
}

void VBO::Bind(unsigned int slot)
{
	const UINT offset = 0;
	engine->rendering->ImmediateContext->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}