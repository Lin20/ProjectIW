#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dinput.h>
#include <vector>
#include <memory>
#include <thread>
#include "General.h"
#include "SimpleMath.h"
#include "ThreadPool.h"
#include "WorldProcessor.h"

using namespace MDC;
using namespace DirectX::SimpleMath;

#define NUM_CHUNKS 8

class Scene
{
public:
	Scene();
	~Scene();

	bool Setup();
	void Update();
	void Render();

private:
	bool SetupShaders();
	void UpdateWorld();

	ShaderGroup* terrain_shaders;
	Camera* camera;
	Vector3 last_update_pos;

	WorldProcessor world;
};