#include "WorldNode.h"
#include "NodeCache.h"

WorldNode::WorldNode() : WorldNode(1, XMINT3(0, 0, 0))
{
}

WorldNode::WorldNode(unsigned int size, XMINT3 pos)
{
	Initialize(size, pos);
}

WorldNode::~WorldNode()
{
	//if (!in_delete_queue)
	//	SDELETEARRAY(children, 8);

	SDELETE(underlying_chunk);
	for (size_t i = 0; i < 8; i++)
	{
		assert(children[i] == 0);
	}
}

void WorldNode::Initialize(unsigned int size, XMINT3 pos)
{
	this->node_stage = 0;
	this->size = size;
	this->position = pos;
	this->underlying_chunk = 0;
	this->parent = 0;
	this->is_leaf = true;
	this->is_empty = false;
	this->is_active = false;
	this->grouped_children = false;
	this->request_abandon = false;
	for (int i = 0; i < 8; i++)
		children[i] = 0;
}

int64_t WorldNode::Hash()
{
	return Hash(position, size);
}

int64_t WorldNode::Hash(XMINT3 position, unsigned int size)
{
	assert(sizeof(double) == sizeof(int64_t));

	float c_x = (float)position.x + 1.0f / (float)size;
	float c_y = (float)position.y + 1.0f / (float)size;
	float c_z = (float)position.z + 1.0f / (float)size;

	const float p1 = 73856093.0f;
	const float p2 = 19349663.0f;
	const float p3 = 83492791.0f;

	double a = c_x * p1;
	double b = c_y * p2;
	double c = c_z * p3;

	int64_t i = *reinterpret_cast<int64_t*>(&a);
	int64_t j = *reinterpret_cast<int64_t*>(&b);
	int64_t k = *reinterpret_cast<int64_t*>(&c);

	return i ^ j ^ k;
}

void WorldNode::SetAsLeaf()
{
}

void WorldNode::Divide(NodeCache* cache)
{
	is_leaf = false;

	int child_size = size / 2;
	WorldNode* child_pool = 0;

	bool cached_found = false;
	for (size_t i = 0; i < 8; i++)
	{
		XMINT3 child_pos = XMINT3(position.x + i / 4 * child_size, position.y + i % 4 / 2 * child_size, position.z + i % 2 * child_size);
		WorldNode* cached_node = cache->Find(Hash(child_pos, child_size), child_pos, child_size);
		if (cached_node)
		{
			// If we found the cached node, just assign it. There's no need to process it further because it's either being processed in the pipeline already, or it's ready.
			cached_node->request_abandon = false;
			cached_node->parent = this;
			children[i] = cached_node;
			cached_found = true;
		}
	}

	if (!cached_found)
	{
		//child_pool = new WorldNode[8];
		//this->grouped_children = true;
	}

	for (int i = 0; i < 8; i++)
	{
		if (children[i])
		{
			children[i]->parent = this;
			assert(children[i]->GetStage() != 0);
			continue;
		}

		XMINT3 child_pos = XMINT3(position.x + i / 4 * child_size, position.y + i % 4 / 2 * child_size, position.z + i % 2 * child_size);

		if (!cached_found)
		{
			// We set the node stage to CREATION here to ensure the node doesn't divide before having its set to generate.
			WorldNode* child = new WorldNode(child_size, child_pos);
			child->node_stage = NODE_STAGE_CREATION;
			child->parent = this;
			children[i] = child;
			//children[i] = child_pool + i;
		}
		else
		{
			WorldNode* child = new WorldNode(child_size, child_pos);
			child->node_stage = NODE_STAGE_CREATION;
			child->parent = this;
			children[i] = child;
		}
	}
}

bool WorldNode::Group()
{
	is_leaf = true;
	return false;
}

void WorldNode::GetLeaves(std::vector<TerrainChunk*>& collection)
{
	if (is_leaf && underlying_chunk)
	{
		collection.push_back(underlying_chunk);
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		if (children[i])
		{
			children[i]->GetLeaves(collection);
		}
	}
}

void WorldNode::DeleteChildren()
{
	for (int i = 0; i < 8; i++)
	{
		if (children[i])
		{
			children[i]->DeleteChildren();
			delete children[i];
			children[i] = 0;
		}
	}
}

WorldNode* WorldNode::GetNodeFromPos(Vector3 pos, bool prefer_parent)
{
	return GetNodeFromPos(XMINT3((int)pos.x, (int)pos.y, (int)pos.z), prefer_parent);
}

WorldNode* WorldNode::GetNodeFromPos(XMINT3 source, bool prefer_parent)
{
	if (source.x < position.x || source.y < position.y || source.z < position.z || source.x >= position.x + size || source.y >= position.y + size || source.z >= position.z + size)
		return 0;
	if (underlying_chunk)
		return this;

	XMINT3 difference = XMINT3(source.x - position.x, source.y - position.y, source.z - position.z);
	int dx = difference.x / (size / 2);
	int dy = difference.y / (size / 2);
	int dz = difference.z / (size / 2);
	int index = dx * 4 + dy * 2 + dz;
	if (children[index])
	{
		if (children[index]->IsLeaf() && prefer_parent)
			return this;
		return children[index]->GetNodeFromPos(source);
	}
	return this;
}

Vector3 WorldNode::GetCenter()
{
	Vector3 p((float)(position.x + size / 2), (float)(position.y + size / 2), (float)(position.z + size / 2));
	return p;
}
