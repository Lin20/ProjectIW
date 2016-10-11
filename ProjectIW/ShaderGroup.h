#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

class ShaderGroup
{
public:
	ShaderGroup(const WCHAR* vertex_name = 0, const WCHAR* pixel_name = 0, const WCHAR* geometry_name = 0);
	~ShaderGroup();

	inline ID3D11VertexShader* GetVertexShader() { return vertex_shader; }
	inline ID3D11PixelShader* GetPixelShader() { return pixel_shader; }
	inline ID3D11GeometryShader* GetGeometryShader() { return geometry_shader; }

	bool LoadShader(const WCHAR* vertex_name = 0, const WCHAR* pixel_name = 0, const WCHAR* geometry_name = 0);
	inline bool IsValid() { return valid; }
	inline ID3D11InputLayout* GetInputLayout() { return input_layout; }

	bool CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, int num_elements);

private:
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11GeometryShader* geometry_shader;

	ID3D11InputLayout* input_layout;
	ID3DBlob* vertex_bytecode;
	bool valid;
};

