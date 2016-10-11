#include "ShaderGroup.h"
#include "Window.h"

ShaderGroup::ShaderGroup(const WCHAR* vertex_name, const WCHAR* pixel_name, const WCHAR* geometry_name)
{
	valid = LoadShader(vertex_name, pixel_name, geometry_name);
}

ShaderGroup::~ShaderGroup()
{
	if (vertex_shader)
	{
		vertex_shader->Release();
		vertex_shader = 0;
	}
	if (pixel_shader)
	{
		pixel_shader->Release();
		pixel_shader = 0;
	}
	if (geometry_shader)
	{
		geometry_shader->Release();
		geometry_shader = 0;
	}
	if (input_layout)
	{
		input_layout->Release();
		input_layout = 0;
	}
	if (vertex_bytecode)
	{
		vertex_bytecode->Release();
		vertex_bytecode = 0;
	}
}

bool ShaderGroup::LoadShader(const WCHAR* vertex_name, const WCHAR* pixel_name, const WCHAR* geometry_name)
{
	vertex_shader = 0;
	pixel_shader = 0;
	geometry_shader = 0;
	input_layout = 0;
	vertex_bytecode = 0;

	if (!vertex_name && !pixel_name && !geometry_name)
		return false;
		
	if (vertex_name)
	{
		HRESULT hr = D3DReadFileToBlob(vertex_name, &vertex_bytecode);
		if (FAILED(hr))
			return false;

		hr = engine->rendering->Device->CreateVertexShader(vertex_bytecode->GetBufferPointer(), vertex_bytecode->GetBufferSize(), 0, &vertex_shader);
		if (FAILED(hr))
			return false;
	}

	if (pixel_name)
	{
		ID3DBlob* pixel_bytecode;
		HRESULT hr = D3DReadFileToBlob(pixel_name, &pixel_bytecode);
		if (FAILED(hr))
			return false;

		hr = engine->rendering->Device->CreatePixelShader(pixel_bytecode->GetBufferPointer(), pixel_bytecode->GetBufferSize(), 0, &pixel_shader);
		if (FAILED(hr))
			return false;
	}

	if (geometry_name)
	{
		ID3DBlob* geometry_bytecode;
		HRESULT hr = D3DReadFileToBlob(geometry_name, &geometry_bytecode);
		if (FAILED(hr))
			return false;

		hr = engine->rendering->Device->CreateGeometryShader(geometry_bytecode->GetBufferPointer(), geometry_bytecode->GetBufferSize(), 0, &geometry_shader);
		if (FAILED(hr))
			return false;
	}

	return true;
}

bool ShaderGroup::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, int num_elements)
{
	HRESULT hr = engine->rendering->Device->CreateInputLayout(layout, num_elements, vertex_bytecode->GetBufferPointer(), vertex_bytecode->GetBufferSize(), &input_layout);
	if (FAILED(hr))
	{
		input_layout = 0;
		return false;
	}

	return true;
}
