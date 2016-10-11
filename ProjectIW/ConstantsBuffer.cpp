#include "ConstantsBuffer.h"

ConstantsBuffer::ConstantsBuffer()
{
	underlying_buffer = 0;
}

ConstantsBuffer::~ConstantsBuffer()
{
	if (underlying_buffer)
		underlying_buffer->Release();
}

HRESULT ConstantsBuffer::Create(UINT size, void* data)
{
	ID3D11Device* device = engine->rendering->Device;
	if (!device)
		return -1;

	this->size = size;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	if (!data)
		return device->CreateBuffer(&desc, NULL, &underlying_buffer);

	D3D11_SUBRESOURCE_DATA r;
	r.pSysMem = data;
	r.SysMemPitch = 0;
	r.SysMemSlicePitch = 0;
	return device->CreateBuffer(&desc, &r, &underlying_buffer);
}

void ConstantsBuffer::SetData(void* data, UINT offset, UINT length)
{
	ID3D11DeviceContext* context = engine->rendering->ImmediateContext;
	if (!context)
		return;

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = context->Map(underlying_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, length);
	context->Unmap(underlying_buffer, 0);
}

void ConstantsBuffer::BindToShader(unsigned int index)
{
	ID3D11DeviceContext* context = engine->rendering->ImmediateContext;
	if (!context)
		return;

	context->VSSetConstantBuffers(index, 1, &underlying_buffer);
}

void ConstantsBuffer::BindToPSShader(unsigned int index)
{
	ID3D11DeviceContext* context = engine->rendering->ImmediateContext;
	if (!context)
		return;

	context->PSSetConstantBuffers(index, 1, &underlying_buffer);
}
