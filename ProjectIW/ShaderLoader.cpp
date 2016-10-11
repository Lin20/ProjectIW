#include "ShaderLoader.h"


ShaderLoader::ShaderLoader(LPCSTR filename, const char* start, const char* version)
{
	this->ByteCode = 0;
	Compile(filename, start, version);
}

ShaderLoader::ShaderLoader(LPCWSTR filename)
{
	HRESULT hr = D3DReadFileToBlob(filename, &ByteCode);
	if (FAILED(hr))
	{
		ByteCode = 0;
	}
	Error = 0;
}

ShaderLoader::~ShaderLoader(void)
{

}

void ShaderLoader::Compile(LPCSTR filename, const char* start, const char* version)
{
	D3DX11CompileFromFile(filename, 0, 0, start, version, 0, 0, 0, &ByteCode, &Error, 0);
}