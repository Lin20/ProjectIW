#include "WorldNode.h"

WorldNode::WorldNode() : WorldNode(1, XMINT3(0, 0, 0))
{
}

WorldNode::WorldNode(unsigned int size, XMINT3 pos) : size(size), position(pos), underlying_chunk(0), parent(0), is_leaf(true), garbage(false), initialized(false), is_empty(false), in_init_queue(false), in_delete_queue(false)
{
	initialized = false;
	in_delete_queue = false;
	for (int i = 0; i < 8; i++)
		children[i] = 0;
}

WorldNode::~WorldNode()
{
	//if (!in_delete_queue)
	//	SDELETEARRAY(children, 8);

	SDELETE(underlying_chunk);
}

void WorldNode::SetAsLeaf()
{
	is_leaf = true;
	if (underlying_chunk || in_delete_queue)
	{
		initialized = true;
		return;
	}

	unsigned int scale = (unsigned int)log2(size / 16);
	unsigned int mul = (unsigned int)pow(2, scale);
	underlying_chunk = new TerrainChunk(position, mul);
	underlying_chunk->GenerateOctree();
	underlying_chunk->GenerateMesh();
	is_empty = underlying_chunk->IsEmpty();
	initialized = true;
}

void WorldNode::Divide(Vector3 center, bool recursive)
{
	initialized = true;
	//SDELETE(underlying_chunk);
	is_leaf = false;
	int child_size = size / 2;
	for (int i = 0; i < 8; i++)
	{
		//SDELETE(children[i]);
		XMINT3 child_pos = XMINT3(position.x + i / 4 * child_size, position.y + i % 4 / 2 * child_size, position.z + i % 2 * child_size);
		children[i] = new WorldNode(child_size, child_pos);
		children[i]->parent = this;

		Vector3 child_center = Vector3(child_pos.x + child_size / 2, child_pos.y + child_size / 2, child_pos.z + child_size / 2);
		child_center -= center;
		float distance = child_center.Length();
		if (recursive && child_size > 16 && distance / (float)child_size < 1.0f)
		{
			children[i]->Divide(center);
		}
		else
		{
			//children[i]->SetAsLeaf();
		}
	}
}

bool WorldNode::Group(Vector3 center, bool recursive)
{
	int child_size = size / 2;
	Vector3 this_center = Vector3(position.x + child_size, position.y + child_size, position.z + child_size);
	this_center -= center;
	float distance = this_center.Length();
	if (distance / (float)child_size >= 1.0f)
	{
		bool is_leaf = IsLeaf();
		if (!is_leaf)
		{
			DeleteChildren();
		}
		else
			SDELETE(underlying_chunk);
		if ((is_leaf && parent) || (recursive && parent))
		{
			if (!parent->Group(center, recursive))
			{
				SetAsLeaf();
			}
		}

		return true;
	}

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
