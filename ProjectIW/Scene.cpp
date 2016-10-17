#include "Scene.h"
#include "VertexPositionColorNormal.h"
#include "TerrainChunk.h"
#include "WorldNode.h"
#include "ShaderLoader.h"
#include "ShaderGroup.h"
#include "Camera.h"
#include "Task.h"

Scene::Scene() : world((int)pow(2, 16))
{
	terrain_shaders = 0;
	camera = 0;
}

Scene::~Scene()
{
	world.Cleanup();
	MeshGeneration::Quit();

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
}

bool Scene::Setup()
{
	if (!SetupShaders())
		return false;

	void* v = _aligned_malloc(sizeof(Camera), 16);
	this->camera = new (v)Camera((float)BUFFER_WIDTH / (float)BUFFER_HEIGHT);
	last_update_pos = Vector3(0, 0, 0);

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
	UpdateWorld();
}

void Scene::UpdateWorld()
{
	Vector3 center = Vector3(camera->Position);
	if ((int)center.x == (int)last_update_pos.x && (int)center.y == (int)last_update_pos.y && (int)center.z == (int)last_update_pos.z)
	{
		last_update_pos = center;
		//return;
	}
	last_update_pos = center;

	world.Update(center);
}

void Scene::Render()
{
	ID3D11DeviceContext* context = engine->rendering->ImmediateContext;

	camera->UpdateFirstPerson();
	camera->SetShaderConstants();

	context->VSSetShader(terrain_shaders->GetVertexShader(), 0, 0);
	context->GSSetShader(terrain_shaders->GetGeometryShader(), 0, 0);
	context->PSSetShader(terrain_shaders->GetPixelShader(), 0, 0);

	auto& nodes = world.GetRenderables();
	for(auto n : nodes)
	{
		int stage = n->GetStage().load();
		if (stage != NODE_STAGE_READY && stage != NODE_STAGE_SPLITTABLE)
			continue;
		TerrainChunk* octree = n->underlying_chunk;
		if (!octree || !octree->GetVBO() || !octree->GetIBO())
			continue;
		octree->GetIBO()->Bind();
		octree->GetVBO()->Bind(0);
		context->IASetInputLayout(terrain_shaders->GetInputLayout());
		camera->GetTerrainBuffer()->BindToShader(0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(octree->GetTriangleCount(), 0, 0);
	}
}
