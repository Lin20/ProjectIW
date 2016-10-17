#pragma once

#include <DirectXMath.h>
#include <vector>
#include <atomic>
#include <mutex>
#include "SimpleMath.h"
#include "General.h"
#include "TerrainChunk.h"
#include "WorldTypes.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace MDC;

class WorldNode
{
public:
	WorldNode();
	WorldNode(unsigned int size, XMINT3 pos);
	~WorldNode();

	void Initialize(unsigned int size, XMINT3 pos);
	
	/// <summary>
	/// Hashes this instance.
	/// </summary>
	/// <remarks>
	/// See http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf for details on the algorithm.
	///
	/// <returns>The hash of this node.</returns>
	int64_t Hash();

	/// <summary>
	/// Hashes an input 3D vector and size, which are expected from a node.
	/// </summary>
	/// <remarks>
	/// See http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf for details on the algorithm.
	///
	/// <returns>The hash of the position and size.</returns>
	static int64_t Hash(XMINT3 pos, unsigned int size);

	void SetAsLeaf();
	
	/// <summary>
	/// Divides the node and creates children, using the cache to find pre-generated nodes if possible.
	/// </summary>
	/// <param name="cache">The node cache containing previously-generated nodes.</param>
	void Divide(NodeCache* cache);

	bool Group();
	void GetLeaves(std::vector<TerrainChunk*>& collection);
	void DeleteChildren();
	WorldNode* GetNodeFromPos(Vector3 pos, bool prefer_parent = false);
	WorldNode* GetNodeFromPos(XMINT3 pos, bool prefer_parent = false);
	Vector3 GetCenter();

	inline std::atomic<bool>& IsLeaf() { return is_leaf; }
	inline std::atomic<int>& GetStage() { return node_stage; }
	inline void SetStage(int stage) { node_stage = stage; }
	inline bool IsActive() { return is_active; }
	inline void SetActive(bool b) { is_active = b; }

	int size;
	XMINT3 position;
	bool grouped_children;
	WorldNode* children[8];
	WorldNode* parent;
	std::atomic<bool> is_leaf;
	std::atomic<bool> request_abandon;
	bool is_empty;
	bool has_dirty_children;
	bool is_active;
	
	/// <summary>
	/// The node stage represents what stage in the pipeline it's in.
	/// See MeshTypes.WorldNodeStages for reference.
	/// TODO: possible change to an atomic enum after further research ensures they're treated as ints are
	/// </summary>
	std::atomic<int> node_stage;
	
	/// <summary>
	/// The underlying chunk containing the mesh.
	/// This shouldn't need locking because its access depends on what node_stage returns, which is lock-free and thread-safe.
	/// For example, underlying_chunk is only used for rendering if the stage is READY or SPLITTABLE, and the stage is only set to that after the chunk is assigned.
	/// </summary>
	TerrainChunk* underlying_chunk;
};
