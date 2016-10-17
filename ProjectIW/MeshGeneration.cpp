#include <cassert>
#include "MeshGeneration.h"
#include "WorldNode.h"

#define NUM_THREADS 4

ThreadPool MeshGeneration::threadpool(NUM_THREADS);
std::atomic<bool> MeshGeneration::shutdown = false;

void MeshGeneration::AddNode(WorldNode* node)
{
	assert(node->GetStage() == NODE_STAGE_CREATION || node->GetStage() == NODE_STAGE_MESH_WAITING);
	Task t = Task([node]()
	{
		MeshGeneration::ProcessNode(node);
	}, nullptr, TaskPriority::Low);

	threadpool.AddTask(t);
}

void MeshGeneration::ProcessNode(WorldNode* node)
{
	// TODO: reuse terrain chunk for regenerating mesh
	assert(node != 0);
	assert(node->underlying_chunk == 0);
	
	if (!shutdown && !node->request_abandon)
	{
		node->SetStage(NODE_STAGE_MESH_GENERATING);

		if (node->IsLeaf())
		{
			unsigned int scale = (unsigned int)log2(node->size / NODE_CHUNK_RESOLUTION);
			unsigned int mul = (unsigned int)pow(2, scale);

			TerrainChunk* chunk = new TerrainChunk(node->position, node->size / NODE_CHUNK_RESOLUTION);
			chunk->GenerateOctree();
			chunk->GenerateMesh();

			node->is_empty = chunk->IsEmpty();
			node->underlying_chunk = chunk;
		}
	}
	node->SetStage(NODE_STAGE_READY);
}

void MeshGeneration::Quit()
{
	threadpool.Quit();
}
