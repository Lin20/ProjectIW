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
	void UpdateTree();
	bool NodeNeedsSplit(WorldNode* n, Vector3& center);
	bool NodeNeedsGroup(WorldNode* n, Vector3& center);
	void ProcessInitQueue();
	void Scene::ProcessDeleteQueue();

	ShaderGroup* terrain_shaders;
	Camera* camera;
	Vector3 last_update_pos;

	WorldNode* base_tree;
	std::vector<TerrainChunk*> chunks;

	bool active;
	ThreadPool* thread_pool;
	std::mutex init_lock;
	std::mutex delete_lock;
	std::thread init_thread;
	std::thread delete_thread;
	std::vector<WorldNode*> init_queue;
	std::vector<WorldNode*> delete_queue;
};