#pragma once
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <fstream>

using namespace std;

class ShaderLoader
{
public:
	ID3DBlob* ByteCode;
	ID3D10Blob* Error;
	ShaderLoader(LPCSTR, const char*, const char*);
	ShaderLoader(LPCWSTR);
	~ShaderLoader(void);

	void Compile(LPCSTR, const char*, const char*);
};

