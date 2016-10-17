#pragma once

#include <vector>
#include <queue>
#include <set>
#include "WorldTypes.h"
#include "WorldNode.h"

/// <summary>
/// This is the initial step in the world pipeline.
/// </summary>
class WorldTree
{
public:
	WorldTree(unsigned int default_res);
	~WorldTree();

	/// <summary>
	/// Cleans up. This is to be called manually.
	/// </summary>
	void Shutdown();

	/// <summary>
	/// Navigates through the tree and collects which nodes need dividing, need grouping, and are ready to be rendered.
	/// </summary>
	/// <param name="pos">The camera's position.</param>
	/// <returns>
	/// The amount of time in us it took.
	/// </returns>
	unsigned int Update(Vector3 camera_pos);

	inline std::vector<DirtyNodeOutput>& GetDirtyNodes() { return output_buffer; }
	inline std::vector<WorldNode*>& GetRenderables() { return renderables; }
	inline bool CanExit() { return tree.IsLeaf() && renderables.size() <= 1; }

private:
	/// <summary>
	/// Creates the octree.
	/// </summary>
	void Setup();

	/// <summary>
	/// Iterates through tree and builds up a list of nodes that need to be divided, need to be grouped, or can be rendered.
	/// </summary>
	/// <param name="camera_pos">The camera position.</param>
	void IterateTree(Vector3 camera_pos);

	/// <summary>
	/// Determines whether or not a node needs to be divided.
	/// </summary>
	/// <param name="n">The node to check.</param>
	/// <param name="pos">The camera's position.</param>
	/// <returns>True if node needs to be divided, false if otherwise.</returns>
	bool NodeNeedsDivision(WorldNode* n, Vector3 camera_pos);

	/// <summary>
	/// Determines whether or not a node needs to be grouped.
	/// </summary>
	/// <param name="n">The node to check.</param>
	/// <param name="pos">The camera's position.</param>
	/// <returns>True if the node needs to be grouped, false if otherwise.</returns>
	bool NodeNeedsGrouping(WorldNode* n, Vector3 camera_pos);

	/// <summary>
	/// Adds the node to renderables.
	/// </summary>
	/// <param name="n">The node to add.</param>
	void AddNodeToRenderables(WorldNode* n);

	unsigned int default_res;
	WorldNode tree;
	std::vector<WorldNode*> temp_process_queue;
	std::vector<DirtyNodeOutput> output_buffer;
	std::vector<WorldNode*> renderables;

	bool shutdown;
	
	//std::vector <TerrainChunk*> renderables_2;
};

