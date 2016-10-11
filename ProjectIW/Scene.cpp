#include "Scene.h"
#include "VertexPositionColorNormal.h"
#include "TerrainChunk.h"
#include "WorldNode.h"
#include "ShaderLoader.h"
#include "ShaderGroup.h"
#include "Camera.h"
#include "Task.h"

#define LODThreshold 3.0f

Scene::Scene()
{
	terrain_shaders = 0;
	camera = 0;
	base_tree = 0;
	active = true;
}

Scene::~Scene()
{
	active = false;
	init_thread.join();
	delete_thread.join();
	if (terrain_shaders)
	{
		delete terrain_shaders;
		terrain_shaders = 0;
	}
	if (camera)
	{
		camera->~Camera();
		_aligned_free(camera);
		camera = 0;
	}
	SDELETE(base_tree);
	SDELETE(thread_pool);
}

bool Scene::Setup()
{
	if (!SetupShaders())
		return false;

	thread_pool = new ThreadPool(8);
	init_thread = std::thread(&Scene::ProcessInitQueue, this);
	delete_thread = std::thread(&Scene::ProcessDeleteQueue, this);

	void* v = _aligned_malloc(sizeof(Camera), 16);
	this->camera = new (v)Camera((float)BUFFER_WIDTH / (float)BUFFER_HEIGHT);
	last_update_pos = Vector3(0, 0, 0);

	unsigned int size = (unsigned int)pow(2, 10);
	Vector3 center((float)size / 2, (float)size * 2, (float)size / 2);
	center.x = center.z = 0;
	center.y = 0;
	chunks.clear();
	base_tree = new WorldNode(size, XMINT3(-(int)size / 2, -(int)size / 2, -(int)size / 2));
	UpdateTree();
	/*base_tree->Divide(center);
	base_tree->GetLeaves(chunks);*/

	/*for (int i = 0; i < NUM_CHUNKS; i++)
	{
		int mult = (int)pow(2, i);
		chunks[i] = new TerrainChunk(XMINT3(mult * 64, 0, 0), mult);
		chunks[i]->GenerateOctree();
		chunks[i]->GenerateMesh();
	}*/

	return true;
}

bool Scene::SetupShaders()
{
	terrain_shaders = new ShaderGroup(L"VertexShader.cso", L"PixelShader.cso", L"GeometryShader.cso");
	if (!terrain_shaders->IsValid())
		return false;
	if (!terrain_shaders->CreateInputLayout(MDC::VertexPositionColorNormal::layout, MDC::VertexPositionColorNormal::num_elements))
		return false;


	return true;
}





void Scene::Update()
{
	if (engine->input->CurrentKeyOnly(DIK_C) || true)
	{
		UpdateTree();
		chunks.clear();
		base_tree->GetLeaves(chunks);
	}
	if (engine->input->CurrentKeyOnly(DIK_R))
	{
		delete base_tree;
		unsigned int size = (unsigned int)pow(2, 16);
		base_tree = new WorldNode(size, XMINT3(-(int)size / 2, 0, -(int)size / 2));
		UpdateTree();
		chunks.clear();
		base_tree->GetLeaves(chunks);
		int x = min(3, 2);
	}
	if (engine->input->CurrentKeyOnly(DIK_T))
	{
		UpdateTree();
		chunks.clear();
		base_tree->GetLeaves(chunks);
	}
}

void Scene::UpdateTree()
{
	Vector3 center = Vector3(camera->Position);
	if ((int)center.x == (int)last_update_pos.x && (int)center.y == (int)last_update_pos.y && (int)center.z == (int)last_update_pos.z)
	{
		last_update_pos = center;
		return;
	}
	last_update_pos = center;

	std::vector<WorldNode*> update_list;
	std::vector<WorldNode*> init_list;
	update_list.push_back(base_tree);

	for (unsigned int i = 0; i < update_list.size(); i++)
	{
		WorldNode* node = update_list[i];
		if (!node)
			continue;

		if (NodeNeedsSplit(node, center))
		{
			if (!node->IsEmpty())
			{
				node->Divide(center, false);
				//for (int i = 0; i < 8; i++)
				//	update_list.push_back(node->children[i]);
			}
		}
		else if (NodeNeedsGroup(node, center))
		{
			bool all_empty = true;
			for (int i = 0; i < 8; i++)
			{
				if (node->children[i])
				{
					if (all_empty && !node->IsEmpty())
						all_empty = false;
					if (!node->children[i]->garbage)
					{
						node->children[i]->garbage = true;
						if (!node->children[i]->in_delete_queue)
						{
							node->children[i]->in_delete_queue = true;
							delete_lock.lock();
							delete_queue.push_back(node->children[i]);
							delete_lock.unlock();
						}
					}
					node->children[i] = 0;
				}
			}
			if (!all_empty)
			{
				node->initialized = false;
				node->is_empty = false;
				init_list.push_back(node);
			}
			else
			{
				node->initialized = true;
				node->is_leaf = true;
				node->is_empty = true;
				node->underlying_chunk = 0;
			}
		}
		else if (!node->initialized)
		{
			init_list.push_back(node);
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				if (node->children[i] && !node->children[i]->in_delete_queue)
					update_list.push_back(node->children[i]);
			}
		}
	}

	for (auto const& node : init_list)
	{
		if (!node || node->garbage || node->in_init_queue || node->in_delete_queue)
			continue;
		/*if (node->parent)
		{
			if (node->parent->initialized && node->parent->is_leaf)
			{
				node->garbage = true;
				node->initialized = true;
			}
		}*/

		//node->SetAsLeaf();
		node->in_init_queue = true;
		init_lock.lock();
		init_queue.push_back(node);
		init_lock.unlock();
	}

	/*for (auto const& node : garbage_list)
	{
		if (node->in_init_queue)
		{
			if (!node->in_delete_queue)
			{
				node->in_delete_queue = true;
				delete_queue.push_back(node);
			}
			continue;
		}
		delete node;
	}*/

	chunks.clear();
	base_tree->GetLeaves(chunks);
}

void Scene::ProcessInitQueue()
{
	while (active)
	{
		unsigned int count = init_queue.size();
		if (count == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}

		unsigned int index = 0;
		for (unsigned int i = 0; i < count; i++)
		{
			init_lock.lock();
			WorldNode* node = init_queue[index];
			init_lock.unlock();

			if (node->garbage)
			{
				node->in_init_queue = false;
				if (!node->in_delete_queue)
				{
					node->in_delete_queue = true;
					delete_lock.lock();
					delete_queue.push_back(node);
					delete_lock.unlock();
				}
				init_lock.lock();
				init_queue.erase(init_queue.begin() + index);
				init_lock.unlock();
				continue;
			}

			Task t = Task([node]()
			{
				node->SetAsLeaf();
			},
				[node]()
			{
				node->in_init_queue = false;
			},
				TaskPriority::Medium);
			thread_pool->AddTask(t);

			init_lock.lock();
			init_queue.erase(init_queue.begin() + index);
			init_lock.unlock();
		}
	}
}

void Scene::ProcessDeleteQueue()
{
	while (active)
	{
		unsigned int count = delete_queue.size();
		if (count == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue;
		}

		unsigned int index = 0;
		for (unsigned int i = 0; i < count; i++)
		{
			delete_lock.lock();
			WorldNode* node = delete_queue[index];
			delete_lock.unlock();

			if (!node->in_init_queue && node->initialized)
			{
				delete node;
				delete_lock.lock();
				delete_queue.erase(delete_queue.begin() + index);
				delete_lock.unlock();
				continue;
			}
			index++;
		}
	}
}



bool Scene::NodeNeedsSplit(WorldNode* n, Vector3& center)
{
	if ((n->IsLeaf() || !n->initialized) && n->size > 16 && Vector3::Distance(n->GetCenter(), center) / n->size < LODThreshold)
		return true;

	return false;
}

bool Scene::NodeNeedsGroup(WorldNode* n, Vector3& center)
{
	if (!n->IsLeaf() && Vector3::Distance(n->GetCenter(), center) / n->size >= LODThreshold)
		return true;

	return false;
}



void Scene::Render()
{
	ID3D11DeviceContext* context = engine->rendering->ImmediateContext;

	camera->UpdateFirstPerson();
	camera->SetShaderConstants();

	context->VSSetShader(terrain_shaders->GetVertexShader(), 0, 0);
	context->GSSetShader(terrain_shaders->GetGeometryShader(), 0, 0);
	context->PSSetShader(terrain_shaders->GetPixelShader(), 0, 0);

	unsigned int count = chunks.size();
	for (unsigned int i = 0; i < count; i++)
	{
		TerrainChunk* octree = chunks[i];
		if (!octree->GetVBO() || !octree->GetIBO())
			continue;
		octree->GetIBO()->Bind();
		octree->GetVBO()->Bind(0);
		context->IASetInputLayout(terrain_shaders->GetInputLayout());
		camera->GetTerrainBuffer()->BindToShader(0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(octree->GetTriangleCount(), 0, 0);
	}
}
