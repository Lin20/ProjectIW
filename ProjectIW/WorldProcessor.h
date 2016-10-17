#pragma once

#include "WorldTree.h"
#include "NodeCreation.h"
#include "MeshGeneration.h"

/// <summary>
/// This controls the entire world pipeline.
/// </summary>
class WorldProcessor
{
public:
	WorldProcessor(unsigned int res);
	~WorldProcessor();
	
	/// <summary>
	/// Cleans up the world in the intended order.
	/// </summary>
	void Cleanup();
	
	/// <summary>
	/// Begins the world pipeline stage, starting with WorldTree.
	/// </summary>
	/// <param name="pos">The camera's position.</param>
	/// <returns>How long the entire pipeline took on this thread.</returns>
	unsigned int Update(Vector3 pos);

	inline std::vector<WorldNode*>& GetRenderables() { return main_tree.GetRenderables(); }

private:
	WorldTree main_tree;
	NodeCreation node_creation;
};

