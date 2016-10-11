#pragma once

#include <d3d11.h>
#include "General.h"
#include "Engine.h"
#include "ConstantsBuffer.h"
#include "ShaderConstants.h"

using namespace DirectX;

class Camera
{
public:

	XMFLOAT3 Position;
	XMFLOAT3 Target;

	Camera(float aspect_ratio);
	~Camera();

	void SetPosition(XMFLOAT3&, XMFLOAT3&);
	XMMATRIX GetWorld();
	XMMATRIX GetView();
	XMMATRIX GetProjection();
	void SetWorld(XMMATRIX& m);
	void SetView(XMMATRIX& m);
	void SetProjection(XMMATRIX& m);
	void SetProjection(float aspectRatio);

	bool UpdateFirstPerson(bool update = false);
	void SetShaderConstants();

	inline ConstantsBuffer* GetTerrainBuffer() { return terrain_buffer; }

private:
	ConstantsBuffer* terrain_buffer;
	BasicShaderConstants terrain_constants;
	
	XMVECTOR VectorPosition;
	XMVECTOR VectorTarget;
	XMVECTOR VectorUp;

	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	XMMATRIX Rotation;
	float rotationX;
	float rotationY;

	float speed_multiplier;
};

