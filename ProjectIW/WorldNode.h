#pragma once

#include <DirectXMath.h>
#include <vector>
#include "SimpleMath.h"
#include "General.h"
#include "TerrainChunk.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace MDC;

class WorldNode
{
public:
	WorldNode();
	WorldNode(unsigned int size, XMINT3 pos);
	~WorldNode();

	void SetAsLeaf();
	void Divide(Vector3 center, bool recursive = true);
	bool Group(Vector3 center, bool recursive = true);
	void GetLeaves(std::vector<TerrainChunk*>& collection);
	void DeleteChildren();
	WorldNode* GetNodeFromPos(Vector3 pos, bool prefer_parent = false);
	WorldNode* GetNodeFromPos(XMINT3 pos, bool prefer_parent = false);
	Vector3 GetCenter();

	inline bool IsLeaf() { return is_leaf; }
	inline bool IsEmpty() { return is_empty; }

	WorldNode* children[8];
	WorldNode* parent;
	int size;
	XMINT3 position;
	TerrainChunk* underlying_chunk;
	bool is_leaf;
	bool initialized;
	bool garbage;
	bool is_empty;
	bool in_init_queue;
	bool in_delete_queue;
};
