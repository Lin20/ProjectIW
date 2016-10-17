#include <chrono>
#include "WorldTree.h"
#include "MeshGeneration.h"
#include "World.h"

#define DEFAULT_BUFFER_SIZE 65536

WorldTree::WorldTree(unsigned int default_res)
{
	World::global_size = (float)default_res;
	World::global_size_inverse = 1.0f / (float)default_res;
	this->shutdown = false;
	this->default_res = default_res;
	this->renderables.reserve(DEFAULT_BUFFER_SIZE);
	this->output_buffer.reserve(DEFAULT_BUFFER_SIZE);
	Setup();
}

WorldTree::~WorldTree()
{
	assert(tree.IsLeaf());

}

void WorldTree::Shutdown()
{
	shutdown = true;
}

/// <summary>
/// Process is as follows:
/// -Create the octree
/// -Move the main node right to the MeshGeneration stage
/// After the main mesh is created, the update stage should pick it up as a node to be divided, just like any normal node.
/// </summary>
void WorldTree::Setup()
{
	tree.Initialize(this->default_res, XMINT3(-(int)(this->default_res / 2), -(int)(this->default_res / 2), -(int)(this->default_res / 2)));
	tree.SetStage(NODE_STAGE_MESH_WAITING);
	tree.SetActive(true);
	renderables.push_back(&tree);

	MeshGeneration::AddNode(&tree);
}

unsigned int WorldTree::Update(Vector3 camera_pos)
{
	using namespace std::chrono;
	steady_clock::time_point start = high_resolution_clock::now();

	output_buffer.clear();
	//renderables.clear();

	//temp_process_queue.push_back(&tree);
	IterateTree(camera_pos);

	microseconds elapsed = duration_cast<microseconds>(high_resolution_clock::now() - start);
	return (unsigned int)elapsed.count();
}


void WorldTree::IterateTree(Vector3 camera_pos)
{
	WorldNode* n = 0;
	unsigned int index = 0;
	size_t size = renderables.size();

	for (size_t i = 0; i < size; i++)
	{
		WorldNode* n = renderables[i];
		assert(n != 0);
		assert(n->IsActive());

		// This is only true if the parent node was grouped and this node is being processed afterwards for some reason
		/*if (!n->IsActive())
		{
			renderables.erase(renderables.begin() + i--);
			size--;
			continue;
		}*/

		//assert(n->IsActive());
		// Get the stage and make sure we stick with it throughout the process here
		int stage = n->GetStage().load();

		if (n->IsLeaf())
		{
			if (n->parent && stage == NODE_STAGE_READY && NodeNeedsGrouping(n->parent, camera_pos))
			{
				n->SetActive(false);
				renderables.erase(renderables.begin() + i--);
				if (!n->parent->IsActive())
					AddNodeToRenderables(n->parent);
				else
					size--;
				assert(size == renderables.size());
				continue;
				//size++;
			}

			// If the node doesn't have ready mesh, there's nothing further to do with it
			if (stage != NODE_STAGE_READY && stage != NODE_STAGE_SPLITTABLE)
			{
				continue;
			}

			// Check if the node is in a restful state and if it needs to be divided
			if (stage == NODE_STAGE_READY && NodeNeedsDivision(n, camera_pos))
			{
				DirtyNodeOutput dirty;
				dirty.node = n;
				dirty.type = NODE_OUTPUT_DIVIDE;

				// We set the stage to splittable because it's ready and to-be enqueued in the NodeCreation stage
				n->SetStage(NODE_STAGE_SPLITTABLE);
				output_buffer.push_back(dirty);
			}

			// The node has mesh, so render it
			//AddNodeToRenderables(n);
		}
		else if (stage == NODE_STAGE_READY) // Node is not a leaf
		{
			if (NodeNeedsGrouping(n, camera_pos))
			{
				if (stage == NODE_STAGE_READY)
				{
					DirtyNodeOutput dirty;
					dirty.node = n;
					dirty.type = NODE_OUTPUT_GROUP;
					bool can_group = true;

					for (size_t i = 0; i < 8; i++)
					{
						if (n->children[i])
						{
							if (n->children[i]->IsActive())
							{
								can_group = false;
								break;
							}
							//n->children[i]->SetActive(false);
							//assert(!n->children[i]->IsActive());
							//n->children[i]->SetActive(false);
						}
					}
					if (!can_group)
						continue;

					// We set the stage to splittable because it's ready and to-be enqueued in the NodeCreation stage
					n->SetStage(NODE_STAGE_SPLITTABLE);
					output_buffer.push_back(dirty);
				}

				// At this point, the node has to be the highest possible level to be collapsed, so it's safe to add it to the renderables.
				//AddNodeToRenderables(n);
				//size++;

				continue;
			}

			bool all_renderable = true;
			for (size_t i = 0; i < 8; i++)
			{
				WorldNode* child = n->children[i];
				assert(child != 0);

				int child_stage = child->GetStage();
				if (child_stage != NODE_STAGE_READY && child_stage != NODE_STAGE_SPLITTABLE)
				{
					all_renderable = false;
					break;
				}
			}

			if (all_renderable)
			{
				n->SetActive(false);
				renderables.erase(renderables.begin() + i--);
				for (size_t i = 0; i < 8; i++)
				{
					WorldNode* child = n->children[i];
					assert(child != 0);

					AddNodeToRenderables(child);
					//renderables.push_back(child);
				}
				size += 7;
			}
			else
			{
				assert(n->IsActive());
				/*if (!n->IsActive()) // Should not ever be false
				{
					AddNodeToRenderables(n);
					size++;
				}*/
			}
		}
	}
}

bool WorldTree::NodeNeedsDivision(WorldNode* n, Vector3 camera_pos)
{
	if (shutdown)
		return false;
	assert(n != 0);
	assert(n->IsLeaf());

	if (n->size <= NODE_CHUNK_RESOLUTION)
		return false;

	// We use raw floats here because Vector3 causes resource contention
	float n_x = (float)(n->position.x + n->size / 2) - camera_pos.x;
	float n_y = (float)(n->position.y + n->size / 2) - camera_pos.y;
	float n_z = (float)(n->position.z + n->size / 2) - camera_pos.z;

	float distance = sqrtf(n_x * n_x + n_y * n_y + n_z * n_z);
	return distance < (float)(n->size * 2);
}

bool WorldTree::NodeNeedsGrouping(WorldNode* n, Vector3 camera_pos)
{
	assert(n != 0);
	assert(!n->IsLeaf());

	if (n->GetStage() != NODE_STAGE_READY)
		return false;

	//Vector3 node_center = Vector3((float)(n->position.x + n->size / 2), (float)(n->position.y + n->size / 2), (float)(n->position.z + n->size / 2));

	if (!shutdown)
	{
		// We use raw floats here because Vector3 causes resource contention
		float n_x = (float)(n->position.x + n->size / 2) - camera_pos.x;
		float n_y = (float)(n->position.y + n->size / 2) - camera_pos.y;
		float n_z = (float)(n->position.z + n->size / 2) - camera_pos.z;

		float distance = sqrtf(n_x * n_x + n_y * n_y + n_z * n_z);
		if (!(distance >= (float)(n->size) * 2.5f))
			return false;
	}

	for (size_t i = 0; i < 8; i++)
	{
		int stage = n->children[i]->GetStage();
		if ((n->children[i] && !n->children[i]->IsLeaf()) || stage == NODE_STAGE_CREATION || stage == NODE_STAGE_SPLITTABLE)
			return false;
	}

	return true;
}

void WorldTree::AddNodeToRenderables(WorldNode* n)
{
	assert(n != 0);
	assert(!n->IsActive());

	n->SetActive(true);
	renderables.push_back(n);
}
