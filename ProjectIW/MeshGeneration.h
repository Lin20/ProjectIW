#pragma once

#include <vector>
#include <d3d11.h>
#include "ThreadPool.h"
#include "WorldTypes.h"

/// <summary>
/// This is the mesh generation stage of the pipeline.
/// It's a background process that reads from a queue and updates mesh for dirty nodes.
/// </summary>
class MeshGeneration
{
public:		
	/// <summary>
	/// Shuts the mesh generation down and forces it to stop generating mesh for new nodes.
	/// </summary>
	inline static void Shutdown() { shutdown = true; }


	/// <summary>
	/// Adds the node to the generation queue.
	/// </summary>
	/// <param name="node">The node to add.</param>
	static void AddNode(WorldNode* node);
	
	/// <summary>
	/// Generates the mesh and sets the node to READY status once it's finished.
	/// </summary>
	/// <param name="node">The node to process.</param>
	static void ProcessNode(WorldNode* node);

	static void Quit();

private:
	static ThreadPool threadpool;
	static std::atomic<bool> shutdown;
};

